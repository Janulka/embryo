/*
 *  libembryo
 *  Copyright (C) 2010 by Jana Hlavacikova
 *
 */

#ifndef EMBRYO_CELL_H
#define EMBRYO_CELL_H
#include <list>
#include <math.h>
#include <ObjectFactory.h>
#include "GaussianDistribution.h"

namespace embryo {

    enum Direction {
        N = 0,
        NE = 1,
        E = 2,
        SE = 3,
        S = 4,
        SW = 5,
        W = 6,
        NW = 7,
        NOWHERE = 8
    };

    class Cell;

    typedef Cell * PCell;
    //  	  typedef Cell ** PPCell;
    typedef std::list<PCell> CellContainer;
    //	  typedef std::list<PPCell> PCellContainer;
    //	  typedef PCellContainer::iterator PCellIterator;
    typedef CellContainer::iterator CellIterator;
    typedef CellContainer::reverse_iterator CellReverseIterator;
    typedef CellContainer::const_iterator CellConstIterator;

    class Cell : public Object {
        public:
            Cell(size_t inNbStates, size_t inNbChemicals, size_t inRow, size_t inColumn, size_t iSize, size_t iIndex, bool iLeftBorder, bool iRightBorder, bool iUpperBorder, bool iLowerBorder, double iValue);
            Cell(size_t inNbStates, size_t inNbChemicals, size_t inRow, size_t inColumn, size_t iSize, size_t iIndex, bool iLeftBorder, bool iRightBorder, bool iUpperBorder, bool iLowerBorder, const Cell * iCell);
            ~Cell();

            void addNeighbor(int iDirection, size_t iSize);
            void addNeighbor(Cell * &iCell, int iDirection);
            void addNeighbor(Cell * &iCell, Direction iDirection);

            void divide();

            bool grow(CellIterator & oTargetCellsBegin, CellIterator & oTargetCellsEnd);//(Direction iDirection);
            //          bool getGrowingDirections(Direction & oDir1, Direction & oDir2, PCellIterator oTargetCellsBegin, PCellIterator oTargetCellsEnd);
            //          bool deleteNeighbors(Direction iDir1, Direction iDir2);
            //          bool clearNeighbors(Direction iDir);
            bool getGrowingDirections(Direction & oDir1, Direction & oDir2, Cell *& oCell1, Cell *& oCell2, Cell *& oCell3);
            bool getFirstNeighbor(Direction iDirection, Cell * & outCell);
            bool getSingleNeighbor(Direction iDirection, Cell * & outCell);
            inline void changePositionX(size_t x) { mPositionX = x; }
            inline void changePositionY(size_t y) { mPositionY = y; }
            inline void multiSize(size_t iMulti) { mSize *= iMulti; }
            inline void decreasePositionY(size_t value) { mPositionY -= sqrt(value); }
            inline void decreasePositionX(size_t value) { mPositionX -= sqrt(value); }
            void swapContent();

            inline void setVisited(bool ibValue) { mVisited = ibValue; }
            inline bool isVisited() { return mVisited; }
            bool wantsGrow();
            bool wantsDivide();
            void setGrowing(bool iBGrow); 
            void setDividing(bool iBDiv);
            void getChemicalList(CellIterator &itrBegin, CellIterator &itrEnd,  double * &oChemicalList, bool ibX, bool &ibIsNeighbor);

            void checkPositionX(size_t & ioSharedLength, size_t iPosx, size_t iSize);
            void checkPositionY(size_t & ioSharedLength, size_t iPosY, size_t iSize);

            bool getNeighbors(Direction iDirection, CellIterator & outCellBegin, CellIterator & outCellEnd);
            //CellContainer getNeighbors(int iDirection);
            void mergeMyNeighbors(Direction iWhichNeighborSide, CellContainer & iMergeCC);
            void changeAllMyNeighbors(Direction iWhichNeighborSide, CellIterator &iNewFirstCell, CellIterator &iNewLastCell);
            void changeAllMyNeighbors(Direction iWhichNeighborSide, CellIterator &iNewFirstCell);
            void clearAllMyNeighbors(Direction iWhichNeighborSide);
            void changeNeighborhood(Direction iFromWhichNeighborSide, Direction iToWhichNeighborSide, Cell *& iCellToChange, Cell *& iCellToRemove, Cell *& iNewCell);
            void changeNeighborhood(Direction iFromWhichNeighborSide, Direction iToWhichNeighborSide, Cell *& iCellToChange, Cell *& iNewCell);
            void changeNeighbor(Direction iWhichNeighborSide, Cell *& iCellToChange, Cell *& iCellToRemove, Cell *& iNewCell);
            void changeNeighbor(Direction iWhichNeighborSide, Cell *& iCellToChange, Cell *& iNewCell);
            void removeNeighbor(Direction iWhichNeighborSide, Cell *& iCellToRemove);

            inline size_t getSize() {return mSize;}
            inline size_t getPositionX() {return mPositionX; }
            inline size_t getPositionY() {return mPositionY; }

            //	  inline void setStateVector(double * iStateVector) { mStateVector = iStateVector; }


            inline double *& getStateVectorAdr() { return mStateVector; }
            inline double ** getPrevStateVectorAdr() { return &mPrevStateVector; }
            inline double ** getChemicalVectorAdr() { return &mChemicalVector; }
            inline double ** getPrevChemicalVectorAdr() { return &mPrevChemicalVector; }

            inline double * getStateVector() { return mStateVector; }
            inline double * getPrevStateVector() { return mPrevStateVector; }
            inline double * getChemicalVector() { return mChemicalVector; }
            inline double * getPrevChemicalVector() { return mPrevChemicalVector; }	  
            inline const double * getStateVector() const { return mStateVector; }
            inline const double * getPrevStateVector() const { return mPrevStateVector; }
            inline const double * getChemicalVector() const { return mChemicalVector; }
            inline const double * getPrevChemicalVector() const { return mPrevChemicalVector; }	  

            void init(double iValue);
            void scramble(Randomizer& inRandomizer, double inStdDev);
            bool blurFirstPass();
            bool blurSecondPass();

            void update(double* &outOldState, double** &outOldChemicalsList, double* &outNewState, double* &outNewChemicals, bool * &ibIsNeighbor); //, bool (&ibIsNeighbor)[5]);

            //          CellContainer & getContainer(Direction iDir);

            bool getContainer(Direction iDir, CellContainer & oContainer);

            size_t getIndex() { return mIndex;}
            bool isOnTheBorder(Direction iDir);
            void setAsBorder(Direction iDir);

            inline bool toDelete() { return mReadyToDelete; }
            inline void setToDelete() { mReadyToDelete = true; }
        protected:
            size_t mNbStates, mNbChemicals;
            double* mStateVector;
            double* mPrevStateVector;
            double* mChemicalVector;
            double* mPrevChemicalVector;

            double* mVectorsMem;

            CellContainer mUpperCell;	//set of cells one level above me
            CellContainer mLowerCell;
            CellContainer mLeftCell;
            CellContainer mRightCell;

            bool mLeftBorder;
            bool mRightBorder;
            bool mUpperBorder;
            bool mLowerBorder;

            size_t mSize; //size of the cell
            size_t mPositionX; 
            size_t mPositionY; 

            bool mVisited;   
            bool mBGrow;
            bool mBDivide;
            size_t mIndex;

            bool mReadyToDelete;
    };

}

#endif
