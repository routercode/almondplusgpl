#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <time.h>

#include "errorlog.h"
#include "general.h"

#ifdef LOGGING_ON
#define _LOGGING_ON 1
#else
#define _LOGGING_ON 0
#endif

// Set this to 0 to have a unified debug / critical error logfile, 1 to separate them into two files.
#define SEPARATE_LOGS 0

const char *crit_sym_name = "latest_crit";
const char *debug_sym_name = "latest_debug";
FILE *criterr = NULL;
FILE *debuglog = NULL;


void DBG(const char *fmt, ...)
{
	if ( _LOGGING_ON ) {
		va_list arglist;
 
		va_start(arglist, fmt);
		if (!SEPARATE_LOGS) {
			fprintf(debuglog, "debug: ");
		}
		vfprintf(debuglog, fmt, arglist);
	}
}


void ERR(const char *fmt, ...)
{
	if ( _LOGGING_ON ) {
		va_list arglist;
 
		va_start(arglist, fmt);
		if (!SEPARATE_LOGS) {
			fprintf(criterr, "ERROR: ");
		}
		vfprintf(criterr, fmt, arglist);
	}
}


void open_logs(char *whoisit, int clean, int redirect_no)
{
	if ( _LOGGING_ON ) {
		if (redirect_no) {
			debuglog = stdout;
			criterr = stdout;
		} else {
			time_t time_var;
			struct tm tm;

			char filename[1024];
			char time_string[30];
			char symlink_name[50];

			if (clean) {
				unlink("logs");
			}
			mkdir("logs", 0xffff);
			chdir("logs");


			// Get the date/time string
			time_var = time(NULL);
			localtime_r(&time_var, &tm);
			snprintf(time_string, 30, "%04d_%02d_%02d_%02dh_%02dm_%02ds",
					 tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
					 tm.tm_hour, tm.tm_min, tm.tm_sec);

			// Assemble the name of the debug logfile
			snprintf(filename, 1024, "debug_%s_%s.log", whoisit, time_string);

			// Create the debug log file
			debuglog = fopen(filename, "w");
			if (debuglog == NULL) {
				fprintf(stderr, "Unable to open log file for debug statements, named: %s\n", filename);
				debuglog = stderr;
			} else {
				fprintf(stderr, "Successfully opened log file %s\n", filename);
			}

			// Create a symlink to the new log file
			snprintf(symlink_name, 50, "%s_%s", debug_sym_name, whoisit);
			unlink(symlink_name);
			symlink(filename, symlink_name);

			// If necessary, create a separate log file for critical errors.
			if (SEPARATE_LOGS) {
				// Assemble the name of the critical error logfile
				snprintf(filename, 1024, "crit_err_%s_%s.log", whoisit, time_string);

				// Create the critical error log file
				criterr = fopen(filename, "w");
				if (criterr == NULL) {
					fprintf(stderr, "Unable to open log file for critical errors named: %s\n", filename);
					criterr = stderr;
				} else {
					fprintf(stderr, "Successfully opened log file %s\n", filename);
				}

				// Create a symlink to the new log file
				snprintf(symlink_name, 50, "%s_%s", crit_sym_name, whoisit);
				unlink(symlink_name);
				symlink(filename, symlink_name);
			} else {
				criterr = debuglog;
			}

			chdir("..");
		}
		LOG(DBG("Debug / error logging initialized.\n"));
	}
}


void close_logs(void)
{
	if ( _LOGGING_ON ) {
		if (SEPARATE_LOGS) {
			fflush(criterr);
			if (criterr != stderr)
				fclose(criterr);
		}

		fflush(debuglog);
		if (debuglog != stderr)
			fclose(debuglog);
	}
}
