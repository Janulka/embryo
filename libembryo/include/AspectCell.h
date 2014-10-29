/*
 *  libembryo
 *  Copyright (C) 2010 by Jana Hlavacikova
 *
 */

#ifndef EMBRYO_ASPECTCELL_H
#define EMBRYO_ASPECTCELL_H
#include <list>


namespace embryo {
  class AspectCell {
    private:
      size_t mPositionX;
      size_t mPositionY;
      size_t mSize;
      double mAspect;
    public:
      AspectCell();
      AspectCell(size_t iPositionX, size_t iPositionY, size_t iSize, double iAspect);
      ~AspectCell();
      void setCell(size_t iPositionX, size_t iPositionY, size_t iSize, double iAspect);
      inline double getAspect() { return mAspect; }
      inline void setAspect(double iAspect) { mAspect = iAspect; }
      inline size_t getPositionX() { return mPositionX; }
      inline size_t getPositionY() { return mPositionY; }
      inline size_t getSize() { return mSize; }
  };
  
   typedef AspectCell * PAspectCell;
   typedef std::list<PAspectCell> AspectCellContainer;
   typedef AspectCellContainer::iterator AspectCellIterator;
   typedef AspectCellContainer::const_iterator AspectCellConstIterator;

}

#endif
