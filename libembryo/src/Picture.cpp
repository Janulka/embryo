/*
 *  libembryo
 *  Copyright (C) 2010 by Jana Hlavacikova
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

#include <algorithm>
#include <math.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <map>
#include "Picture.h"
#include "ArrayOps.h"
#include "Exception.h"
#include "Randomizer.h"

using namespace std;
using namespace embryo;


Picture::Picture() : mWidth(0), mHeight(0), mName("") {
}

Picture::Picture(size_t inWidth, size_t inHeight) : mWidth(inWidth), mHeight(inHeight), mName("") {
    mArray = new double[mWidth * mHeight];
}

Picture::Picture(const Picture& inPic) : mWidth(inPic.mWidth), mHeight(inPic.mHeight) {


    size_t lNbPixels = mWidth * mHeight;
    mArray = new double[lNbPixels];
    arrayd::copy(mArray, inPic.mArray, lNbPixels);

    copy(inPic, inPic.mWidth, inPic.mHeight);
}

Picture::~Picture() {
    AspectCellIterator aciBegin;
    AspectCellIterator aciEnd;
    getAspectCells(aciBegin, aciEnd);

    while (aciBegin != aciEnd) {
        if (*aciBegin) {
            delete (*aciBegin);
            *aciBegin = NULL;
            aciBegin = mAspectCells.erase(aciBegin);
        } else
            ++aciBegin;
    }

    //if (mArray)
      //  delete[] mArray;

    //mAspectCells.clear();
}

Picture&
        Picture::operator =(const Picture& inPic) {

    copy(inPic, inPic.mWidth, inPic.mHeight);
    mWidth = inPic.mWidth;
    mHeight = inPic.mHeight;

    return *this;
}

void
Picture::setup(size_t inWidth, size_t inHeight) {
    mWidth = inWidth;
    mHeight = inHeight;
    mName = "";
    mArray = new double[mWidth * mHeight];
}

void
Picture::affineTransform(double inScale, double inBias) {
    AspectCellIterator aciBegin;
    AspectCellIterator aciEnd;
    getAspectCells(aciBegin, aciEnd);

    while (aciBegin != aciEnd) {
        double lAspect = inScale * (*aciBegin)->getAspect() + inBias;
        (*aciBegin)->setAspect(lAspect);
        ++aciBegin;
    }
}

void
Picture::copy(const Picture& inPic, size_t inX, size_t inY) {

    mHeight = inY;
    mWidth = inX;


    mAspectCells.clear();

    AspectCellConstIterator aciBegin;
    AspectCellConstIterator aciEnd;
    inPic.getAspectCells(aciBegin, aciEnd);

    while (aciBegin != aciEnd) {
        mAspectCells.push_back(*aciBegin);
        ++aciBegin;
    }
}

double Picture::colorPerPixel(const size_t inX, const size_t inY) const {
    int pos = (int) (mHeight * inX + inY);
    return mArray[pos];
}

double Picture::colour(const size_t inX, const size_t inY) const {
    AspectCellConstIterator aciBegin = mAspectCells.begin();
    AspectCellConstIterator aciEnd = mAspectCells.end();

    while (aciBegin != aciEnd) {
        if (((*aciBegin)->getPositionX() <= inX) && ((*aciBegin)->getPositionX() + (*aciBegin)->getSize() >= inX) && ((*aciBegin)->getPositionY() <= inY) && ((*aciBegin)->getPositionY() + (*aciBegin)->getSize() >= inY))
            return (*aciBegin)->getAspect();
        ++aciBegin;
    }

    return -1.0;
}

double
embryo::distanceStructure(const Picture& inA, const Picture& inB) {
    AspectCellConstIterator aciBegin;
    AspectCellConstIterator aciEnd;
    inA.getAspectCells(aciBegin, aciEnd);

    size_t lNbPixels = (inB.height() * inB.width());
    //    double lPenalty = (double)(lNbPixels) / (double) (inA.cells());
    double lPenalty = (double) (inA.cells()) / (double) (lNbPixels);
    double lSum = 0.0;
    int x = 0;
    int xOld = 0;
    int y = 0;
    int total_black = 0;
    int total_white = 0;
    double lTotalColorDiff = 0;

    while (aciBegin != aciEnd) {

        const double* lPixels = inB.pixels();

        double lAspectCandidate = (*aciBegin)->getAspect();
        x = (*aciBegin)->getPositionX();
        y = (*aciBegin)->getPositionY();
        int maxX = x + sqrt((*aciBegin)->getSize());
        int maxY = y + sqrt((*aciBegin)->getSize());

        int plus = 0;
        int minus = 0;

        int black = 0;
        int white = 0;
        double lColorDiff = 0;

        //map of color pixels [color, number]
        //round a color pixel value, insert into map. after inserting all pixels of a cell, sort map; get the max number of same color pixels
        //        std::map<float, int> lSortedColorPixels;
        //cout << fixed << setprecision(1) << 9.09090901 << endl;

        xOld = x;
        while (y < maxY) {
            x = xOld;
            while (x < maxX) {
                //getColor
                ios_base::fixed;
                double lColor = lPixels[y * inB.width() + x];
                //
                if (lColor < 0.5) //  == 0 monochromatic pics
                    black++;
                else
                    white++;

                ++x;
            }
            ++y;
        }

        double lFitness = (black > white) ? white : black;
        total_black += black;
        total_white += white;

        lSum += lFitness; 

        ++aciBegin;
    }

    double lNormalize = lSum;

    lSum += lPenalty * (double) inB.height();
    lSum /= (double) (inB.height() + (total_black > total_white) ? total_white : total_black);
    return lSum;
}

double
embryo::distance(const Picture& inA, const Picture& inB) { //inA = mCandidate; inB = mTargetPic
    //first is mCandidatePic; mCandidatePic_#cells <= mTargetPic_#cells
    int lSize = inA.getAspectCellsSize();

    AspectCellConstIterator aciBegin;
    AspectCellConstIterator aciEnd;
    inA.getAspectCells(aciBegin, aciEnd);

    double lSum = 0.0;

    const double* lPixels = inB.pixels();

    int lMaxCellSize = 1;
    Randomizer ran;
    ran.init(1985);

    int x = 0;
    int xOld = 0;
    int y = 0;
    while (aciBegin != aciEnd) {
        double lAspectCandidate = (*aciBegin)->getAspect();
        x = (*aciBegin)->getPositionX();
        y = (*aciBegin)->getPositionY();
        int maxX = x + sqrt((*aciBegin)->getSize());
        int maxY = y + sqrt((*aciBegin)->getSize());

                xOld = x;
                while (y < maxY) {
                    x = xOld;
                    while (x < maxX) {
                        //getColor
                        int coordinate = y * inB.width() + x;
                        double lDiff = lAspectCandidate - lPixels[coordinate];
        
                        lSum += lDiff * lDiff;
                        ++x;
                    }
                    ++y;
                }
        ++aciBegin;
    }

    return ((double)(lSum / (double) (inB.getAspectCellsSize()))); //lNbPixels);
}

//color distance fitness function #2 (distance between a random pixel of covered by a cell and the cell)
double
embryo::distance2(const Picture& inA, const Picture& inB) { //inA = mCandidate; inB = mTargetPic
    //first is mCandidatePic; mCandidatePic_#cells <= mTargetPic_#cells
    int lSize = inA.getAspectCellsSize();

    AspectCellConstIterator aciBegin;
    AspectCellConstIterator aciEnd;
    inA.getAspectCells(aciBegin, aciEnd);

    double lSum = 0.0;

    const double* lPixels = inB.pixels();

    int lMaxCellSize = 1;
    Randomizer ran;
    ran.init(1985);

    int x = 0;
    int xOld = 0;
    int y = 0;
    while (aciBegin != aciEnd) {
        double lAspectCandidate = (*aciBegin)->getAspect();
        x = (*aciBegin)->getPositionX();
        y = (*aciBegin)->getPositionY();
        int maxX = x + sqrt((*aciBegin)->getSize());
        int maxY = y + sqrt((*aciBegin)->getSize());

        int ranX = (ran.uint32() % (maxX - x)) + x;
        int ranY = (ran.uint32() % (maxY - y)) + y;
        int coordinate = ranY * inB.width() + ranX;
//        double lDiff = lAspectCandidate - lPixels[coordinate];
        double lDiff = fabs(lAspectCandidate - lPixels[coordinate]);

        int lCellSize = (*aciBegin)->getSize();
        if (lCellSize > lMaxCellSize)
            lMaxCellSize = lCellSize;
//        lSum += lDiff * lDiff * lCellSize;
        lSum += lDiff * lCellSize;

        ++aciBegin;
    }

    return ((double) (lSum / (double) (inA.getAspectCellsSize() * lMaxCellSize))); 
}

double
embryo::distance(const double& inPixelA, const double& inPixelB) {
    double lDiff = inPixelA - inPixelB;
    return (lDiff * lDiff);
}

double
embryo::maxDistance(const Picture& inPic) {
    size_t lNbPixels = inPic.getAspectCellsSize();

    AspectCellConstIterator aciBegin;
    AspectCellConstIterator aciEnd;
    inPic.getAspectCells(aciBegin, aciEnd);

    double lSum = 0.0;
    while (aciBegin != aciEnd) {
        double lDiff = std::max((*aciBegin)->getAspect(), 1.0 - (*aciBegin)->getAspect());
        lSum += lDiff * lDiff;
        ++aciBegin;
    }

    return lSum / lNbPixels;
}

void Picture::init() {
    for (int i = 0; i < mHeight; i++)
        for (int j = 0; j < mWidth; j++)
            mAspectCells.push_back(new AspectCell(i, j, 1, 0.0));
}

void Picture::init(int iSize, double iAspect) {
    for (int i = 0; i < mHeight; i++)
        for (int j = 0; j < mWidth; j++)
            mAspectCells.push_back(new AspectCell(i * iSize, j * iSize, iSize, iAspect));
}

void Picture::eraseAspectCells() {
    AspectCellIterator it = mAspectCells.begin();
    AspectCellIterator ite = mAspectCells.end();
    while (it != ite) {
        delete (*it);
        (*it) = NULL;
        it = mAspectCells.erase(it);
    }
    mAspectCells.clear();
}

void Picture::setNextAspectCell(int iPositionX, int iPositionY, int iSize, double iAspect) {
    mAspectCells.push_back(new AspectCell(iPositionX, iPositionY, iSize, iAspect));
}

void Picture::print() {
    AspectCellIterator it = mAspectCells.begin();
    AspectCellIterator ite = mAspectCells.end();
    int c = 0;
    while (it != ite) {
        printf("%d> x: %d, y: %d, s: %d, a: %e \n", ++c, (*it)->getPositionX(), (*it)->getPositionY(), (*it)->getSize(), (*it)->getAspect());
        ++it;
    }
}

void Picture::getAspectCells(AspectCellIterator & aciBegin, AspectCellIterator & aciEnd) {
    aciBegin = mAspectCells.begin();
    aciEnd = mAspectCells.end();
}
