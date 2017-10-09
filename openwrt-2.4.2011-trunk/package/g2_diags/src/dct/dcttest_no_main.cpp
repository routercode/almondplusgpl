//
// Copyright (c) 2003 Timothy A. Seufert (tas@mindspring.com)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//


//
// dctest 2.1 -- Disk Corruption Tester
//
// $Id: dcttest_no_main.cpp,v 1.1 2011/07/12 18:47:37 acarr Exp $
//
// This program tests for data corruption during disk I/O.  Since it is
// an end to end test (goes through the file system) it often manages to
// find other kinds of hardware problems too (such as bad RAM).
//


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>

#include "dct.h"
#include "workqueue.h"
#include "PRBS23.h"


const char* copyright_banner =
    "Disk Corruption Tester (dctest) version 2.1\n"
    "Copyright (C) 2003 Timothy A. Seufert (tas@mindspring.com)\n";


typedef enum { CREATE, VERIFY } testmode;


//
// How large a work buffer is, in megabytes.  This value is approximate because
// the actual buffer size may be affected by the system's page size.
//
const uint32_t bufsize_Mbytes = 4;


//
// The number of uint32_ts in a work buffer.  This value is calculated by main().
//
uint32_t bufsize = 0;


//
// Global settings.
//
char* testfilename = "test.dat";
char* logfilename = "log.txt";
uint32_t testcount = 1;
uint32_t verifypasses = 1;
uint32_t filesize_MB = 100;
bool halt_on_error = false;
bool display_help = false;
uint32_t pagesize = 0;


//
// Settings derived from primary global settings above.
//
FILE* logfile = NULL;
off_t filesize = 0;


//
// Table of command line parameters
//

struct ptable_entry {
	char* name;
	char* alt_name;
	char* description;
	uint32_t* uint_param;
	bool* bool_param;
	char** string_param;
	bool interactive_entry_allowed;
};

ptable_entry ptable[] = {
	{ "--name", "-n", "Name of test file (default test.dat)",
	  NULL, NULL, &testfilename, true },

	{ "--log", "-l", "Name of log file (default log.txt)",
	  NULL, NULL, &logfilename, true },

	{ "--count", "-c", "Number of times to run test, 0 = infinite loop (default 1)",
	  &testcount, NULL, NULL, true },

	{ "--verify", "-v", "Number of verification passes per test (default 1)",
	  &verifypasses, NULL, NULL, true },

	{ "--size", "-s", "Size of test file, in megabytes (default 100)",
	  &filesize_MB, NULL, NULL, true },

	{ "--halt", NULL, "Halt if corruption is detected, leaving test file intact",
	  NULL, &halt_on_error, NULL, true },

	{ "--help", "-h", "Display help",
	  NULL, &display_help, NULL, false },

	{ NULL, NULL, NULL, NULL, NULL, NULL, false }
};


//
// Replacement for strdup, since strdup is not present on all systems
//
char* dupstr(char* s)
{
	int len = strlen(s) + 1;
	char* copy = (char*) malloc(len);
	if (copy != NULL) memcpy(copy, s, len);
	return copy;
}


//
// Account for lack of getpagesize() call under "classic" MacOS
//
#ifdef MPW
int getpagesize(void)
{
	return 4096;
}
#endif


//
// Some silliness necessary to compensate for a certain compiler that believes
// functions in a C++ source file have a different type signature than ordinary
// C functions, and therefore barfs when you try to use said functions as
// parameters to APIs that were defined in C source files...
//
extern "C" {
	typedef void* (*pthread_fn_ptr)(void *);
	typedef void (*sig_fn_ptr)(int);
};


//
// Fatal error handler.
//
void fatal(const char *fmt, ...)
{
	va_list arglist;
	va_start(arglist, fmt);
	vfprintf(stderr, fmt, arglist);

	//
	// Flush stdout and logfile so that nothing gets lost.
	//
	fflush(stdout);
	if (logfile != NULL) {
		fflush(logfile);
	}

	exit(-1);
}


//
// Prints the same message to the console and to the logfile.
//
void print_and_log(const char* fmt, ...)
{
	static char pbuf[512];

	va_list arglist;
	va_start(arglist, fmt);
	vsnprintf(pbuf, 512, fmt, arglist);
	fputs(pbuf, stdout);
	fputs(pbuf, logfile);
}


//
// Signal handler function (clean up before exiting)
//
void sigcleanup(int sig)
{
	//
	// Flush stdout and logfile so that nothing gets lost.
	//
	fflush(stdout);
	if (logfile != NULL) {
		fflush(logfile);
	}

	//
	// Reraise the signal so it kills us.
	//
	signal(sig, SIG_DFL);
	raise(sig);
}


//
// Timestamp function for computing throughput
//
double timestamp()
{
	struct timeval tv;
	struct timezone tz;

	gettimeofday(&tv, &tz);
	return ( (double) tv.tv_sec + (double) tv.tv_usec * 1e-6 );
}


//
// Utility functions for displaying progress indicators
//
off_t progress_amount_displayed;
off_t progress_delta;
char progress_mark;

void init_progress_display(off_t worklen)
{
	progress_amount_displayed = 0;
	progress_delta = worklen / 64;
	progress_mark = '.';
}

void display_progress(off_t progress)
{
	if (progress >= (progress_amount_displayed + progress_delta)) {
		while (progress >= (progress_amount_displayed + progress_delta)) {
			progress_amount_displayed += progress_delta;
			putchar(progress_mark);
		}
		fflush(stdout);
	}
	progress_mark = '.';
}

void print_rate(off_t len, double elapsed_time)
{
	// get the size into megabytes, then print the rate
	double megabytes = len * sizeof(uint32_t) / 1048576.0;
	printf(" (%.1f MB/s)\n", megabytes / elapsed_time);
}


//
// Fill a buffer with PRBS23 data.
//
void fillbuf(uint32_t* buf, uint32_t buflen, uint32_t& shiftreg)
{
	register uint32_t s = shiftreg;

	for (uint32_t i = 0; i < buflen; i++) {
		buf[i] = s;
		s = PRBS23_Next_Value(s);
	}

	shiftreg = s;
}


//
// Check that a buffer contains PRBS23 data.
//
void checkbuf(uint32_t* buf, uint32_t buflen, uint32_t& shiftreg, int& found_corruption, off_t offset)
{
	register uint32_t s = shiftreg;

	for (uint32_t i = 0; i < buflen; i++) {
		if (buf[i] != s) {
			fprintf(logfile, "\tERROR: At file offset %016llX, expected %08X, got %08X\n",
					offset + ((off_t)i * sizeof(uint32_t)), s, buf[i]);
			found_corruption = 1;
			progress_mark = 'E';
		}
		s = PRBS23_Next_Value(s);
	}

	shiftreg = s;
}


//
// Structure for communicating information to worker threads
//
typedef struct workorder {
	workqueue* q;
	testmode mode;
	int fd;
	off_t filesize;
};


//
// Subclass qelem to create a work buffer class.
//
class wbuf : public qelem {
public:
	wbuf() {
		//
		// buf should be page aligned for best performance.  So far, have not needed to
		// explicitly align it since many malloc implementations return page aligned
		// buffers when the requested block of memory is larger than a page.  Should
		// use posix_memalign instead of malloc on systems which support it.
		//
		buf = (uint32_t*) malloc(bufsize * sizeof(uint32_t));
	}
	virtual ~wbuf() {
		free(buf);
	}

	uint32_t buflen;
	uint32_t* buf;
};


//
// Producer thread for testfile creation and verification.
//
void* producer_fn(void* param)
{
	workorder* wo = (workorder*) param;
	workqueue* q = wo->q;

	uint32_t shiftreg = PRBS23_INITIAL_VALUE;
	off_t remaining_work = wo->filesize;
	wbuf* workbuf;

	while (remaining_work > 0) {
		workbuf = (wbuf*) q->get();

		workbuf->buflen = (remaining_work > bufsize) ? (bufsize) : (remaining_work);

		if (wo->mode == VERIFY) {
			read(wo->fd, workbuf->buf, workbuf->buflen * sizeof(uint32_t));
		} else {
			fillbuf(workbuf->buf, workbuf->buflen, shiftreg);
		}

		remaining_work -= workbuf->buflen;
		q->enqueue(workbuf);
	}

	// Tell consumer to shut down by sending it a workbuf with no work.
	workbuf = (wbuf*) q->get();
	workbuf->buflen = 0;
	q->enqueue(workbuf);

	return ((void*) 0);
}


//
// Consumer thread for testfile creation and verification.
//
void* consumer_fn(void* param)
{
	workorder* wo = (workorder*) param;
	workqueue* q = wo->q;

	uint32_t shiftreg = PRBS23_INITIAL_VALUE;
	off_t progress = 0;
	int found_corruption = 0;

	init_progress_display(wo->filesize * sizeof(uint32_t));

	while (1) {
		wbuf* workbuf = (wbuf*) q->dequeue();

		if (workbuf->buflen == 0) {
			q->release(workbuf);
			break;
		}

		if (wo->mode == VERIFY) {
			checkbuf(workbuf->buf, workbuf->buflen, shiftreg, found_corruption, progress);
		} else {
			write(wo->fd, workbuf->buf, workbuf->buflen * sizeof(uint32_t));
		}

		progress += (workbuf->buflen * sizeof(uint32_t));
		q->release(workbuf);
		display_progress(progress);
	}

	return ((void*) found_corruption);
}


//
// process_testfile()
//
// Creates (mode == CREATE) or verifies (mode == VERIFY) a testfile named filename,
// filesize uint32_ts long.  Uses worker threads to accomplish the actual work,
// overlapping I/O with computation.
//
// If used in VERIFY mode, returns 1 if any corruption was encountered during
// the test.  Otherwise, returns 0.
//
// File size parameter is in units of uint32_t, not char.
//
int process_testfile(testmode mode, char* filename, off_t filesize)
{
	int found_corruption = 0;
	pthread_t producer, consumer;
	pthread_attr_t attr;
	int fd;

	if (mode == VERIFY) {
		fd = open(filename, O_RDONLY, 0);
	} else {
		fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0644);
	}
	if (fd == -1) {
		fatal("Error: Couldn't open test file (%s).\n", filename);
	}

	// On MacOS X, turn off caching.  Ideally this should be done on any OS that
	// supports it, since cache can defeat end-to-end testing.
#if defined(__APPLE__) && defined(__MACH__)
	fcntl(fd, F_NOCACHE, 1);
#endif

	wbuf* buffers = new wbuf[2];

	workqueue* q = new workqueue;

	workorder* wo = new workorder;
	wo->q = q;
	wo->mode = mode;
	wo->fd = fd;
	wo->filesize = filesize;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	int p_err = pthread_create(&producer, &attr, (pthread_fn_ptr) producer_fn, (void*) wo);
	int c_err = pthread_create(&consumer, &attr, (pthread_fn_ptr) consumer_fn, (void*) wo);

	if (p_err || c_err) {
		fatal("Error while creating worker threads.\n");
	}

	double start_time = timestamp();

	// Give the work queue some empty buffers to get things moving
	q->release(&(buffers[0]));
	q->release(&(buffers[1]));

	// Now wait for the threads to finish
	p_err = pthread_join(producer, (void**)(&found_corruption));
	c_err = pthread_join(consumer, (void**)(&found_corruption));

	double end_time = timestamp();

	if (p_err || c_err) {
		fatal("pthread_join() error.\n");
	}

	print_rate(filesize, end_time - start_time);

	pthread_attr_destroy(&attr);

	delete wo;
	delete q;
	delete [] buffers;

	close(fd);

	return found_corruption;
}


void print_usage(void)
{
	printf("\n%s\n"
		   "Usage: dctest option1 <parm1> option2 <parm2> ...\n"
		   "\n"
		   "Options:\n",
		   copyright_banner);

	for (int p = 0; ptable[p].name != NULL; p++) {
		printf("  %s", ptable[p].name);
		if (ptable[p].uint_param != NULL) {
			printf(" N");
		} else if (ptable[p].string_param != NULL) {
			printf(" filename");
		}
		if (ptable[p].alt_name != NULL) {
			printf("  /  %s", ptable[p].alt_name);
			if (ptable[p].uint_param != NULL) {
				printf(" N");
			} else if (ptable[p].string_param != NULL) {
				printf(" filename");
			}
		}
		printf("\n        %s\n\n", ptable[p].description);
	}

	printf("Example: Run test 5 times, logging to a file named \"foo.txt\":\n"
		   "  dctest --c 5 --log foo.txt\n\n");
}


int process_argument(char* arg, char* nextarg)
{
	int arguments_processed = 1;

	for (int p = 0; ; p++) {
		if (ptable[p].name == NULL) {
			print_usage();
			fatal("Error: Unknown parameter %s\n\n", arg);
		}

		bool match = (strcmp(arg, ptable[p].name) == 0);
		if (ptable[p].alt_name != NULL) {
			match |= (strcmp(arg, ptable[p].alt_name)==0);
		}

		if (match) {
			if (ptable[p].bool_param != NULL) {
				*(ptable[p].bool_param) = true;
			} else {
				if (nextarg == NULL) {
					print_usage();
					fatal("Error: Missing parameter for argument %s .\n\n", arg);
				}

				if (ptable[p].uint_param != NULL) {
					sscanf(nextarg, "%u", ptable[p].uint_param);
				} else if (ptable[p].string_param != NULL) {
					*(ptable[p].string_param) = dupstr(nextarg);
				}
				arguments_processed = 2;
			}
			break;
		}
	}

	return arguments_processed;
}


void read_arguments(void)
{
	char param[512];
	char* token;

	printf("\nEnter test parameters (hit return to accept default shown in parentheses)\n");
	for (int p = 0; ptable[p].name != NULL; p++) {
		if (ptable[p].interactive_entry_allowed == true) {
			printf("%s%s: ", ptable[p].description,
				   (ptable[p].bool_param != NULL) ? (" [y/n] (n)") : (""));
			fgets(param, 512, stdin);
			token = strtok(param, " \f\n\r\t\v");
			if (token != NULL) {
				if (ptable[p].uint_param != NULL) {
					sscanf(token, "%u", ptable[p].uint_param);
				} else if (ptable[p].string_param != NULL) {
					*(ptable[p].string_param) = dupstr(token);
				} else if (ptable[p].bool_param != NULL) {
					*(ptable[p].bool_param) = (token[0] == 'y' || token[0] == 'Y');
				}
			}
		}
	}
}


int dct_main(void)
{
	int return_val = 0, err;
	int argc=0;
	char **argv;

	//
	// Install signal handler to perform cleanup before exit.
	//
	signal(SIGINT, (sig_fn_ptr) sigcleanup);
	signal(SIGHUP, (sig_fn_ptr) sigcleanup);
	signal(SIGTERM, (sig_fn_ptr) sigcleanup);

	
	if (argc < 2) {
		printf("\n%s\nNo parameters supplied.  Entering interactive mode.\n"
			   "For help on command line params, use --help switch.\n"
			   "Hit Control-C at any time to exit program.\n", copyright_banner);
		read_arguments();
	} else {
		for (int i = 1; i < argc; ) {
			i += process_argument(argv[i], argv[i+1]);
		}
	}

	if (display_help) {
		print_usage();
		return 0;
	}

	if (testcount < 0 || testcount > 10000) {
		fatal("\nError: Test count value %d is out of range (0 - 10000)\n\n", testcount);
	}
	if (verifypasses < 1 || verifypasses > 100) {
		fatal("\nError: Verification passes value %d is out of range (1 - 100)\n\n", verifypasses);
	}
	if (filesize_MB < 1 || filesize_MB > 16777216) {
		fatal("\nError: Test file size %d is out of range (1 - 16777216)\n\n", filesize_MB);
	}

	//
	// In many operating systems it is advantageous to do file I/O in units of pages,
	// using buffers which start on page boundaries.  Therefore we shall make the buffer
	// size a multiple of the page size and a multiple of the unit of work (uint32_t),
	// while trying to keep it at least as large as the bufsize_Mbytes constant.
	//
	pagesize = getpagesize();
	uint32_t base_size = pagesize * sizeof(uint32_t);
	bufsize = ((0x100000 * bufsize_Mbytes) / base_size) + 1;
	bufsize = bufsize * base_size;

	//
	// For the purposes of the rest of the program, the buffer size is the number of uint32_ts
	// in a buffer, not the number of bytes.
	//
	bufsize /= sizeof(uint32_t);

	//
	// Translate the file size from megabytes to uint32_ts.
	//
	filesize = (off_t) filesize_MB * (1048576 / sizeof(uint32_t));

	//
	// Create the log file.
	//
	logfile = fopen(logfilename, "a");
	if (logfile == NULL) {
		fatal("Error: Couldn't open log file (%s).\n", logfilename);
	}

	time_t t = time(NULL);

	fprintf(logfile,
			"\n============================================================\n"
			"%s\n"
			"Start time: %s\n",
			copyright_banner,
			ctime(&t));
	fprintf(logfile,
			"Test parameters:\n\t"
			"Test file size is %d megabytes\n\t",
			filesize_MB);
	fprintf(logfile,
			"%d%s test iteration%s\n\t",
			testcount,
			(testcount == 0) ? (" (infinite loop)") : (""),
			(testcount == 1) ? ("") : ("s") );
	fprintf(logfile,
			"%d verification pass%s per test iteration\n",
			verifypasses,
			(verifypasses == 1) ? ("") : ("es") );

	//
	// Test loop.
	//
	for (uint32_t testnum = 1; (testnum <= testcount) || (testcount == 0); testnum++) {
		print_and_log("\nBeginning test iteration #%d; creating test file...", testnum);
		putchar('\n');

		process_testfile(CREATE, testfilename, filesize);

		fprintf(logfile, " (done)\n");

		for (uint32_t pass = 1; pass <= verifypasses; pass++) {
			printf("Verifying integrity of test file, pass #%d\n", pass);

			err = process_testfile(VERIFY, testfilename, filesize);

			fprintf(logfile, "Completed test #%d verification pass #%d (%s)\n",
					testnum, pass, (err) ? "FAIL" : "OK");

			if (err) {
				printf("Warning, corruption detected!  Check logfile for details.\n");
				return_val = 1;

				if (halt_on_error) {
					print_and_log("\nHalting test.\n");
					fclose(logfile);
					logfile = NULL;
					return 1;
				}
			}
		}
	}

	print_and_log("\nFinished testing, deleting test file.\n\n");
	if (remove(testfilename)) {
		print_and_log("Warning: test file could not be deleted!\n\n");
	}

	fclose(logfile);

	return return_val;
}
