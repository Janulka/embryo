/*
 *  libembryo
 *  Copyright (C) 2008 by Alexandre Devert
 *
 *           DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *                  Version 2, December 2004
 *
 *  Copyright (C) 2004 Sam Hocevar
 *  14 rue de Plaisance, 75014 Paris, France
 *  Everyone is permitted to copy and distribute verbatim or modified
 *  copies of this license document, and changing it is allowed as long
 *  as the name is changed.
 *
 *            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *  TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
 *
 *  0. You just DO WHAT THE FUCK YOU WANT TO.
 */

/*
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.
 
   Before using, initialize the state by using init_genrand(seed)  
   or init_by_array(init_key, key_length).
 
   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          
 
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
 
     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
 
     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
 
     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.
 
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 
   Any feedback is very welcome.
   http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
*/

#include <cstring>
#include "Randomizer.h"

using namespace embryo;



#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL
#define UPPER_MASK 0x80000000UL
#define LOWER_MASK 0x7fffffffUL



void
Randomizer::init(uint32_t inSeed) {
  mRegister[0] = inSeed & 0xffffffffUL;

  for(mIndex = 1; mIndex < N; mIndex++) {
    mRegister[mIndex] =
      (1812433253UL * (mRegister[mIndex - 1] ^ (mRegister[mIndex - 1] >> 30)) + mIndex);
    mRegister[mIndex] &= 0xffffffffUL;
  }
}



void
Randomizer::init(uint32_t* inArray, int inSize) {
  init(19650218UL);

  int i = 1;
  int j = 0;
  int k = (N > inSize ? N : inSize);

  for (; k != 0; k--) {
    mRegister[i] =
      (mRegister[i] ^ ((mRegister[i - 1] ^ (mRegister[i-1] >> 30)) * 1664525UL)) + inArray[j] + j;
    mRegister[i] &= 0xffffffffUL;
    i++;
    j++;

    if (i >= N) {
      mRegister[0] = mRegister[N-1];
      i = 1;
    }
    if (j >= inSize)
      j = 0;
  }

  for (k = N - 1; k; k--) {
    mRegister[i] =
      (mRegister[i] ^ ((mRegister[i-1] ^ (mRegister[i-1] >> 30)) * 1566083941UL)) - i;
    mRegister[i] &= 0xffffffffUL;
    i++;

    if (i >= N) {
      mRegister[0] = mRegister[N-1];
      i = 1;
    }
  }

  mRegister[0] = 0x80000000UL;
}



uint32_t
Randomizer::uint32() {
  int i;
  uint32_t lY;
  static  uint32_t lMag01[2] = {0x0UL , MATRIX_A};

  if (mIndex >= N) {
    for(i = 0; i < N - M; i++) {
      lY = (mRegister[i] & UPPER_MASK) | (mRegister[i + 1] & LOWER_MASK);
      mRegister[i] = mRegister[i + M] ^ (lY >> 1) ^ lMag01[lY & 0x1UL];
    }

    for(; i < N - 1; i++) {
      lY = (mRegister[i] & UPPER_MASK) | (mRegister[i + 1] & LOWER_MASK);
      mRegister[i] = mRegister[i + (M - N)] ^ (lY >> 1) ^ lMag01[lY & 0x1UL];
    }

    lY = (mRegister[N - 1] & UPPER_MASK) | (mRegister[0] & LOWER_MASK);
    mRegister[N - 1] = mRegister[M - 1] ^ (lY >> 1) ^ lMag01[lY & 0x1UL];

    mIndex = 0;
  }

  lY = mRegister[mIndex++];
  lY ^= (lY >> 11);
  lY ^= (lY << 7) & 0x9d2c5680UL;
  lY ^= (lY << 15) & 0xefc60000UL;
  lY ^= (lY >> 18);

  return lY;
}



double
Randomizer::real() {
  return double(uint32()) * (1.0 / 4294967295.0);
}
