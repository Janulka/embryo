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

#ifndef EMBRYO_RANDOMIZER_H
#define EMBRYO_RANDOMIZER_H

#include <stdint.h>



namespace embryo {
  /*
   * An implementation of the "Mersenne Twister" pseudo-random number generator,
   * Takuji Nishimura and Makoto Matsumoto.
   */

  class Randomizer {
  private:
    int mIndex;
    uint32_t mRegister[624];

  public:
    void init(uint32_t inSeed);

    void init(uint32_t* inArray, int inSize);

    uint32_t uint32();

    // Allow to be used with the STL
    inline int operator () (int inMax) {
      return uint32() % inMax;
    }

    // Generate a real number with uniform distribution over [0, 1)
    double real();
    
    double gaussian(double inStdDev);
  }; // class Randomizer
} // namespace embryo



#endif /* EMBRYO_RANDOMIZER_H */
