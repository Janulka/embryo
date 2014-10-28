/*
 *  libembryo
 *  Copyright (C) 2010 by Jana Hlavacikova
 *
 */

#ifndef EMBRYO_ASPECTCELL_H
#define EMBRYO_ASPECTCELL_H
#include <list>

//using namespace std;

namespace embryo {
  
//   typedef AspectCell * PAspectCell;
//   typedef std::List<PAspectCell> AspectCellContainer;
//   typedef AspectCellContainer::iterator AspectCellIterator;
  
  class AspectCell {
    private:
      int mPositionX;
      int mPositionY;
      int mSize;
      double mAspect;
    public:
      AspectCell();
      AspectCell(int iPositionX, int iPositionY, int iSize, double iAspect);
      ~AspectCell();
      void setCell(int iPositionX, int iPositionY, int iSize, double iAspect);
      inline double getAspect() { return mAspect; }
      inline void setAspect(double iAspect) { mAspect = iAspect; }
      inline int getPositionX() { return mPositionX; }
      inline int getPositionY() { return mPositionY; }
      inline int getSize() { return mSize; }
  };
  
   typedef AspectCell * PAspectCell;
   typedef std::list<PAspectCell> AspectCellContainer;
   typedef AspectCellContainer::iterator AspectCellIterator;
   typedef AspectCellContainer::const_iterator AspectCellConstIterator;

}

#endif
