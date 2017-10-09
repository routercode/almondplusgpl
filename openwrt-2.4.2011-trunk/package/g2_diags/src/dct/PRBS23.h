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
// $Id: PRBS23.h,v 1.1 2011/07/12 18:47:37 acarr Exp $
//


#ifndef __PRBS23_H__
#define __PRBS23_H__


#include <stdint.h>


//
// The first four bytes of a PRBS23 sequence.  Prime a shift register
// with this value to begin computing the sequence.
//
const uint32_t PRBS23_INITIAL_VALUE = 0xFFFFFE00;


//
// Given a PRBS23 shift register as input, compute the next value of
// the shift register (i.e. the next 32 bits of the PRBS23 sequence).
//
// Note that the 32-bit chunks of PRBS23 data are big-endian, i.e. the most
// significant bits in a 32-bit chunk come first in the PRBS23 sequence.  If
// this code is used on little endian processors, it will be necessary to
// byteswap the 32-bit chunks before storing them to memory structures.
//
static inline uint32_t PRBS23_Next_Value(uint32_t current)
{
	return ( (((current << 9) ^ (current << 14)) & 0xFFFF0000) |
	         (((current >> 14) ^ (current >> 4)) & 0x0000FFFF) );
}


//
// Notes on the above PRBS23 pseudorandom bit sequence generator:
//
// PRBSn generators are serial shift registers n bits long with
// feedback taps at two or more bit positions.  The initial value
// of the shift register is defined to be all ones.
//
// The taps for PRBS23 are 23, 18.  The serial equation for PRBS23 is thus:
//
//    Bit[N] = Bit[N-23] ^ Bit[N-18]               (Eqn. 1)
//
// where Bit[N] refers to Bit #N of the serial PRBS23 sequence.
//
// This implementation stores a 32-bit chunk of the PRBS23 bit stream in a
// register.  Each iteration through the loop calculates the next 32 bits
// of the PRBS23 sequence.
//
// The upper 18 bits of the new 32-bit shift register value are
// computed in parallel:
//
//    new = ((old >> 23) ^ (old >> 18)) << 32
//
// This expression is simplified by merging the final left shift into
// the two right shifts (avoids use of 64-bit temp registers):
//
//    new = (old >> (23-32)) ^ (old >> (18-32))
//        = (old >> -9) ^ (old >> -14)
//        = (old << 9) ^ (old << 14)               (Eqn. 2)
//
// Eqn. 2 cannot directly compute the lower 14 bits of the new value, since
// they all depend on one or two bits in the upper 18 bits of the new value.
// To get around this problem, we substitute Eqn. 1 into itself, and simplify:
//
//    Bit[N] = (Bit[N-23-23] ^ Bit[N-23-18]) ^ (Bit[N-18-23] ^ Bit[N-18-18])
//    Bit[N] = (Bit[N-46] ^ Bit[N-41]) ^ (Bit[N-41] ^ Bit[N-36])
//    Bit[N] = Bit[N-46] ^ (Bit[N-41] ^ Bit[N-41]) ^ Bit[N-36]
//    Bit[N] = Bit[N-46] ^ 0 ^ Bit[N-36]
//    Bit[N] = Bit[N-46] ^ Bit[N-36]
//
// In the 32-bit parallel world, this gives us:
//
//    new = ((old >> 46) ^ (old >> 36)) << 32
//        = (old >> 14) ^ (old >> 4)               (Eqn. 3)
//
// Now we can calculate the upper 16 bits with Eqn. 2 and the lower 16
// with Eqn. 3.
//


#endif
