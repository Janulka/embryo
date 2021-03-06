#include <unistd.h>
#include <stdint.h>
#include <ctime>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <vector>
#include "random.h"

using namespace std;
using namespace cmaes;



/*
 * "SuperFastHash" hash function by Paul Hsieh
 */

#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif



#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif



uint32_t
hash(const char* inString, size_t inSize) {
  uint32_t lHash = inSize, tmp;

  size_t lRemainder = inSize & 3;
  inSize >>= 2;

  // Main loop
  for (; inSize != 0; inSize--) {
    lHash  += get16bits(inString);
    tmp    = (get16bits(inString+2) << 11) ^ lHash;
    lHash   = (lHash << 16) ^ tmp;
    inString  += 2*sizeof (uint16_t);
    lHash  += lHash >> 11;
  }

  // Handle end cases
  switch(lRemainder) {
  case 3:
    lHash += get16bits (inString);
    lHash ^= lHash << 16;
    lHash ^= inString[sizeof(uint16_t)] << 18;
    lHash += lHash >> 11;
    break;
  case 2:
    lHash += get16bits(inString);
    lHash ^= lHash << 11;
    lHash += lHash >> 17;
    break;
  case 1:
    lHash += *inString;
    lHash ^= lHash << 10;
    lHash += lHash >> 1;
  }

  // Force "avalanching" of final 127 bits
  lHash ^= lHash << 3;
  lHash += lHash >> 5;
  lHash ^= lHash << 4;
  lHash += lHash >> 17;
  lHash ^= lHash << 25;
  lHash += lHash >> 6;

  return lHash;
}



/*
 * Build a 32 bits number, in order to have a different number, even if 
 * from two processes launched in the same time.
 */

uint32_t
buildSeed() {
  std::vector<char> lIdString;

  // Concatenate the system date
  {
    time_t lTime = time(0);
    for(size_t i = 0; i < sizeof(time_t); ++i)
      lIdString.push_back(((char*)(&lTime))[i]);
  }

  // Concatenate the processus PID
  {
    pid_t lPid = getpid();
    for(size_t i = 0; i < sizeof(pid_t); ++i)
      lIdString.push_back(((char*)(&lPid))[i]);
  }

  // Concatenate the parent processus PID
  {
    pid_t lPid = getppid();
    for(size_t i = 0; i < sizeof(pid_t); ++i)
      lIdString.push_back(((char*)(&lPid))[i]);
  }

  char* lTmp = new char[lIdString.size()];
  for(size_t i = 0; i < lIdString.size(); ++i)
    lTmp[i] = lIdString[i];

  //  Hash the string
  uint32_t lResult = hash(lTmp, lIdString.size());

  // Job done
  delete[] lTmp;
  return lResult;
}



long
randomizer::init(long unsigned inSeed) {
  flgstored = 0;
  if (inSeed < 1)
    inSeed = buildSeed();

  return start(inSeed);
}



long
randomizer::start(long unsigned inSeed) {
  long tmp;
  int i;

  flgstored = 0;
  startseed = inSeed;
  if (inSeed < 1)
    inSeed = 1;
  aktseed = inSeed;

  for (i = 39; i >= 0; --i) {
    tmp = aktseed/127773;
    aktseed = 16807 * (aktseed - tmp * 127773)
                      - 2836 * tmp;
    if (aktseed < 0)
      aktseed += 2147483647;
    if (i < 32)
      rgrand[i] = aktseed;
  }
  aktrand = rgrand[0];

  return inSeed;
}




double
randomizer::gauss() {
  double x1, x2, rquad, fac;

  if (flgstored) {
    flgstored = 0;
    return hold;
  }
  do {
    x1 = 2.0 * uniform() - 1.0;
    x2 = 2.0 * uniform() - 1.0;
    rquad = x1*x1 + x2*x2;
  } while(rquad >= 1 || rquad <= 0);
  
  fac = sqrt(-2.0*log(rquad)/rquad);
  flgstored = 1;
  hold = fac * x1;
  return fac * x2;
}



double
randomizer::uniform() {
  long tmp;

  tmp = aktseed/127773;
  aktseed = 16807 * (aktseed - tmp * 127773) - 2836 * tmp;
  if (aktseed < 0)
    aktseed += 2147483647;
  tmp = aktrand / 67108865;
  aktrand = rgrand[tmp];
  rgrand[tmp] = aktseed;
  return (double)(aktrand)/(2.147483647e9);
}
