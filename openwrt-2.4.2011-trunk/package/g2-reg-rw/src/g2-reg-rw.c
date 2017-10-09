/*
 * Implement a SPY server that can handle register read/write requests
 * from the REG web application.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char *usage = "<address> [<data>] : if only <address> is specified, read; if <address> and <data> specified, write";

static int   x_debug = 0;

extern int          rw_init( unsigned int base, unsigned int sz );
extern unsigned int rw_read( unsigned int address );
extern void         rw_write( unsigned int address, unsigned int data );

int main( int argc, char **argv )
{
  unsigned int base = 0xf0000000;
  unsigned int sz   = 0x06840300;

  int rd = 1;
  unsigned int addr = -1;
  unsigned int data = 0;

  if ( argc == 1 && strcmp( argv[1], "-h" ) == 0 ) {
    printf( "%s\n", usage );
    exit( 0 );
  }

  if ( argc == 2 ) {
    rd = 1;
    addr = (unsigned) strtoll( argv[1], NULL, 16 );
  }
  else if ( argc == 3 ) {
    rd = 0;
    addr = (unsigned) strtoll( argv[1], NULL, 16 );
    data = (unsigned) strtoll( argv[2], NULL, 16 );
  }
  else {
    printf( "%s\n", usage );
    exit( 1 );
  }

  addr -= base;

  if ( x_debug ) 
    printf( "debug: operation: '%s', address: 0x%08x, data: 0x%08x\n",
	    ((rd==1)?"read":"write"), addr, data );

  if ( rw_init( base, sz ) == 0 ) {
    printf( "Failed to initialize device memory map!\n" );
    exit( 1 );
  }

  unsigned int value = data;
  if ( rd == 1 ) {
    // its a read
    value = rw_read( addr );
    printf( "0x%08x\n", value );
  }
  else {
    // its a write
    rw_write( addr, data );
  }

  rw_close();
  exit( 0 );
}


