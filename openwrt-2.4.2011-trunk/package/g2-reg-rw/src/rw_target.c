#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

// This routine should perform any initialization (for example, initialize mmap).
//
// Returns 0 on failure, non-zero on success.
//

static unsigned char *mapbase = NULL;
static unsigned int mapsz = 0;

int rw_init( unsigned int base, unsigned int sz )
{
  // Might do something like:
  //
  // fd = open (dev_mmap, O_RDWR | O_SYNC)
  // mem = mmap (0, MAX_MMAP_SIZE,
  //             PROT_READ | PROT_WRITE, MAP_SHARED, fd, 
  //             device_base_address)
  //
  mapsz = sz;

  int pfd = -1;
  if ((pfd = open ("/dev/mem", O_RDWR|O_SYNC)) == -1) {
    fprintf(stderr, "ERROR: g2-reg-rw: Failed to open /dev/mem!\n");
    return 0;
  }

  mapbase = (unsigned char*)mmap( NULL,
				  sz,
				  PROT_READ | PROT_WRITE, MAP_SHARED,
				  pfd,
				  base );
  if ( mapbase == MAP_FAILED ) {
    fprintf(stderr, "ERROR: g2-reg-rw: failed to map memory\n" );
  }

  close( pfd );

  return 1;
}

// Read a register.  Return the value.  The return
// is expected to be 32-bits wide.
//
unsigned int rw_read( unsigned int address )
{
  return *(unsigned int *)(mapbase + address);
}

// Write a register.
//
void rw_write( unsigned int address, unsigned int data )
{
  *(unsigned int *)(mapbase + address) = data;
}

void rw_close()
{
  munmap( mapbase, mapsz );
}

