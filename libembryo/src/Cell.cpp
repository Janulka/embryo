/*
 *  libembryo
 *  Copyright (C) 2010 by Jana Hlavacikova
 *
 */

#include <algorithm>

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include "Cell.h"
#include "MLPController.h"
#include "ESNController.h"
#include "ArrayOps.h"
// #include "GaussianDistribution.h"

using namespace std;
using namespace embryo;

class exceptionNeighbors : public std::exception {
public:

    virtual const char* what() const throw () {
        return "error by trying diffusion - there are no neighbors. This exception can be used in this sense only when the sides lengths are not mod. 4";
    }
};

class exceptionLength : public std::exception {
public:

    virtual const char* what() const throw () {
        return "the sum of the shared length of neighbors differs from the length of the cell";
    }
};

Cell::Cell(size_t inNbStates, size_t inNbChemicals, size_t inRow, size_t inColumn, size_t iSize, size_t iIndex, bool iLeftBorder, bool iRightBorder, bool iUpperBorder, bool iLowerBorder, double iValue) {
    // Allocate memory for the states and the chemicals
    mVectorsMem = new double[2 * (inNbStates + inNbChemicals)];

    mStateVector = mVectorsMem;
    mPrevStateVector = mStateVector + inNbStates;
    mChemicalVector = mPrevStateVector + inNbStates;
    mPrevChemicalVector = mChemicalVector + inNbChemicals;

    mSize = iSize;
    mPositionX = inColumn;
    mPositionY = inRow;

    mNbStates = inNbStates;
    mNbChemicals = inNbChemicals;

    //    init();
    init(iValue);

    mVisited = false;
    mBGrow = false;
    mBDivide = false;
    mIndex = iIndex;
    mLeftBorder = iLeftBorder;
    mLowerBorder = iLowerBorder;
    mUpperBorder = iUpperBorder;
    mRightBorder = iRightBorder;
    mReadyToDelete = false;
}

//Cell::Cell(size_t inNbStates, size_t inNbChemicals, size_t inRow, size_t inColumn, size_t iSize, size_t iIndex, Cell * iCell) {

Cell::Cell(size_t inNbStates, size_t inNbChemicals, size_t inRow, size_t inColumn, size_t iSize, size_t iIndex, bool iLeftBorder, bool iRightBorder, bool iUpperBorder, bool iLowerBorder, const Cell * iCell) {
    // Allocate memory for the states and the chemicals
    mVectorsMem = new double[2 * (inNbStates + inNbChemicals)];

    mStateVector = mVectorsMem;
    mPrevStateVector = mStateVector + inNbStates;
    mChemicalVector = mPrevStateVector + inNbStates;
    mPrevChemicalVector = mChemicalVector + inNbChemicals;

    mSize = iSize;
    mPositionX = inColumn;
    mPositionY = inRow;

    mNbStates = inNbStates;
    mNbChemicals = inNbChemicals;

    const double * lState = iCell->getStateVector();
    const double * lPrevState = iCell->getPrevStateVector();
    const double * lChem = iCell->getChemicalVector();
    const double * lPrevChem = iCell->getPrevChemicalVector();
    arrayd::copy(mStateVector, lState, mNbStates);
    arrayd::copy(mPrevStateVector, lPrevState, mNbStates);
    arrayd::copy(mChemicalVector, lChem, mNbChemicals);
    arrayd::copy(mPrevChemicalVector, lPrevChem, mNbChemicals);

    mVisited = false;
    mBGrow = false;
    mBDivide = false;
    mIndex = iIndex;

    mLeftBorder = iLeftBorder;
    mLowerBorder = iLowerBorder;
    mUpperBorder = iUpperBorder;
    mRightBorder = iRightBorder;
    //    mLeftBorder = iCell->isOnTheBorder(W);
    //    mRightBorder = iCell->isOnTheBorder(E);
    //    mUpperBorder = iCell->isOnTheBorder(N);
    //    mLowerBorder = iCell->isOnTheBorder(S);

    mReadyToDelete = false;
}

Cell::~Cell() {
    //   delete[] mUpperCell;
    //   delete[] mLowerCell;
    //   delete[] mLeftCell;
    //   delete[] mRightCell;

    //cycling in deleting neighbors
    //    printf("index:%d\n", mIndex);
    //    CellIterator itr;
    //    CellContainer cc = mUpperCell;
    //    if (!cc.empty()) {
    //        printf("cc size:%d\n", cc.size());
    //        int size = cc.size();
    //        itr = cc.begin();
    //        //while ((itr != cc.end()) || (*itr != NULL)) {
    //        int counter = 0;
    //        while (++counter <= size) {
    //            printf("counter:%d\n", counter);
    //            Cell *& c = *itr;
    //            if (c != NULL) {
    //                delete c; //(*itr);
    //                c = NULL;
    //            }
    //            itr = cc.erase(itr);
    //            ++itr;
    //        }
    //    }
    //
    //    cc = mLowerCell;
    //    if (!cc.empty()) {
    //        printf("cc size:%d\n", cc.size());
    //        itr = cc.begin();
    //        //        while ((itr != cc.end()) || (*itr != NULL)) {
    //        int size = cc.size();
    //        int counter = 0;
    //        while (++counter <= size) {
    //            printf("counter:%d\n", counter);
    //            Cell *& c = *itr;
    //            if (c != NULL) {
    //                delete c; //(*itr);
    //                c = NULL;
    //            }
    //            itr = cc.erase(itr);
    //            ++itr;
    //        }
    //    }
    //
    //    cc = mLeftCell;
    //    if (!cc.empty()) {
    //        printf("cc size:%d\n", cc.size());
    //        itr = cc.begin();
    //        //        while ((itr != cc.end()) || (*itr != NULL)) {
    //        int size = cc.size();
    //        int counter = 0;
    //        while (++counter <= size) {
    //            printf("counter:%d\n", counter);
    //            Cell *& c = *itr;
    //            if (c != NULL) {
    //                delete c; //(*itr);
    //                c = NULL;
    //            }
    //            itr = cc.erase(itr);
    //            ++itr;
    //        }
    //    }
    //
    //    cc = mRightCell;
    //    if (!cc.empty()) {
    //        printf("cc size:%d\n", cc.size());
    //        itr = cc.begin();
    //        //        while ((itr != cc.end()) || (*itr != NULL)) {
    //        int size = cc.size();
    //        int counter = 0;
    //        while (++counter <= size) {
    //            printf("counter:%d\n", counter);
    //            Cell *& c = *itr;
    //            if (c != NULL) {
    //                delete c; //(*itr);
    //                c = NULL;
    //            }
    //            itr = cc.erase(itr);
    //            ++itr;
    //        }
    //    }

    delete[] mVectorsMem;
    mStateVector = NULL;
    mPrevStateVector = NULL;
    mChemicalVector = NULL;
    mPrevChemicalVector = NULL;
    mVectorsMem = NULL;
}

void Cell::init(double iValue) {
    //    arrayd::zero(mStateVector, mNbStates);
    //    arrayd::zero(mPrevStateVector, mNbStates);
    //    arrayd::zero(mChemicalVector, mNbChemicals);
    //    arrayd::zero(mPrevChemicalVector, mNbChemicals);

    arrayd::fill(mStateVector, mNbStates, iValue);
    arrayd::fill(mPrevStateVector, mNbStates, iValue);
    arrayd::fill(mChemicalVector, mNbChemicals, iValue);
    arrayd::fill(mPrevChemicalVector, mNbChemicals, iValue);
}

void
Cell::scramble(Randomizer& inRandomizer, double inStdDev) {
    GaussianDistribution lDistrib;

    double* lOffset = mChemicalVector;
    for (size_t i = mNbChemicals; i != 0; --i, ++lOffset)
        (*lOffset) += lDistrib(inRandomizer, inStdDev);


    // States scramble
    {
        double* lOffset = mStateVector;
        for (size_t i = mNbStates; i != 0; --i, ++lOffset)
            (*lOffset) += lDistrib(inRandomizer, inStdDev);
    }
}

bool Cell::wantsGrow() {
    return mBGrow;
}

bool Cell::wantsDivide() {
    return mBDivide;
}

void Cell::setGrowing(bool iBGrow) {
    mBGrow = iBGrow;
}

void Cell::setDividing(bool iBDiv) {
    mBDivide = iBDiv;
}

bool Cell::blurFirstPass() {

    /*
     * Gaussian blur order 2, two passes algorithm
     */

    // First pass (horizontal)
    double * lSrc = mChemicalVector;
    double * lDst = mPrevChemicalVector;
    bool bIsNotNull = true;
    int lNbNeighbors = 2;
    //   double ** neighborsChemicals = (double **) calloc((unsigned) (lNbNeighbors), sizeof(double*));
    //   for(int i = 0; i < lNbNeighbors; i++)
    //     neighborsChemicals[i]=(double *) calloc((unsigned) (mNbChemicals),sizeof(double));

    double ** neighborsChemicals = new double *[lNbNeighbors];
    for (int i = 0; i < lNbNeighbors; ++i)
        neighborsChemicals[i] = new double[mNbChemicals];

    bool ok = true;

    if ((mLeftBorder) || (mLeftCell.size() == 0) || (mLeftCell.begin() == mLeftCell.end())) { //left border
        if ((mRightBorder) || (mRightCell.size() == 0) || (mRightCell.begin() == mRightCell.end())) {
            //            std::cout << "no neighbors left and right" << std::endl;
            //            exceptionNeighbors(); //only when the sides lengths are not mod. 4
            for (size_t k = 0; k < mNbChemicals; ++k)
                lDst[k] = (2.0 * lSrc[k]) / 4.0;
            ok = false;
            goto free;
        }
        CellIterator itrBegin = mRightCell.begin();
        CellIterator itrEnd = mRightCell.end();
        getChemicalList(itrBegin, itrEnd, neighborsChemicals[0], true, bIsNotNull);
        //     getChemicalList(mRightCell, neighborsChemicals[0], true);

        //     double* lRight  = lSrc + mNbChemicals;
        //     double* lCenter = lSrc;
        for (size_t k = 0; k < mNbChemicals; ++k)
            lDst[k] = (2.0 * lSrc[k] + neighborsChemicals[0][k]) / 4.0;

    } else if ((mRightBorder) || (mRightCell.size() == 0) || (mRightCell.begin() == mRightCell.end())) { //right border
        if ((mLeftBorder) || (mLeftCell.size() == 0) || (mLeftCell.begin() == mLeftCell.end())) {
            //            std::cout << "no neighbors left and right" << std::endl;
            //            exceptionNeighbors(); //only when the sides lengths are not mod. 4
            for (size_t k = 0; k < mNbChemicals; ++k)
                lDst[k] = (2.0 * lSrc[k]) / 4.0;
            ok = false;
            goto free;
        }
        CellIterator itrBegin = mLeftCell.begin();
        CellIterator itrEnd = mLeftCell.end();
        getChemicalList(itrBegin, itrEnd, neighborsChemicals[1], true, bIsNotNull);
        //     getChemicalList(mLeftCell, neighborsChemicals[1], true);

        //     double* lCenter = lSrc;
        //     double* lLeft   = lSrc - mNbChemicals;
        for (size_t k = 0; k < mNbChemicals; ++k)
            lDst[k] = (neighborsChemicals[1][k] + 2.0 * lSrc[k]) / 4.0;
    } else {//middle

        CellIterator itrBegin = mRightCell.begin();
        CellIterator itrEnd = mRightCell.end();
        getChemicalList(itrBegin, itrEnd, neighborsChemicals[0], true, bIsNotNull);
        //     getChemicalList(mRightCell, neighborsChemicals[0], true);

        itrBegin = mLeftCell.begin();
        itrEnd = mLeftCell.end();
        getChemicalList(itrBegin, itrEnd, neighborsChemicals[1], true, bIsNotNull);
        //     getChemicalList(mLeftCell, neighborsChemicals[1], true);

        //       double* lLeft   = lSrc - mNbChemicals;
        //       double* lCenter = lSrc;
        //       double* lRight  = lSrc + mNbChemicals;
        for (size_t k = 0; k < mNbChemicals; ++k)
            lDst[k] = (neighborsChemicals[1][k] + 2.0 * lSrc[k] + neighborsChemicals[0][k]) / 4.0;
    }

free:
    for (int i = 0; i < lNbNeighbors; ++i)
        delete[] neighborsChemicals[i];

    delete[] neighborsChemicals;

    return ok;
}

bool Cell::blurSecondPass() {

    /*
     * Gaussian blur order 2, two passes algorithm
     */

    // Second pass (vertical)
    double * lSrc = mChemicalVector;
    double * lDst = mPrevChemicalVector;
    bool bIsNotNull = true;
    int lNbNeighbors = 2;
    //   double ** neighborsChemicals = (double **) calloc((unsigned) (lNbNeighbors), sizeof(double*));
    //   for(int i = 0; i < lNbNeighbors; i++)
    //     neighborsChemicals[i]=(double *) calloc((unsigned) (mNbChemicals),sizeof(double));
    double ** neighborsChemicals = new double *[lNbNeighbors];
    for (int i = 0; i < lNbNeighbors; ++i)
        neighborsChemicals[i] = new double[mNbChemicals];

    bool ok = true;

    if ((mUpperBorder) || (mUpperCell.size() == 0)) {// || (mUpperCell.begin() == mUpperCell.end())) {
        if ((mLowerBorder) || (mLowerCell.size() == 0)) {// || (mLowerCell.begin() == mLowerCell.end())) {
            //            std::cout << "no neighbors up and down" << std::endl;
            //            exceptionNeighbors(); //only when the sides lengths are not mod. 4
            for (size_t k = 0; k < mNbChemicals; ++k)
                lDst[k] = (2.0 * lSrc[k]) / 4.0;
            ok = false;
            goto free;
        }
        CellIterator itrBegin = mLowerCell.begin();
        CellIterator itrEnd = mLowerCell.end();

        getChemicalList(itrBegin, itrEnd, neighborsChemicals[0], true, bIsNotNull);
        //     getChemicalList(mLowerCell, neighborsChemicals[0], true);

        //     double* lRight  = lSrc + mNbChemicals;
        //     double* lCenter = lSrc;
        for (size_t k = 0; k < mNbChemicals; ++k)
            if (neighborsChemicals[0] != NULL)
                lDst[k] = (2.0 * lSrc[k] + neighborsChemicals[0][k]) / 4.0;

    } else if ((mLowerBorder) || (mLowerCell.size() == 0)) {// || (mLowerCell.begin() == mLowerCell.end())) {
        if ((mUpperBorder) || (mUpperCell.size() == 0)) {// || (mUpperCell.begin() == mUpperCell.end())) {
            //            std::cout << "no neighbors up and down" << std::endl;
            //            exceptionNeighbors(); //only when the sides lengths are not mod. 4
            for (size_t k = 0; k < mNbChemicals; ++k)
                lDst[k] = (2.0 * lSrc[k]) / 4.0;
            ok = false;
            goto free;
        }
        CellIterator itrBegin = mUpperCell.begin();
        CellIterator itrEnd = mUpperCell.end();
        getChemicalList(itrBegin, itrEnd, neighborsChemicals[1], true, bIsNotNull);
        //     getChemicalList(mUpperCell, neighborsChemicals[1], true);

        //     double* lCenter = lSrc;
        //     double* lLeft   = lSrc - mNbChemicals;
        for (size_t k = 0; k < mNbChemicals; ++k)
            if (neighborsChemicals[1] != NULL)
                lDst[k] = (neighborsChemicals[1][k] + 2.0 * lSrc[k]) / 4.0;
    } else {

        CellIterator itrBegin = mLowerCell.begin();
        CellIterator itrEnd = mLowerCell.end();
        getChemicalList(itrBegin, itrEnd, neighborsChemicals[0], true, bIsNotNull);
        //     getChemicalList(mLowerCell, neighborsChemicals[0], true);

        itrBegin = mUpperCell.begin();
        itrEnd = mUpperCell.end();
        getChemicalList(itrBegin, itrEnd, neighborsChemicals[1], true, bIsNotNull);
        //     getChemicalList(mUpperCell, neighborsChemicals[1], true);

        //       double* lLeft   = lSrc - mNbChemicals;
        //       double* lCenter = lSrc;
        //       double* lRight  = lSrc + mNbChemicals;
        for (size_t k = 0; k < mNbChemicals; ++k)
            if (neighborsChemicals[1] != NULL)
                lDst[k] = (neighborsChemicals[1][k] + 2.0 * lSrc[k] + neighborsChemicals[0][k]) / 4.0;
    }

free:
    for (int i = 0; i < lNbNeighbors; ++i)
        delete[] neighborsChemicals[i];

    delete[] neighborsChemicals;

    return ok;
}

void Cell::getChemicalList(CellIterator &itrBegin, CellIterator &itrEnd, double * &oChemicalList, bool ibX, bool &ibIsNeighbor) {

    if (*itrBegin == *itrEnd) { //(cc.size() == 0) {
        std::cout << "size 0" << std::endl;
        ibIsNeighbor = false; //oChemicalList =  0;
    } else {
        for (size_t i = 0; i < mNbChemicals; ++i)
            oChemicalList[i] = 0.00;

        int lMyLength = sqrt(getSize());
        int lSharedLengthSum = 0;
        CellIterator itr = itrBegin; // cc.begin();
        while (itr != itrEnd) { // cc.end()) {

            if ((*itr) != NULL) {
                //                continue;
                const double * tmp = (*itr)->getPrevChemicalVector();
                if ((tmp != NULL)) {
                    size_t sharedLength = sqrt((*itr)->getSize());
                    if (ibX)
                        checkPositionX(sharedLength, (*itr)->getPositionX(), sharedLength);
                    else
                        checkPositionY(sharedLength, (*itr)->getPositionY(), sharedLength);

                    for (size_t k = 0; k < mNbChemicals; ++k) {

                        if ((!(std::isnan(tmp[k]))) && (!(std::isinf(tmp[k])))) {
                            //                            std::cout << tmp[k] << std::endl;
                            double lTmpChem = (tmp[k] * (double) ((double) sharedLength / (double) (lMyLength)));
                            //                            std::cout << lTmpChem << std::endl;
                            oChemicalList[k] += lTmpChem;

                            lSharedLengthSum += sharedLength;
                        }
                    }
                }
            }
            ++itr;
        }
        if (lSharedLengthSum != lMyLength)
            exceptionLength();
    }
    //}
}

void Cell::checkPositionX(size_t & ioSharedLength, size_t iPosx, size_t iSize) {
    if (iPosx < getPositionX())
        ioSharedLength -= (getPositionX() - iPosx);
    double myLength = sqrt(getSize());
    if ((iPosx + iSize) > (getPositionX() + myLength))
        ioSharedLength -= iPosx + iSize - getPositionX() - myLength;
}

void Cell::checkPositionY(size_t & ioSharedLength, size_t iPosY, size_t iSize) {
    if (iPosY < getPositionY())
        ioSharedLength -= (getPositionY() - iPosY);
    double myLength = sqrt(getSize());
    if ((iPosY + iSize) > (getPositionY() + myLength))
        ioSharedLength -= iPosY + iSize - getPositionY() - myLength;
}

void Cell::update(double* &outOldState, double** &outOldChemicalsList, double* &outNewState, double* &outNewChemicals, bool * &ibIsNeighbor) {//, bool (&ibIsNeighbor)[5]) { //adresa pointra na double = double* &outOldState

    // Switch states
    std::swap(mPrevStateVector, mStateVector);
    std::swap(mPrevChemicalVector, mChemicalVector);

    outOldState = mPrevStateVector;
    outNewState = mStateVector;
    //   double * oldChemicals = mPrevChemicalVector;
    outNewChemicals = mChemicalVector;

    if ((!mUpperBorder) && (mUpperCell.size() > 0)) {
        CellIterator itrBegin = mUpperCell.begin();
        CellIterator itrEnd = mUpperCell.end();
        ibIsNeighbor[0] = true;
        getChemicalList(itrBegin, itrEnd, outOldChemicalsList[0], true, ibIsNeighbor[0]);
    } else {
        //outOldChemicalsList[0] = 0;	//alokovana pamat
        ibIsNeighbor[0] = false;
    }
    if ((!mLowerBorder) && (mLowerCell.size() > 0)) {
        CellIterator itrBegin = mLowerCell.begin();
        CellIterator itrEnd = mLowerCell.end();
        ibIsNeighbor[1] = true;
        //     getChemicalList(mLowerCell, outOldChemicalsList[1], true);
        getChemicalList(itrBegin, itrEnd, outOldChemicalsList[1], true, ibIsNeighbor[1]);
    } else
        ibIsNeighbor[1] = false;
    //outOldChemicalsList[1] = 0;

    if ((!mRightBorder) && (mRightCell.size() > 0)) {
        CellIterator itrBegin = mRightCell.begin();
        CellIterator itrEnd = mRightCell.end();
        ibIsNeighbor[2] = true;
        //     getChemicalList(mRightCell, outOldChemicalsList[2], false);
        getChemicalList(itrBegin, itrEnd, outOldChemicalsList[2], false, ibIsNeighbor[2]);
    } else
        ibIsNeighbor[2] = false;
    //outOldChemicalsList[2] = 0;

    if ((!mLeftBorder) && (mLeftCell.size() > 0)) {
        CellIterator itrBegin = mLeftCell.begin();
        CellIterator itrEnd = mLeftCell.end();
        //     getChemicalList(mLeftCell, outOldChemicalsList[3], false);
        ibIsNeighbor[3] = true;
        getChemicalList(itrBegin, itrEnd, outOldChemicalsList[3], false, ibIsNeighbor[3]);
    } else
        ibIsNeighbor[3] = false;
    //outOldChemicalsList[3] = 0;

    ibIsNeighbor[4] = true;
    *outOldChemicalsList[4] = *mPrevChemicalVector; //oldChemicals;

}

void Cell::mergeMyNeighbors(Direction iWhichNeighborSide, CellContainer & iMergeCC) {
    switch (iWhichNeighborSide) {
        case 0:
        { //N
            mUpperCell.merge(iMergeCC);
            break;
        }
        case 2:
        { //E
            mRightCell.merge(iMergeCC);
            break;
        }
        case 6:
        { //W
            mLeftCell.merge(iMergeCC);
            break;
        }
        case 4:
        { //S
            mLowerCell.merge(iMergeCC);
            break;
        }
        default:
            break;
    }
}

void Cell::changeAllMyNeighbors(Direction iWhichNeighborSide, CellIterator &iNewFirstCell, CellIterator &iNewLastCell) {
    //TODO: if no neighbors ?
    switch (iWhichNeighborSide) {
        case N:
        { //N
            if (iNewFirstCell != iNewLastCell)
                mUpperCell.assign(iNewFirstCell, iNewLastCell);
            else
                mUpperCell.clear();
            break;
        }
        case E:
        { //E
            if (iNewFirstCell != iNewLastCell)
                mRightCell.assign(iNewFirstCell, iNewLastCell);
            else
                mRightCell.clear();
            break;
        }
        case W:
        { //W
            if (iNewFirstCell != iNewLastCell)
                mLeftCell.assign(iNewFirstCell, iNewLastCell);
            else
                mLeftCell.clear();
            break;
        }
        case S:
        { //S
            if (iNewFirstCell != iNewLastCell)
                mLowerCell.assign(iNewFirstCell, iNewLastCell);
            else
                mLowerCell.clear();
            break;
        }
        default:
            break;
    }
}

void Cell::clearAllMyNeighbors(Direction iWhichNeighborSide) {
    //TODO: if no neighbors ?
    switch (iWhichNeighborSide) {
        case 0:
        { //N
            mUpperCell.clear();
            break;
        }
        case 2:
        { //E
            mRightCell.clear();
            break;
        }
        case 6:
        { //W
            mLeftCell.clear();
            break;
        }
        case 4:
        { //S
            mLowerCell.clear();
            break;
        }
        default:
            break;
    }
}

void Cell::changeAllMyNeighbors(Direction iWhichNeighborSide, CellIterator &iNewFirstCell) {
    switch (iWhichNeighborSide) {
        case 0:
        { //N
            mUpperCell.begin() = iNewFirstCell;
            break;
        }
        case 2:
        { //E
            mRightCell.begin() = iNewFirstCell;
            break;
            ;
        }
        case 6:
        { //W
            mLeftCell.begin() = iNewFirstCell;
            break;
            ;
        }
        case 4:
        { //S
            mLowerCell.begin() = iNewFirstCell;
            break;
            ;
        }
        default:
            break;
    }
}
//(iDirOp2, iDir2, lNeighborCell1, iNewCell, iNewCell);W E 10 14 14

//void Cell::changeNeighborhood(Direction iFromWhichNeighborSide, Direction iToWhichNeighborSide, int iCellToChange, int iCellToRemove, Cell *& iNewCell) {

void Cell::changeNeighborhood(Direction iFromWhichNeighborSide, Direction iToWhichNeighborSide, Cell *& iCellToChange, Cell *& iCellToRemove, Cell *& iNewCell) {
    CellIterator ocb;
    CellIterator oce;

    if (getNeighbors(iFromWhichNeighborSide, ocb, oce))
        while (ocb != oce) {
            (*ocb)->changeNeighbor(iToWhichNeighborSide, iCellToChange, iCellToRemove, iNewCell); //change neighbors to me
            ++ocb;
        }
}

void Cell::changeNeighborhood(Direction iFromWhichNeighborSide, Direction iToWhichNeighborSide, Cell *& iCellToChange, Cell *& iNewCell) {
    CellIterator ocb;
    CellIterator oce;

    if (getNeighbors(iFromWhichNeighborSide, ocb, oce))
        while (ocb != oce) {
            (*ocb)->changeNeighbor(iToWhichNeighborSide, iCellToChange, iNewCell); //change neighbors to me
            ++ocb;
        }
}

//void Cell::changeNeighbor(Direction iWhichNeighborSide, int iCellToChange, int iCellToRemove, Cell *& iNewCell) {

bool orderer(Cell * iCell1, Cell * iCell2) {
    return (iCell1->getIndex() < iCell2->getIndex());
}

bool duplicates(Cell * iCell1, Cell * iCell2) {
    return (iCell1->getIndex() == iCell2->getIndex());
}

void Cell::changeNeighbor(Direction iWhichNeighborSide, Cell *& iCellToChange, Cell *& iCellToRemove, Cell *& iNewCell) {
    CellContainer * cc = NULL;
    switch (iWhichNeighborSide) {
        case 0:
        { //N
            cc = &mUpperCell;
            break;
        }
        case 2:
        { //E
            cc = &mRightCell;
            break;
        }
        case 6:
        { //W
            cc = &mLeftCell;
            break;
        }
        case 4:
        { //S
            cc = &mLowerCell;
            break;
        }
        default:
            return;
    }

    CellIterator ci = ((*cc).begin());
    //    printf("i: %d \n", (*ci)->getIndex());

    //    while (ci != (*cc).end()) {
    //        if ((*ci) != NULL) {
    //            if ((*ci)->getIndex() == iCellToChange->getIndex()) {
    //                *ci = iNewCell;
    //            }
    //        }
    //        ++ci;
    //    }

    //    if (iNewCell != iCellToRemove) {
    while (ci != (*cc).end()) {
        if ((*ci) != NULL) {
            if ((*ci)->getIndex() == iCellToChange->getIndex()) {
                *ci = iNewCell;
                ++ci;
            } else if ((*ci)->getIndex() == iCellToRemove->getIndex()) { //delete
                ci = (*cc).erase(ci);
            } else
                ++ci;
        } else
            ++(ci);
    }
    (*cc).sort(orderer);
    (*cc).unique(duplicates);
    //    } else
    //        while (ci != (*cc).end()) {
    //            if ((*ci) != NULL) {
    //                if ((*ci)->getIndex() == iCellToChange->getIndex()) {
    //                    ci = (*cc).erase(ci);
    //                } else
    //                    ++ci;
    //            } else
    //                ++(ci);
    //        }
}

void Cell::changeNeighbor(Direction iWhichNeighborSide, Cell *& iCellToChange, Cell *& iNewCell) {
    CellContainer * cc = NULL;
    switch (iWhichNeighborSide) {
        case 0:
        { //N
            cc = &mUpperCell;
            break;
        }
        case 2:
        { //E
            cc = &mRightCell;
            break;
        }
        case 6:
        { //W
            cc = &mLeftCell;
            break;
        }
        case 4:
        { //S
            cc = &mLowerCell;
            break;
        }
        default:
            return;
    }

    CellIterator ci = ((*cc).begin());
    while (ci != (*cc).end()) {
        if ((*ci) != NULL) {
            if ((*ci)->getIndex() == iCellToChange->getIndex()) {
                *ci = iNewCell;
            }
        }
        ++ci;
    }
    (*cc).sort(orderer);
    (*cc).unique(duplicates);
}

void Cell::removeNeighbor(Direction iWhichNeighborSide, Cell *& iCellToRemove) {
    CellContainer * cc = NULL;
    switch (iWhichNeighborSide) {
        case N:
        { //N
            cc = &mUpperCell;
            break;
        }
        case E:
        { //E
            cc = &mRightCell;
            break;
        }
        case W:
        { //W
            cc = &mLeftCell;
            break;
        }
        case S:
        { //S
            cc = &mLowerCell;
            break;
        }
        default:
            return;
    }

    CellIterator ci = ((*cc).begin());

    while (ci != (*cc).end()) {
        if ((*ci) != NULL) {
            if ((*ci) == iCellToRemove) { //delete
                ci = (*cc).erase(ci);
            } else
                ++ci;
        } else
            ++(ci);
    }
}

bool Cell::getGrowingDirections(Direction & oDir1, Direction & oDir2, Cell *& oCell1, Cell *& oCell2, Cell *& oCell3) {
    CellContainer cc;
    bool bGrowUp = !mUpperBorder;
    if (bGrowUp) {
        cc = mUpperCell;
        if ((cc.size() != 1) || (!cc.front()->wantsGrow()))
            bGrowUp = false;
    }
    bool bGrowRight = !mRightBorder;
    if (bGrowRight) {
        cc = mRightCell;
        if ((cc.size() != 1) || (!cc.front()->wantsGrow()))
            bGrowRight = false;
    }
    bool bGrowLeft = !mLeftBorder;
    if (bGrowLeft) {
        cc = mLeftCell;
        if ((cc.size() != 1) || (!cc.front()->wantsGrow()))
            bGrowLeft = false;
    }
    bool bGrowDown = !mLowerBorder;
    if (bGrowDown) {
        cc = mLowerCell;
        if ((cc.size() != 1) || (!cc.front()->wantsGrow()))
            bGrowDown = false;
    }
    oCell1 = NULL;
    oCell2 = NULL;
    oCell3 = NULL;
    if (bGrowUp) {
        if ((!mUpperBorder) && (mUpperCell.size() == 1)) {
            Cell *& lUpper = mUpperCell.front();
            if ((lUpper == NULL) || (lUpper->getSize() != mSize) || (lUpper->toDelete()))
                return false;
            if ((bGrowRight) && (!mRightBorder) && (mRightCell.size() == 1)) {
                Cell *& lRight = mRightCell.front();
                if ((lRight == NULL) || (lRight->getSize() != mSize) || (lRight->toDelete()))
                    return false;
                if (!(lUpper->getContainer(E, cc)))
                    return false;
                cc = lUpper->mRightCell;
                if ((cc.size() == 1) && (cc.front()->wantsGrow()) && (cc == lRight->mUpperCell)) {
                    oDir1 = N;
                    oDir2 = E;
                    //                        if ((mUpperCell.size() > 1) || (mRightCell.size() > 1) || (cc.size() > 1))
                    //                            return false;
                    oCell1 = lUpper;
                    oCell2 = lRight;
                    oCell3 = cc.front();
                    //                    return true;
                } else
                    return false;
            } else if ((bGrowLeft) && (!mLeftBorder) && (mLeftCell.size() == 1)) {
                Cell *& lLeft = mLeftCell.front();
                if ((lLeft == NULL) || (lLeft->getSize() != mSize) || (lLeft->toDelete()))
                    return false;
                if (!(lUpper->getContainer(W, cc)))
                    return false;
                cc = lUpper->mLeftCell;
                if ((cc.size() == 1) && (cc.front()->wantsGrow()) && (cc == lLeft->mUpperCell)) {
                    oDir1 = N;
                    oDir2 = W;
                    //                        if ((mUpperCell.size() > 1) || (mLeftCell.size() > 1) || (cc.size() > 1))
                    //                            return false;
                    oCell1 = lUpper;
                    oCell2 = lLeft;
                    oCell3 = cc.front();
                    //                    return true;
                } else
                    return false;
            } else
                return false;
        } else
            return false;
    } else if (bGrowDown) {
        if ((!mLowerBorder) && (mLowerCell.size() == 1)) {
            Cell *& lLower = mLowerCell.front();
            if ((lLower == NULL) || (lLower->getSize() != mSize) || (lLower->toDelete()))
                return false;
            if ((bGrowRight) && (!mRightBorder) && (mRightCell.size() == 1)) {
                Cell *& lRight = mRightCell.front();
                if ((lRight == NULL) || (lRight->getSize() != mSize) || (lRight->toDelete()))
                    return false;
                if (!(lLower->getContainer(E, cc)))
                    return false;
                cc = lLower->mRightCell;
                if ((cc.size() == 1) && (cc.front()->wantsGrow()) && (cc == lRight->mLowerCell)) {
                    oDir1 = S;
                    oDir2 = E;
                    //                        if ((mLowerCell.size() > 1) || (mRightCell.size() > 1) || (cc.size() > 1))
                    //                            return false;
                    oCell1 = lLower;
                    oCell2 = lRight;
                    oCell3 = cc.front();
                } else
                    return false;
            } else if ((bGrowLeft) && (!mLeftBorder) && (mLeftCell.size() == 1)) {
                Cell *& lLeft = mLeftCell.front();
                if ((lLeft == NULL) || (lLeft->getSize() != mSize) || (lLeft->toDelete()))
                    return false;
                if (!(lLower->getContainer(W, cc)))
                    return false;
                cc = lLower->mLeftCell;
                if ((cc.size() == 1) && (cc.front()->wantsGrow()) && (cc == lLeft->mLowerCell)) {
                    oDir1 = S;
                    oDir2 = W;
                    //                        if ((mLowerCell.size() > 1) || (mLeftCell.size() > 1) || (cc.size() > 1))
                    //                            return false;
                    oCell1 = lLower;
                    oCell2 = lLeft;
                    oCell3 = cc.front();
                } else
                    return false;
            } else
                return false;
        } else
            return false;
    } else
        return false;

    if ((oCell1 == NULL) || (oCell2 == NULL) || (oCell3 == NULL))
        return false;

    if ((oCell3->getSize() != mSize) || (oCell3->toDelete()))
        return false;

    if ((oCell1->getSize() != mSize) || (oCell1->toDelete()))
        return false;

    if ((oCell2->getSize() != mSize) || (oCell2->toDelete()))
        return false;

    return true;
}

bool Cell::getContainer(Direction iDir, CellContainer & oContainer) {
    bool result = true;
    if (isOnTheBorder(iDir))
        return false;
    switch (iDir) {
        case N:
        {
            if ((mUpperBorder) || (mUpperCell.empty()))
                result = false;
            //            printf("get from north:%d",iDir);
            oContainer = mUpperCell;
            break;
        }
        case S:
        {
            if ((mLowerBorder) || (mLowerCell.empty()))
                result = false;
            //            printf("get from south:%d",iDir);
            oContainer = mLowerCell;
            break;
        }
        case E:
        {
            if ((mRightBorder) || (mRightCell.empty()))
                result = false;
            //            printf("get from east:%d",iDir);
            oContainer = mRightCell;
            break;
        }
        case W:
        {
            if ((mLeftBorder) || (mLeftCell.empty()))
                result = false;
            //            printf("get from west:%d",iDir);
            oContainer = mLeftCell;
            break;
        }
        default:
            return false;
    }
    return result;
}

bool Cell::grow(CellIterator & oTargetCellsBegin, CellIterator & oTargetCellsEnd) {
    bool bGrowUp = true;
    CellContainer cc = mUpperCell;
    if ((cc.size() != 1) || (!cc.front()->wantsGrow()))
        bGrowUp = false;

    bool bGrowRight = true;
    cc = mRightCell;
    if ((cc.size() != 1) || (!cc.front()->wantsGrow()))
        bGrowRight = false;

    bool bGrowLeft = true;
    cc = mLeftCell;
    if ((cc.size() != 1) || (!cc.front()->wantsGrow()))
        bGrowLeft = false;

    bool bGrowDown = true;
    cc = mLowerCell;
    if ((cc.size() != 1) || (!cc.front()->wantsGrow()))
        bGrowDown = false;

    if (bGrowUp) {
        if (bGrowRight) {
            //bool bGrowRight2 = true;
            cc = mUpperCell.front()->mRightCell;
            if ((cc.size() == 1) || (cc.front()->wantsGrow())) {
                //bGrowRight2 = false;
                //bGrowUp = true;
                if (cc == mRightCell.front()->mUpperCell) {
                    // 	if ((cc.size() != 1) || (!cc[0].wantsGrow())
                    // 	  bGrowUp = false;
                    //
                    // 	if (bGrowUp) {
                    CellContainer oTargetCells;
                    oTargetCells.push_back(mUpperCell.front());
                    oTargetCells.push_back(mRightCell.front());
                    oTargetCells.push_back(cc.front());
                    oTargetCellsBegin = oTargetCells.begin();
                    oTargetCellsEnd = oTargetCells.end();
                    //grow right up

                    mPositionY -= mSize;
                    mSize *= 2;

                    //                    //change neighbors of merged cells
                    //                    (mUpperCell.front())->changeNeighborhood(N, S, mUpperCell.front(), cc.front(), this);
                    //                    (mUpperCell.front())->changeNeighborhood(W, E, mUpperCell.front(), this, this);
                    //
                    //                    (mRightCell.front())->changeNeighborhood(E, W, mRightCell.front(), cc.front(), this);
                    //                    (mRightCell.front())->changeNeighborhood(S, N, mRightCell.front(), this, this);
                    //
                    //                    (cc.front())->changeNeighborhood(E, W, cc.front(), mRightCell.front(), this);
                    //                    (cc.front())->changeNeighborhood(N, S, cc.front(), mUpperCell.front(), this);

                    //change my own neighborhood
                    CellIterator ocb;
                    CellIterator oce;

                    (mUpperCell.front())->getNeighbors(N, ocb, oce);
                    changeAllMyNeighbors(N, ocb, oce);
                    mUpperCell.merge((cc.front())->mUpperCell);

                    mLeftCell.merge((mUpperCell.front())->mLeftCell);

                    (mRightCell.front())->getNeighbors(E, ocb, oce);
                    changeAllMyNeighbors(E, ocb, oce);
                    mRightCell.merge((cc.front())->mRightCell);

                    mLowerCell.merge((mRightCell.front())->mLowerCell);

                    return true;
                    //exit (0);
                }
            }
        }

        if (bGrowLeft) {
            //bGrowLeft = true;
            cc = mUpperCell.front()->mLeftCell;
            if ((cc.size() == 1) || (cc.front()->wantsGrow())) {
                if (cc == mLeftCell.front()->mUpperCell) {
                    CellContainer oTargetCells;
                    oTargetCells.push_back(mUpperCell.front());
                    oTargetCells.push_back(mLeftCell.front());
                    oTargetCells.push_back(cc.front());
                    oTargetCellsBegin = oTargetCells.begin();
                    oTargetCellsEnd = oTargetCells.end();
                    //grow left up
                    mPositionY -= mSize;
                    mPositionX -= mSize;
                    mSize *= 2;

                    //                    //change neighbors of merged cells
                    //                    //changeNeighborhood(Direction iFromWhichNeighborSide, Direction iToWhichNeighborSide, Cell * &iCellToChange, Cell * &iCellToRemove, Cell * const iNewCell
                    //
                    //                    (mUpperCell.front())->changeNeighborhood(N, S, mUpperCell.front(), cc.front(), this);
                    //                    (mUpperCell.front())->changeNeighborhood(E, W, mUpperCell.front(), this, this);
                    //
                    //                    (mLeftCell.front())->changeNeighborhood(W, E, mLeftCell.front(), cc.front(), this);
                    //                    (mLeftCell.front())->changeNeighborhood(S, N, mLeftCell.front(), this, this);
                    //
                    //                    (cc.front())->changeNeighborhood(W, E, cc.front(), mLeftCell.front(), this);
                    //                    (cc.front())->changeNeighborhood(N, S, cc.front(), mUpperCell.front(), this);

                    //change my own neighborhood
                    CellIterator ocb;
                    CellIterator oce;

                    (mUpperCell.front())->getNeighbors(N, ocb, oce);
                    changeAllMyNeighbors(N, ocb, oce);
                    mUpperCell.merge((cc.front())->mUpperCell);

                    mRightCell.merge((mUpperCell.front())->mRightCell);

                    (mLeftCell.front())->getNeighbors(W, ocb, oce);
                    changeAllMyNeighbors(W, ocb, oce);
                    mLeftCell.merge((cc.front())->mLeftCell);

                    mLowerCell.merge((mLeftCell.front())->mLowerCell);

                    return true;
                }
            }
        }
    }

    if (bGrowDown) {
        if (bGrowRight) {
            //bGrowRight = true;
            cc = mLowerCell.front()->mRightCell;
            if ((cc.size() == 1) || (cc.front()->wantsGrow())) {
                if (cc == mRightCell.front()->mLowerCell) {
                    CellContainer oTargetCells;
                    oTargetCells.push_back(mLowerCell.front());
                    oTargetCells.push_back(mRightCell.front());
                    oTargetCells.push_back(cc.front());
                    oTargetCellsBegin = oTargetCells.begin();
                    oTargetCellsEnd = oTargetCells.end();

                    //grow right down
                    mSize *= 2;

                    //                    //change neighbors of merged cells
                    //                    //changeNeighborhood(Direction iFromWhichNeighborSide, Direction iToWhichNeighborSide, Cell * &iCellToChange, Cell * &iCellToRemove, Cell * const iNewCell
                    //
                    //                    (mLowerCell.front())->changeNeighborhood(S, N, mLowerCell.front(), cc.front(), this);
                    //                    (mLowerCell.front())->changeNeighborhood(W, E, mLowerCell.front(), this, this);
                    //
                    //                    (mRightCell.front())->changeNeighborhood(E, W, mRightCell.front(), cc.front(), this);
                    //                    (mRightCell.front())->changeNeighborhood(N, S, mRightCell.front(), this, this);
                    //
                    //                    (cc.front())->changeNeighborhood(E, W, cc.front(), mRightCell.front(), this);
                    //                    (cc.front())->changeNeighborhood(S, N, cc.front(), mLowerCell.front(), this);

                    //change my own neighborhood
                    CellIterator ocb;
                    CellIterator oce;

                    (mLowerCell.front())->getNeighbors(S, ocb, oce);
                    changeAllMyNeighbors(S, ocb, oce);
                    mLowerCell.merge((cc.front())->mLowerCell);

                    mLeftCell.merge((mLowerCell.front())->mLeftCell);

                    (mRightCell.front())->getNeighbors(E, ocb, oce);
                    changeAllMyNeighbors(E, ocb, oce);
                    mRightCell.merge((cc.front())->mRightCell);

                    mUpperCell.merge((mRightCell.front())->mUpperCell);

                    return true;
                }
            }
        }

        if (bGrowLeft) {
            //bGrowLeft = true;
            cc = mLowerCell.front()->mLeftCell;
            if ((cc.size() == 1) || (cc.front()->wantsGrow())) {
                if (cc == mLeftCell.front()->mLowerCell) {
                    CellContainer oTargetCells;
                    oTargetCells.push_back(mLowerCell.front());
                    oTargetCells.push_back(mLeftCell.front());
                    oTargetCells.push_back(cc.front());
                    oTargetCellsBegin = oTargetCells.begin();
                    oTargetCellsEnd = oTargetCells.end();

                    //grow left down
                    mPositionX -= mSize;
                    mSize *= 2;


                    //                    //change neighbors of merged cells
                    //                    //changeNeighborhood(Direction iFromWhichNeighborSide, Direction iToWhichNeighborSide, Cell * &iCellToChange, Cell * &iCellToRemove, Cell * const iNewCell
                    //
                    //                    (mLowerCell.front())->changeNeighborhood(S, N, mLowerCell.front(), cc.front(), this);
                    //                    (mLowerCell.front())->changeNeighborhood(E, W, mLowerCell.front(), this, this);
                    //
                    //                    (mLeftCell.front())->changeNeighborhood(W, E, mLeftCell.front(), cc.front(), this);
                    //                    (mLeftCell.front())->changeNeighborhood(N, S, mLeftCell.front(), this, this);
                    //
                    //                    (cc.front())->changeNeighborhood(W, E, cc.front(), mLeftCell.front(), this);
                    //                    (cc.front())->changeNeighborhood(S, N, cc.front(), mLowerCell.front(), this);

                    //change my own neighborhood
                    CellIterator ocb;
                    CellIterator oce;

                    (mLowerCell.front())->getNeighbors(S, ocb, oce);
                    changeAllMyNeighbors(S, ocb, oce);
                    mLowerCell.merge((cc.front())->mLowerCell);

                    mRightCell.merge((mLowerCell.front())->mRightCell);

                    (mLeftCell.front())->getNeighbors(W, ocb, oce);
                    changeAllMyNeighbors(W, ocb, oce);
                    mLeftCell.merge((cc.front())->mLeftCell);

                    mUpperCell.merge((mLeftCell.front())->mUpperCell);

                    return true;
                }
            }
        }
    }
    return false;
}

void Cell::addNeighbor(Cell * &iCell, int iDirection) {
    //    printf("me: %d | d: %d | i: %d \n", mIndex, iDirection, iCell->getIndex());
    switch (iDirection) {
        case N:
        { //N
            mUpperCell.push_back(iCell);
            break;
        }
        case E:
        { //E
            mRightCell.push_back(iCell);
            break;
        }
        case W:
        { //W
            mLeftCell.push_back(iCell);
            break;
        }
        case S:
        { //S
            mLowerCell.push_back(iCell);
            break;
        }
        default:
            break;
    }
}

void Cell::addNeighbor(Cell * &iCell, Direction iDirection) {
    switch (iDirection) {
        case N:
        { //N
            mUpperCell.push_back(iCell);
            break;
        }
        case E:
        { //E
            mRightCell.push_back(iCell);
            break;
        }
        case W:
        { //W
            mLeftCell.push_back(iCell);
            break;
        }
        case S:
        { //S
            mLowerCell.push_back(iCell);
            break;
        }
        default:
            break;
    }
}

bool Cell::isOnTheBorder(Direction iDir) {
    switch (iDir) {
        case N:
        {
            return mUpperBorder;
            break;
        }
        case S:
        {
            return mLowerBorder;
            break;
        }
        case E:
        {
            return mRightBorder;
            break;
        }
        case W:
        {
            return mLeftBorder;
            break;
        }
        case NE:
        {
            if (!mRightBorder)
                return mUpperBorder;
            else
                return mRightBorder;
            break;
        }
        case SE:
        {
            if (!mRightBorder)
                return mLowerBorder;
            else
                return mRightBorder;
            break;
        }
        case NW:
        {
            if (!mLeftBorder)
                return mUpperBorder;
            else
                return mLeftBorder;
            break;
        }
        case SW:
        {
            if (!mLeftBorder)
                return mLowerBorder;
            else
                return mLeftBorder;
            break;
        }
        default:
        {
            return false;
            break;
        }
    }
}

void Cell::setAsBorder(Direction iDir) {
    switch (iDir) {
        case N:
        {
            mUpperBorder = true;
            break;
        }
        case S:
        {
            mLowerBorder = true;
            break;
        }
        case E:
        {
            mRightBorder = true;
            break;
        }
        case W:
        {
            mLeftBorder = true;
            break;
        }
        case NE:
        {
            break;
        }
        case SE:
        {
            break;
        }
        case NW:
        {
            break;
        }
        case SW:
        {
            break;
        }
        default:
            break;
    }
}

bool Cell::getNeighbors(Direction iDirection, CellIterator & outCellBegin, CellIterator & outCellEnd) {
    bool result = true;
    switch (iDirection) {
        case N:
        { //N
            if ((mUpperBorder) || (mUpperCell.empty()))
                result = false;
            outCellEnd = mUpperCell.end();
            outCellBegin = mUpperCell.begin();
            break;
        }
        case E:
        { //E
            if ((mRightBorder) || (mRightCell.empty()))
                result = false;
            outCellEnd = mRightCell.end();
            outCellBegin = mRightCell.begin();
            break;
        }
        case NE:
        { //NE
            if ((mUpperBorder) || (mRightBorder) || (mUpperCell.empty()) || (mRightCell.empty()))
                result = false;
            Cell * lTmpCell = mUpperCell.front();
            if (lTmpCell == NULL)
                return false;
            outCellEnd = lTmpCell->mRightCell.end();
            outCellBegin = lTmpCell->mRightCell.begin();
            break;
        }
        case NW:
        {
            if ((mUpperBorder) || (mLeftBorder) || (mUpperCell.empty()) || (mLeftCell.empty()))
                result = false;
            Cell * lTmpCell = mUpperCell.front();
            if (lTmpCell == NULL)
                return false;
            outCellEnd = lTmpCell->mLeftCell.end();
            outCellBegin = lTmpCell->mLeftCell.begin();
            break;
        }
        case W:
        {
            if ((mLeftBorder) || (mLeftCell.empty()))
                result = false;
            outCellEnd = mLeftCell.end();
            outCellBegin = mLeftCell.begin();
            break;
        }
        case S:
        {
            if ((mLowerBorder) || (mLowerCell.empty()))
                result = false;
            outCellEnd = mLowerCell.end();
            outCellBegin = mLowerCell.begin();
            break;
        }
        case SE:
        {
            if ((mLowerBorder) || (mRightBorder) || (mLowerCell.empty()) || (mRightCell.empty()))
                result = false;
            Cell * lTmpCell = mLowerCell.front();
            if (lTmpCell == NULL)
                result = false;
            outCellEnd = lTmpCell->mRightCell.end();
            outCellBegin = lTmpCell->mRightCell.begin();
            break;
        }
        case SW:
        {
            if ((mLowerBorder) || (mLeftBorder) || (mLowerCell.empty()) || (mLeftCell.empty()))
                result = false;
            Cell * lTmpCell = mLowerCell.front();
            if (lTmpCell == NULL)
                result = false;
            outCellEnd = lTmpCell->mLeftCell.end();
            outCellBegin = lTmpCell->mLeftCell.begin();
            break;
        }
        default:
        {
            return false;
        }
    }
    return result;
}

bool Cell::getSingleNeighbor(Direction iDirection, Cell * & outCell) {
    //TODO: cross directions
    bool result = true;
    switch (iDirection) {
        case N:
        { //N
            if ((mUpperBorder) || (mUpperCell.empty()) || (mUpperCell.size() != 1))
                result = false;
            outCell = mUpperCell.front();
            break;
        }
        case E:
        { //E
            if ((mRightBorder) || (mRightCell.empty()) || (mRightCell.size() != 1))
                result = false;
            outCell = mRightCell.front();
            break;
        }
        case NE:
        { //NE
            if ((mUpperBorder) || (mRightBorder) || (mUpperCell.empty()) || (mRightCell.empty()))
                result = false;
            Cell * lTmpCell = mUpperCell.front();
            if (lTmpCell == NULL)
                result = false;
            outCell = lTmpCell->mRightCell.front();
            break;
        }
        case NW:
        { //NW
            if ((mUpperBorder) || (mLeftBorder) || (mUpperCell.empty()) || (mLeftCell.empty()))
                result = false;
            Cell * lTmpCell = mUpperCell.front();
            if (lTmpCell == NULL)
                result = false;
            outCell = lTmpCell->mLeftCell.front();
            break;
        }
        case W:
        { //W
            if ((mLeftBorder) || (mLeftCell.empty()) || (mLeftCell.size() != 1))
                result = false;
            outCell = mLeftCell.front();
            break;
        }
        case S:
        { //S
            if ((mLowerBorder) || (mLowerCell.empty()) || (mLowerCell.size() != 1))
                result = false;
            outCell = mLowerCell.front();
            break;
        }
        case SE:
        {
            if ((mLowerBorder) || (mRightBorder) || (mLowerCell.empty()) || (mRightCell.empty()))
                result = false;
            Cell * lTmpCell = mLowerCell.front();
            if (lTmpCell == NULL)
                result = false;
            outCell = lTmpCell->mRightCell.front();
            break;
        }
        case SW:
        {
            if ((mLowerBorder) || (mLeftBorder) || (mLowerCell.empty()) || (mLeftCell.empty()))
                result = false;
            Cell * lTmpCell = mLowerCell.front();
            if (lTmpCell == NULL)
                result = false;
            outCell = lTmpCell->mLeftCell.front();
            break;
        }
        default:
        {
            result = false;
            break;
        }
    }
    return result;
}

bool Cell::getFirstNeighbor(Direction iDirection, Cell * & outCell) {
    bool result = true;
    switch (iDirection) {
        case N:
        { //N
            if ((mUpperBorder) || (mUpperCell.empty()))
                result = false;
            outCell = mUpperCell.front();
            break;
        }
        case E:
        { //E
            if ((mRightBorder) || (mRightCell.empty()))
                result = false;
            outCell = mRightCell.front();
            break;
        }
        case NE:
        { //NE
            if ((mUpperBorder) || (mRightBorder) || (mUpperCell.empty()) || (mRightCell.empty()))
                result = false;
            Cell * lTmpCell = mUpperCell.front();
            if (lTmpCell == NULL)
                result = false;
            outCell = lTmpCell->mRightCell.front();
            break;
        }
        case NW:
        { //NW
            if ((mUpperBorder) || (mLeftBorder) || (mUpperCell.empty()) || (mLeftCell.empty()))
                result = false;
            Cell * lTmpCell = mUpperCell.front();
            if (lTmpCell == NULL)
                result = false;
            outCell = lTmpCell->mLeftCell.front();
            break;
        }
        case W:
        { //W
            if ((mLeftBorder) || (mLeftCell.empty()))
                result = false;
            outCell = mLeftCell.front();
            break;
        }
        case S:
        { //S
            if ((mLowerBorder) || (mLowerCell.empty()))
                result = false;
            outCell = mLowerCell.front();
            break;
        }
        case SE:
        {
            if ((mLowerBorder) || (mRightBorder) || (mLowerCell.empty()) || (mRightCell.empty()))
                result = false;
            Cell * lTmpCell = mLowerCell.front();
            if (lTmpCell == NULL)
                result = false;
            outCell = lTmpCell->mRightCell.front();
            break;
        }
        case SW:
        {
            if ((mLowerBorder) || (mLeftBorder) || (mLowerCell.empty()) || (mLeftCell.empty()))
                result = false;
            Cell * lTmpCell = mLowerCell.front();
            if (lTmpCell == NULL)
                result = false;
            outCell = lTmpCell->mLeftCell.front();
            break;
        }
        default:
        {
            result = false;
            break;
        }
    }
    return result;
}

void Cell::divide() {
    //       if (mSize == 1)
    //     	  return;
    //       int lNewCellsNumber = mSize - 1;
    //       vector<Cell*> lCells;
    //       for (int i = 0; i < lNewCellsNumber; i++) {
    //           lCells.push_back(new Cell(mNbStates, mNbChemicals, (i % mSize) + mPositionX, (i / mSize) + mPositionY, 1));
    //       }
    //
    //       for (int i = mSize+1; i < lNewCellsNumber-mSize; i++) {
    //         if ((i % mSize != 0) && (i % (mSize - 1) != 0)) {
    //           lCells[i].addNeighbor(lCells[i-1], W);
    //           lCells[i].addNeighbor(lCells[i+1], E);
    //           lCells[i].addNeighbor(lCells[i-mSize], N);
    //           lCells[i].addNeighbor(lCells[i+mSize], S);
    //         }
    //       }
    //
    //
    //       Cell * lNeighbors = getNeighbors(W);
    //       int i = 0;
    //       while (lNeighbors != NULL) {
    //         if ((*lNeighbors)->mPositionY <= mPositionY+i) {
    //           lCells[i*mSize].addNeighbor(*lNeighbors, W);
    //           ++lNeighbors;
    //         } else
    //           i++;
    //       }
    //
    //       Cell * lNeighbors = getNeighbors(E);
    //       int i = 0;
    //       while (lNeighbors != NULL) {
    //         if ((*lNeighbors)->mPositionY <= mPositionY+i) {
    //           lCells[i*mSize].addNeighbor(*lNeighbors, E);
    //           ++lNeighbors;
    //         } else
    //           i++;
    //       }
    //
    //       Cell * lNeighbors = getNeighbors(N);
    //       int i = 0;
    //       while (lNeighbors != NULL) {
    //         if ((*lNeighbors)->mPositionX <= mPositionX+i) {
    //           lCells[i*mSize].addNeighbor(*lNeighbors, N);
    //           ++lNeighbors;
    //         } else
    //           i++;
    //       }
    //
    //       Cell * lNeighbors = getNeighbors(S);
    //       int i = 0;
    //       while (lNeighbors != NULL) {
    //         if ((*lNeighbors)->mPositionX <= mPositionX+i) {
    //           lCells[i*mSize].addNeighbor(*lNeighbors, S);
    //           ++lNeighbors;
    //         } else
    //           i++;
    //       }

}
