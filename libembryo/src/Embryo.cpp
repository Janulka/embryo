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
#include <list>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cmath>
#include <omp.h>
#include <set>
#include "embryo.h"
#include "Embryo.h"
#include "MapTreeParser.h"
#include "ObjectFactoryDealer.h"
// #include "ArrayOps.h"
#include "Exception.h"
#include "MLPController.h"
#include "ESNController.h"
#include "FixedMonitor.h"
#include "EnergyMonitor.h"

#include "Picture.h"
#include "PGM.h"

using namespace std;
using namespace embryo;

typedef unsigned int uint;

int gCellCounter = 0;

class exceptionDelete : public std::exception {
public:

    virtual const char* what() const throw () {
        return "error by deleting";
    }
};

class exceptionMerge : public std::exception {
private:
    int reas;
public:

    exceptionMerge(int i) : std::exception() {
        reas = i;
    }

    virtual const char* what() const throw () {
        static char s[50];
        sprintf(s, "error by merging %d", reas);
        return s;
        // return "error by merging";
    }
};

class exceptionFinding : public std::exception {
public:

    virtual const char* what() const throw () {
        return "error by searching cell";
    }
};

void
safeOpen(ifstream& inFile, const std::string& inFileName) {
    inFile.open(inFileName.c_str());
    if (!inFile.good()) {
        cerr
                << "Unable to open '"
                << inFileName
                << "'"
                << endl;
        exit(EXIT_FAILURE);
    }
}

uint getCPUsCount() {
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    /*
    I read somewhere that the processor ID could be repeated. I don't know if
    that's true or not.
     */
    std::set<unsigned int> IDs;
    while (!cpuinfo.eof()) {
        std::getline(cpuinfo, line);
        if (!line.size())
            continue;
        if (line.find("processor") != 0)
            continue;
        size_t start = line.find(':'),
                end;
        for (; line[start] < '0' || line[start] > '9'; start++);
        for (end = start; line[end] >= '0' && line[end] <= '9'; end++);
        line = line.substr(start, end - start);
        IDs.insert(atoi(line.c_str()));
    }
    return IDs.size();
}

namespace embryo {

    struct FireOnInit {
        const Embryo& mEmbryo;

        FireOnInit(const Embryo & inEmbryo) : mEmbryo(inEmbryo) {
        }

        inline void operator () (EmbryoListener * inListener) {
            inListener->onInit(mEmbryo);
        }
    }
    ; // struct FireOnInit

    struct FireOnUpdate {
        const Embryo& mEmbryo;

        FireOnUpdate(const Embryo & inEmbryo) : mEmbryo(inEmbryo) {
        }

        inline void operator () (EmbryoListener * inListener) {
            inListener->onUpdate(mEmbryo);
        }
    }
    ; // struct FireOnUpdate
} // namespace embryo




// --- Embryo -----------------------------------------------------------------

Embryo::Embryo(size_t inWidth, size_t inHeight,
        size_t inNbStates,
        size_t inNbChemicals,
        const Picture * inTargetPic,
        size_t inSize,
        HandleT<Monitor> inMonitor,
        HandleT<Controller> inController,
        bool inDiffuseChemicals,
        bool ibDifferentStatesInitialization) : mbDifferentStatesInitialization(ibDifferentStatesInitialization), mDiffuseChemicals(inDiffuseChemicals), mWidth(inWidth), mHeight(inHeight), mNbStates(inNbStates), mNbChemicals(inNbChemicals), /*mTargetPic2(inTargetPic2),*/ mCandidatePic(inTargetPic[0].width(), inTargetPic[0].height()), mMonitor(inMonitor), mController(inController), mPicsNo(inSize) {
    mTargetPic = inTargetPic;
    setup();
    mController->init(mNbStates, mNbChemicals);
}

void
Embryo::setup() {
    mMaxDist = new double[mPicsNo];
    for (size_t i = 0; i < mPicsNo; i++) {
        mMaxDist[i] = maxDistance(mTargetPic[i]);
    }
    mNbStepsMax = mNbCells;
    mBorderVector = new double[(2 * (mWidth + mHeight)) * mNbChemicals];
}

void Embryo::initCells(size_t initState) {
    if (!(mCellContainer.empty())) {
        CellIterator itr = mCellContainer.begin();
        while (itr != mCellContainer.end()) {
            delete (*itr);
            (*itr) = NULL;
            itr = mCellContainer.erase(itr);
        }
    }

    mNbCells = mWidth * mHeight;
    bool lLeftBorder;
    bool lRightBorder;
    bool lUpperBorder;
    bool lLowerBorder;
    for (size_t i = 0; i < mNbCells; i++) {
        lLeftBorder = false;
        lRightBorder = false;
        lUpperBorder = false;
        lLowerBorder = false;
        if ((i % mWidth) == 0)
            lLeftBorder = true;
        if (i > (mWidth - 1))
            if (((i - mWidth + 1) % mWidth) == 0)
                lRightBorder = true;
        if (i < mWidth)
            lUpperBorder = true;
        if (i >= ((mHeight - 1) * mWidth))
            lLowerBorder = true;
        int x = i % mWidth;
        int y = (int) (i / mWidth);
        double value = 0.0;
        if (mbDifferentStatesInitialization) {
            //0 near border, 1 in middle
            int middle = (mHeight > mWidth) ? mHeight : mWidth;
            middle /= 2;
            middle--;
            int _x = (x > middle) ? (mWidth - x) : x;
            int _y = (y > middle) ? (mHeight - y) : y;
            value = (_x < _y) ? _x : _y;
            value /= (double) middle;
        }
        mCellContainer.push_back(new Cell(mNbStates, mNbChemicals, y, x, 1, i, lLeftBorder, lRightBorder, lUpperBorder, lLowerBorder, value));
    }

    ////
    //SET NEIGHBORS
    ///
    CellIterator ci = mCellContainer.begin();
    CellIterator ci2 = mCellContainer.begin();

    //CellIterator ciL = mCellContainer.begin();
    CellIterator ci3 = mCellContainer.begin();

    size_t i = 0;
    ++ci3;
    while (i < (mWidth - 1)) {
        (*ci2)->addNeighbor(*ci3, 2); // add east (right) neighbor
        (*ci3)->addNeighbor(*ci2, 6); // add west (left) neighbor
        ++ci2;
        ++ci3;
        ++i;
    }

    ++ci2;
    ++ci3;
    i = 0;

    while (ci2 != mCellContainer.end()) {
        (*ci)->addNeighbor(*ci2, 4); // add south neighbor
        (*ci2)->addNeighbor(*ci, 0); // add north neighbor

        if (i < (mWidth - 1)) {
            (*ci2)->addNeighbor(*ci3, 2); // add east (right) neighbor
            (*ci3)->addNeighbor(*ci2, 6); // add west (left) neighbor
        } else if (i == (mWidth - 1)) {
            i = -1;
        }

        ++ci2;
        ++ci;
        ++ci3;
        ++i;
    }
    arrayd::fillBorder(mBorderVector, 2 * (mWidth + mHeight) * mNbChemicals, initState, mPicsNo);

    // Initialize the monitor
    mMonitor->init(*this);

    // Listeners
    for_each(mListeners.begin(), mListeners.end(), FireOnInit(*this));
}

Embryo::~Embryo() {
    delete[] mBorderVector;
    delete[] mMaxDist;
    CellIterator itr = mCellContainer.begin();
    while (itr != mCellContainer.end()) {
        delete (*itr);
        (*itr) = NULL;
        itr = mCellContainer.erase(itr);
    }
}

void
Embryo::addListener(EmbryoListener* inListener) {
    if (find(mListeners.begin(), mListeners.end(), inListener) != mListeners.end())
        throw Exception("Same EmbryoListener added twice");

    mListeners.push_back(inListener);
}

void
Embryo::removeListener(EmbryoListener* inListener) {
    vector<EmbryoListener*>::iterator it =
            find(mListeners.begin(), mListeners.end(), inListener);

    if (it == mListeners.end())
        throw Exception("Try to remove an EmbryoListener no previously added");

    mListeners.erase(it);
}

void
Embryo::init(size_t initState) {

    //    Randomizer ran;
    //    ran.init(1985);
    double value = 0.0;
    CellIterator ci = mCellContainer.begin();
    while (ci != mCellContainer.end()) {
        //        value = ran.real();
        (*ci)->init(value);
        ++ci;
    }
    arrayd::fillBorder(mBorderVector, 2 * (mWidth + mHeight) * mNbChemicals, initState, mPicsNo);

    // Initialize the monitor
    mMonitor->init(*this);

    // Listeners
    for_each(mListeners.begin(), mListeners.end(), FireOnInit(*this));

}

void
Embryo::resetMonitor(size_t inBorder) { //(double inBorder) {

    //fixed vs. energy - control the ending condition

    // Initialize the monitor
    mMonitor->init(*this);

    arrayd::fillBorder(mBorderVector, 2 * (mWidth + mHeight) * mNbChemicals, inBorder, mPicsNo);
}

void
Embryo::reshape(size_t inWidth, size_t inHeight) {
    if ((inWidth == mWidth) && (inHeight == mHeight))
        return;

    // Redo a setup
    mWidth = inWidth;
    mHeight = inHeight;
    setup();
}

void
Embryo::scramble(Randomizer& inRandomizer, double inStdDev) {
    GaussianDistribution lDistrib;

    // Chemicals scramble

    CellIterator ci = mCellContainer.begin();
    while (ci != mCellContainer.end()) {
        (*ci)->scramble(inRandomizer, inStdDev);
        ++ci;
    }
}

double
Embryo::energy() const {
    //TDO: counting energy for monitor
    double lSquareSum = 0.0;
    double lSquare = 0.0;

    CellConstIterator it = mCellContainer.begin();
    while (it != mCellContainer.end()) {
        // Chemicals sum
        lSquare = 0.0;
        if ((*it) != NULL) {
            {
                const double* lOffset = (*it)->getChemicalVector();
                for (size_t i = mNbChemicals; i != 0; --i, ++lOffset) {
                    if ((!(std::isnan(lOffset[i]))) && (!(std::isinf(lOffset[i]))))
                        lSquare += (*lOffset) * (*lOffset);
                }
            }
            // States sums
            {
                const double* lOffset = (*it)->getStateVector();
                for (size_t i = mNbStates; i != 0; --i, ++lOffset) {
                    if ((!(std::isnan(lOffset[i]))) && (!(std::isinf(lOffset[i]))))
                        lSquare += (*lOffset) * (*lOffset);
                }
            }

            lSquare *= ((double) ((*it)->getSize()));
            lSquareSum += lSquare;
        }
        ++it;
    }
    return lSquareSum;
}

void
Embryo::getColours(Picture & inPicture) const {
    inPicture.eraseAspectCells();

    int c = 0;
    CellConstIterator ci = mCellContainer.begin();
    while (ci != mCellContainer.end()) {
        const double* lSrcStates = (*ci)->getStateVector();
        const double* lSrcChems = (*ci)->getChemicalVector();

        double lAspect = mController->colour(lSrcStates, lSrcChems);
        inPicture.setNextAspectCell((*ci)->getPositionX(), (*ci)->getPositionY(), (*ci)->getSize(), lAspect);
        ++ci;
    }
}

void
Embryo::getChemicals(Picture& inPicture, size_t inIndex) const {
    inPicture.eraseAspectCells();
    CellConstIterator ci = mCellContainer.begin();
    while (ci != mCellContainer.end()) {
        const double* chem = (*ci)->getChemicalVector() + inIndex;
        inPicture.setNextAspectCell((*ci)->getPositionX(), (*ci)->getPositionY(), (*ci)->getSize(), *chem);
        ++ci;
    }
}

void
Embryo::getStates(Picture& inPicture, size_t inIndex) const {

    inPicture.eraseAspectCells();
    CellConstIterator ci = mCellContainer.begin();
    while (ci != mCellContainer.end()) {
        const double* st = (*ci)->getChemicalVector() + inIndex; //TODO: need of inIndex?
        inPicture.setNextAspectCell((*ci)->getPositionX(), (*ci)->getPositionY(), (*ci)->getSize(), *st);
        ++ci;
    }
}

double Embryo::getColorOnPixel(const size_t x, const size_t y, Picture & pic) {

    AspectCellIterator aciBegin;
    AspectCellIterator aciEnd;
    pic.getAspectCells(aciBegin, aciEnd);

    while (aciBegin != aciEnd) {
        if (((*aciBegin)->getPositionX() <= x) && ((*aciBegin)->getPositionY() <= y) && ((*aciBegin)->getPositionX() + sqrt((*aciBegin)->getSize()) > x) && ((*aciBegin)->getPositionY() + sqrt((*aciBegin)->getSize()) > y)) {
            return (*aciBegin)->getAspect();
        }
        ++aciBegin;
    }

    return NULL;
}

void
Embryo::diffuseChemicals() {
    /*
     * Gaussian blur order 2, two passes algorithm
     */

    // First pass (horizontal)

    CellIterator ci = mCellContainer.begin();
    while (ci != mCellContainer.end()) {
        if ((!(*ci)->toDelete())) {
            (*ci)->blurFirstPass();
        }
        ++ci;
    }

    // Second pass (vertical)

    ci = mCellContainer.begin();
    while (ci != mCellContainer.end()) {
        if (!((*ci)->toDelete())) {
            (*ci)->blurSecondPass();
        }
        ++ci;
    }
}

bool Embryo::checkConstraints(Cell * iCell) {
    //circle
    double radius = mWidth;
    if (mHeight < mWidth)
        radius = mHeight;
    //TODO:
    int tmpX = iCell->getPositionX() - iCell->getSize() - mWidth / 2;
    int tmpY = iCell->getPositionY() - iCell->getSize() - mHeight / 2;

    int tmpX2 = iCell->getPositionX() + iCell->getSize() - mWidth / 2;
    int tmpY2 = iCell->getPositionY() + iCell->getSize() - mHeight / 2;

    if ((tmpX * tmpX + tmpY * tmpY < radius * radius) && (tmpX2 * tmpX2 + tmpY2 * tmpY2 < radius * radius))
        return true;

    return false;
}

void Embryo::updateStructure() {
    bool bGrowing = false;
    Direction oDir1;
    Direction oDir2;
    Cell * oCell1;
    Cell * oCell2;
    Cell * oCell3;
    double * newStateVector;


    CellIterator ci = mCellContainer.begin();
    while (ci != mCellContainer.end()) {
        if ((*ci != NULL) && (!(*ci)->isVisited()) && (!(*ci)->toDelete())) {
            if ((*ci)->wantsGrow()) {

                bGrowing = (*ci)->getGrowingDirections(oDir1, oDir2, oCell1, oCell2, oCell3);

                if (bGrowing) {
                    int lNewX = (oCell2->getPositionX() < (*ci)->getPositionX()) ? oCell2->getPositionX() : (*ci)->getPositionX();
                    int lNewY = (oCell1->getPositionY() < (*ci)->getPositionY()) ? oCell1->getPositionY() : (*ci)->getPositionY();
                    int lNewLength = sqrt((*ci)->getSize() * 4);
                    bool bModPos = false;

                    //                    if ((lNewX % lNewLength == 0) && (lNewY % lNewLength == 0))
                    bModPos = true;
                    if (bModPos) {
                        if ((changeNeighborhood(*ci, oDir1, oDir2))) {
                            newStateVector = (*ci)->getStateVectorAdr();

                            removeMeGetState(oCell1, newStateVector);
                            removeMeGetState(oCell2, newStateVector);
                            removeMeGetState(oCell3, newStateVector);


                            //interpolation of states
                            for (int s = 0; s < mNbStates; s++)
                                newStateVector[s] /= ((double) 4.0);

                            mCellChange = true;
                        }
                    }

                }
            } else if ((*ci)->wantsDivide()) {
                divide(*ci);
            }

        }
        ++ci;
    }
}

void Embryo::updateStructureSingle(Cell *& iCell) {
    bool bGrowing = false;
    Direction oDir1;
    Direction oDir2;
    Cell * oCell1;
    Cell * oCell2;
    Cell * oCell3;
    double * newStateVector;
    if ((iCell != NULL) && (!(iCell)->isVisited()) && (!(iCell)->toDelete())) {
        if ((iCell)->wantsGrow()) {

            bGrowing = (iCell)->getGrowingDirections(oDir1, oDir2, oCell1, oCell2, oCell3);

            if (bGrowing) {
                int lNewX = (oCell2->getPositionX() < (iCell)->getPositionX()) ? oCell2->getPositionX() : (iCell)->getPositionX();
                int lNewY = (oCell1->getPositionY() < (iCell)->getPositionY()) ? oCell1->getPositionY() : (iCell)->getPositionY();
                int lNewLength = sqrt((iCell)->getSize() * 4);
                bool bModPos = false;

                //                    if ((lNewX % lNewLength == 0) && (lNewY % lNewLength == 0))
                bModPos = true;
                if (bModPos) {

                    if ((changeNeighborhood(iCell, oDir1, oDir2))) {
                        newStateVector = (iCell)->getStateVectorAdr();

                        removeMeGetState(oCell1, newStateVector);
                        removeMeGetState(oCell2, newStateVector);
                        removeMeGetState(oCell3, newStateVector);


                        //interpolation of states
                        for (int s = 0; s < mNbStates; s++)
                            newStateVector[s] /= ((double) 4.0);

                        mCellChange = true;
                    }
                }

            }
        } else if ((iCell)->wantsDivide()) {
            divide(iCell);
        }

    }
}

bool
Embryo::update(bool ibColor) {
    // Diffusion of the chemicals
    //    bool bCellChange = false;
    mCellChange = false;

    if (mDiffuseChemicals)
        diffuseChemicals();


    int lNbChemCells = 5; 
    bool * bIsNeighbor = new bool[lNbChemCells];

    double ** outNeighborsChemicals = new double *[lNbChemCells];
    for (int i = 0; i < lNbChemCells; ++i) {
        outNeighborsChemicals[i] = new double[mNbChemicals];
    }

    for (int i = 0; i < lNbChemCells; ++i)
        bIsNeighbor[i] = true;

    CellIterator ci;

    for (ci = mCellContainer.begin(); ci != mCellContainer.end(); ci++) {
        double * outOldState;
        double * outNewState;
        double * outNewChemicals;

        (*ci)->update(outOldState, outNeighborsChemicals, outNewState, outNewChemicals, bIsNeighbor);

        mController->update((const double* &) outOldState, (double * const* &) outNeighborsChemicals, outNewState, outNewChemicals, bIsNeighbor); // growing/dividing request is set

        if (!ibColor) {
            double oGrowing;
            const double* lSrcStates = outNewState; //(*ci)->getStateVector();
            const double* lSrcChems = outNewChemicals; //(*ci)->getChemicalVector();
            oGrowing = mController->structure(lSrcStates, lSrcChems);

            double up = 1.0;
            double down = -1.0;
            double range = 2.0;
            double intervals = 3.0;

            (*ci)->setGrowing(false);
            (*ci)->setDividing(false);
            if (oGrowing < (down + (range / intervals))) {
                (*ci)->setDividing(true);
            } else if (oGrowing >= (up - (range / intervals))) {
                (*ci)->setGrowing(true);
            }
        }
    }

    int maxth = omp_get_max_threads();

    if (!ibColor) {
        updateStructure();

        ci = mCellContainer.begin();
        while (ci != mCellContainer.end()) {
            if ((*ci)->toDelete()) {
                delete (*ci);
                (*ci) = NULL;
                ci = mCellContainer.erase(ci);
            } else {
                (*ci)->setVisited(false);
                ++ci;
            }
        }
    }

    for (int i = 0; i < lNbChemCells; ++i)
        if (outNeighborsChemicals[i] != NULL) {
            delete outNeighborsChemicals[i];
            outNeighborsChemicals[i] = NULL;
        }

    if (outNeighborsChemicals)
        delete[] outNeighborsChemicals;

    if (bIsNeighbor)
        delete[] bIsNeighbor;


    // Ask the monitor if should stop
    mMonitor->next(*this);

    // Listeners
    for_each(mListeners.begin(), mListeners.end(), FireOnUpdate(*this));

    // Job done
    return mMonitor->hasNext(ibColor);
}
//}

bool sorter(Cell * iCell1, Cell * iCell2) {
    return (iCell1->getIndex() < iCell2->getIndex());
}

bool duplicate(Cell * iCell1, Cell * iCell2) {
    return (iCell1->getIndex() == iCell2->getIndex());
}

void Embryo::removeMeGetState(Cell *& iCellP, double *& iStateVector) {
    const double * tmpState = (iCellP)->getStateVector();
    iCellP->setToDelete();
    if ((tmpState == NULL) || (iStateVector == NULL))
        return;
    for (int s = 0; s < mNbStates; s++) {

        double d = tmpState[s];

        if ((!(std::isnan(d))) && (!(std::isinf(d))) && (!(std::isnan(iStateVector[s]))) && (!(std::isinf(iStateVector[s])))) {
            //            std::cout << d << " | " << iStateVector[s] << std::endl;
            ((iStateVector)[s]) += d;
        }
    }
}

void Embryo::changeBorderStatus(Cell *& iNewCell, Cell * iC1, Cell * iC2, Cell * iC3) {
    for (int i = 0; i < 8; i++) {
        if ((iC1->isOnTheBorder((Direction) i)) || (iC2->isOnTheBorder((Direction) i)) || (iC3->isOnTheBorder((Direction) i)))
            iNewCell->setAsBorder((Direction) i);
    }
}

void Embryo::printout() {
    std::cout << std::endl;
    CellIterator ci = mCellContainer.begin();
    while (ci != mCellContainer.end()) {
        if (!(*ci)->toDelete()) {
            std::cout << "\ni: " << (*ci)->getIndex() << " size: " << (*ci)->getSize() << " x: " << (*ci)->getPositionX() << " y: " << (*ci)->getPositionY() << "\n";
            CellIterator itrB;
            CellIterator itrE;
            for (int i = 0; i < 8; i++) {
                if (i % 2 == 0) {
                    printf("\t%d: ", i);
                    if (!((*ci)->isOnTheBorder((Direction) i)))
                        if ((*ci)->getNeighbors((Direction) i, itrB, itrE)) {
                            while (itrB != itrE) {
                                printf("%d,", (*itrB)->getIndex());
                                ++itrB;
                            }
                        }
                }
            }
        }
        ++ci;
    }
    printf("\n\n");
}

bool Embryo::changeNeighborhood(Cell *& iNewCell, Direction & iDir1, Direction & iDir2) {

    Direction iDirOp1 = (Direction) ((iDir1 >= 4) ? (iDir1 - 4) : (iDir1 + 4));
    Direction iDirOp2 = (Direction) ((iDir2 >= 4) ? (iDir2 - 4) : (iDir2 + 4));

    //change neighbors of merged cells
    Cell * lNeighborCell1 = NULL;
    if (!(iNewCell->getSingleNeighbor(iDir1, lNeighborCell1)))
        return false;
    Cell * lNeighborCell2 = NULL;
    if (!(iNewCell->getSingleNeighbor(iDir2, lNeighborCell2)))
        return false;

    if ((lNeighborCell1 == NULL) || (lNeighborCell2 == NULL) || (lNeighborCell1->getIndex() == lNeighborCell2->getIndex())) {
        printout();
        throw exceptionMerge(2);
    }

    Cell * lNeighborCellCross = NULL;
    if (!(lNeighborCell1->getSingleNeighbor(iDir2, lNeighborCellCross)))
        return false;
    Cell * lNeighborCellCrossCheck = NULL;
    if (!(lNeighborCell2->getSingleNeighbor(iDir1, lNeighborCellCrossCheck)))
        return false;

    if ((lNeighborCellCross == NULL) || (lNeighborCellCross->getIndex() != lNeighborCellCrossCheck->getIndex())) {
        printout();
        throw exceptionMerge(3);
        //        return false;
    }

    if ((iNewCell->toDelete()) || (lNeighborCell1->toDelete()) || (lNeighborCell2->toDelete()) || (lNeighborCellCross->toDelete()))
        return false;

    //change my own neighborhood
    CellIterator ocb;
    CellIterator oce;

    CellContainer cc;
    CellContainer cc2;

    changeBorderStatus(iNewCell, lNeighborCell1, lNeighborCell2, lNeighborCellCross);

    if (lNeighborCell1->getContainer(iDir1, cc)) { //getting neigborhs in direction iDir1 of my current neighbor from direction iDir1

        if (!(lNeighborCellCross->getContainer(iDir1, cc2))) //neigbors to merge
        {
            printout();
            printf("\n merging %d, dir: %d, %d :: %d | %d | %d\n", iNewCell->getIndex(), iDir1, iDir2, lNeighborCell1->getIndex(), lNeighborCell2->getIndex(), lNeighborCellCross->getIndex());
            throw exceptionMerge(4);
        }

        cc.sort(sorter); //sort
        cc2.sort(sorter);
        cc.merge(cc2); //merging neigbors
        cc.unique(duplicate); //remove duplicates

        ocb = cc.begin();
        oce = cc.end();
        iNewCell->changeAllMyNeighbors(iDir1, ocb, oce); //changing my current neigbors

        //change neighbors of neighbors of the merged cells
        lNeighborCell1->changeNeighborhood(iDir1, iDirOp1, lNeighborCell1, lNeighborCellCross, iNewCell);
        lNeighborCellCross->changeNeighborhood(iDir1, iDirOp1, lNeighborCellCross, lNeighborCell1, iNewCell);

        if (!(iNewCell->getContainer(iDir1, cc))) //my neigbors from direction iDir1 - currently assigned from lNeighborCell1
        {
            printout();
            printf("\n merging %d, dir: %d, %d :: %d | %d | %d\n", iNewCell->getIndex(), iDir1, iDir2, lNeighborCell1->getIndex(), lNeighborCell2->getIndex(), lNeighborCellCross->getIndex());
            throw exceptionMerge(5);
        }
    } else
        iNewCell->setAsBorder(iDir1);

    if ((iNewCell->getContainer(iDirOp2, cc))) {
        if (!(lNeighborCell1->getContainer(iDirOp2, cc2))) {
            printout();
            printf("\n merging %d, dir: %d, %d :: %d | %d | %d\n", iNewCell->getIndex(), iDir1, iDir2, lNeighborCell1->getIndex(), lNeighborCell2->getIndex(), lNeighborCellCross->getIndex());
            throw exceptionMerge(6);
        }

        cc.sort(sorter); //sort
        cc2.sort(sorter);
        cc.merge(cc2); //merging neigbors
        cc.unique(duplicate); //remove duplicates

        ocb = cc.begin();
        oce = cc.end();
        iNewCell->changeAllMyNeighbors(iDirOp2, ocb, oce);

        //change neighbors of neighbors of the merged cells
        lNeighborCell1->changeNeighborhood(iDirOp2, iDir2, lNeighborCell1, iNewCell);

        if (!(iNewCell->getContainer(iDirOp2, cc))) {
            printout();
            printf("\n merging %d, dir: %d, %d :: %d | %d | %d\n", iNewCell->getIndex(), iDir1, iDir2, lNeighborCell1->getIndex(), lNeighborCell2->getIndex(), lNeighborCellCross->getIndex());
            throw exceptionMerge(7);
        }

    } else
        iNewCell->setAsBorder(iDirOp2);

    if (lNeighborCell2->getContainer(iDir2, cc)) {

        if (!(lNeighborCellCross->getContainer(iDir2, cc2))) {
            printout();
            printf("\n merging %d, dir: %d, %d :: %d | %d | %d\n", iNewCell->getIndex(), iDir1, iDir2, lNeighborCell1->getIndex(), lNeighborCell2->getIndex(), lNeighborCellCross->getIndex());
            throw exceptionMerge(8);
        }

        cc.sort(sorter); //sort
        cc2.sort(sorter);
        cc.merge(cc2); //merging neigbors
        cc.unique(duplicate); //remove duplicates

        ocb = cc.begin();
        oce = cc.end();
        iNewCell->changeAllMyNeighbors(iDir2, ocb, oce);

        //change neighbors of neighbors of the merged cells
        lNeighborCell2->changeNeighborhood(iDir2, iDirOp2, lNeighborCell2, lNeighborCellCross, iNewCell);
        lNeighborCellCross->changeNeighborhood(iDir2, iDirOp2, lNeighborCellCross, lNeighborCell2, iNewCell);

        if (!(iNewCell->getContainer(iDir2, cc))) {
            printout();
            printf("\n merging %d, dir: %d, %d :: %d | %d | %d\n", iNewCell->getIndex(), iDir1, iDir2, lNeighborCell1->getIndex(), lNeighborCell2->getIndex(), lNeighborCellCross->getIndex());
            throw exceptionMerge(9);
        }

    } else
        iNewCell->setAsBorder(iDir2);

    //    printf("\nDir %d: ", iDirOp1);
    if ((iNewCell->getContainer(iDirOp1, cc))) {
        if (!(lNeighborCell2->getContainer(iDirOp1, cc2))) {
            printout();
            printf("\n merging %d, dir: %d, %d :: %d | %d | %d\n", iNewCell->getIndex(), iDir1, iDir2, lNeighborCell1->getIndex(), lNeighborCell2->getIndex(), lNeighborCellCross->getIndex());
            throw exceptionMerge(10);
        }

        cc.sort(sorter); //sort
        cc2.sort(sorter);
        cc.merge(cc2); //merging neigbors
        cc.unique(duplicate); //remove duplicates

        ocb = cc.begin();
        oce = cc.end();
        iNewCell->changeAllMyNeighbors(iDirOp1, ocb, oce);

        //change neighbors of neighbors of the merged cells
        lNeighborCell2->changeNeighborhood(iDirOp1, iDir1, lNeighborCell2, iNewCell);


        if (!(iNewCell->getContainer(iDirOp1, cc))) {
            printout();
            printf("\n merging %d, dir: %d, %d :: %d | %d | %d\n", iNewCell->getIndex(), iDir1, iDir2, lNeighborCell1->getIndex(), lNeighborCell2->getIndex(), lNeighborCellCross->getIndex());
            throw exceptionMerge(11);
        }

    } else
        iNewCell->setAsBorder(iDirOp1);

    //calibrating position
    if (iDir1 == N) {
        iNewCell->decreasePositionY(iNewCell->getSize());
    }
    if (iDir2 == W) {
        iNewCell->decreasePositionX(iNewCell->getSize());
    }

    //increase size
    iNewCell->multiSize(4);
    iNewCell->setGrowing(false);
    iNewCell->setDividing(false);

    lNeighborCell1->setToDelete();
    lNeighborCell2->setToDelete();
    lNeighborCellCross->setToDelete();
    iNewCell->setVisited(true);
    lNeighborCell1->setVisited(true);
    lNeighborCell2->setVisited(true);
    lNeighborCellCross->setVisited(true);

    return true;
}

void Embryo::divide(Cell *& iCell) {
    if ((iCell->getSize() == 1) || (iCell->toDelete()))
        return;

    mCellChange = true;

    int lSize = iCell->getSize() / 4;
    int lLength = sqrt(lSize);
    Cell * lCellNW = new Cell(mNbStates, mNbChemicals, iCell->getPositionY(), iCell->getPositionX(), lSize, mNbCells++, iCell->isOnTheBorder(W), false, iCell->isOnTheBorder(N), false, iCell);
    Cell * lCellNE = new Cell(mNbStates, mNbChemicals, iCell->getPositionY(), iCell->getPositionX() + lLength, lSize, mNbCells++, false, iCell->isOnTheBorder(E), iCell->isOnTheBorder(N), false, iCell);
    Cell * lCellSW = new Cell(mNbStates, mNbChemicals, iCell->getPositionY() + lLength, iCell->getPositionX(), lSize, mNbCells++, iCell->isOnTheBorder(W), false, false, iCell->isOnTheBorder(S), iCell);
    Cell * lCellSE = new Cell(mNbStates, mNbChemicals, iCell->getPositionY() + lLength, iCell->getPositionX() + lLength, lSize, mNbCells++, false, iCell->isOnTheBorder(E), false, iCell->isOnTheBorder(S), iCell);

    CellContainer ccNeighbors;
    CellIterator ccItrB;

    { //north
        if (!(iCell->isOnTheBorder(N))) {
            iCell->getContainer(N, ccNeighbors);
            ccNeighbors.unique(duplicate);
            ccItrB = ccNeighbors.begin();

            while ((ccItrB != ccNeighbors.end())) {
                if (((*ccItrB)->getPositionX()) < (lCellNE)->getPositionX()) {
                    lCellNW->addNeighbor(*ccItrB, N);
                    (*ccItrB)->addNeighbor(lCellNW, S);
                    if (((*ccItrB)->getPositionX() + sqrt((*ccItrB)->getSize())) > (lCellNE)->getPositionX()) {
                        lCellNE->addNeighbor(*ccItrB, N);
                        (*ccItrB)->addNeighbor(lCellNE, S);
                    }
                } else {
                    lCellNE->addNeighbor(*ccItrB, N);
                    (*ccItrB)->addNeighbor(lCellNE, S);
                }
                (*ccItrB)->removeNeighbor(S, iCell);

                ++ccItrB;
            }
        }
    }

    { //south
        if (!(iCell->isOnTheBorder(S))) {
            iCell->getContainer(S, ccNeighbors);
            ccNeighbors.unique(duplicate);
            ccItrB = ccNeighbors.begin();
            while (ccItrB != ccNeighbors.end()) {
                if (((*ccItrB)->getPositionX()) < (lCellSE)->getPositionX()) {
                    lCellSW->addNeighbor(*ccItrB, S);
                    (*ccItrB)->addNeighbor(lCellSW, N);
                    if (((*ccItrB)->getPositionX() + sqrt((*ccItrB)->getSize())) > (lCellSE)->getPositionX()) {
                        lCellSE->addNeighbor(*ccItrB, S);
                        (*ccItrB)->addNeighbor(lCellSE, N);
                    }
                } else {
                    lCellSE->addNeighbor(*ccItrB, S);
                    (*ccItrB)->addNeighbor(lCellSE, N);
                }
                (*ccItrB)->removeNeighbor(N, iCell);

                ++ccItrB;
            }
        }
    }

    { //west
        if (!(iCell->isOnTheBorder(W))) {
            iCell->getContainer(W, ccNeighbors);
            ccNeighbors.unique(duplicate);
            ccItrB = ccNeighbors.begin();
            while (ccItrB != ccNeighbors.end()) {
                if (((*ccItrB)->getPositionY()) < (lCellSW)->getPositionY()) {
                    lCellNW->addNeighbor(*ccItrB, W);
                    (*ccItrB)->addNeighbor(lCellNW, E);
                    if (((*ccItrB)->getPositionY() + sqrt((*ccItrB)->getSize())) > (lCellSW)->getPositionY()) {
                        lCellSW->addNeighbor(*ccItrB, W);
                        (*ccItrB)->addNeighbor(lCellSW, E);
                    }
                } else {
                    lCellSW->addNeighbor(*ccItrB, W);
                    (*ccItrB)->addNeighbor(lCellSW, E);
                }
                (*ccItrB)->removeNeighbor(E, iCell);


                ++ccItrB;
            }
        }
    }

    { //east
        if (!(iCell->isOnTheBorder(E))) {
            iCell->getContainer(E, ccNeighbors);
            ccNeighbors.unique(duplicate);
            ccItrB = ccNeighbors.begin();
            while (ccItrB != ccNeighbors.end()) {
                if (((*ccItrB)->getPositionY()) < (lCellSE)->getPositionY()) {
                    lCellNE->addNeighbor(*ccItrB, E);
                    (*ccItrB)->addNeighbor(lCellNE, W);
                    if (((*ccItrB)->getPositionY() + sqrt((*ccItrB)->getSize())) > (lCellSE)->getPositionY()) {
                        lCellSE->addNeighbor(*ccItrB, E);
                        (*ccItrB)->addNeighbor(lCellSE, W);
                    }
                } else {
                    lCellSE->addNeighbor(*ccItrB, E);
                    (*ccItrB)->addNeighbor(lCellSE, W);
                }

                (*ccItrB)->removeNeighbor(W, iCell);

                ++ccItrB;
            }
        }
    }

    lCellSE->addNeighbor(lCellNE, N);
    lCellSW->addNeighbor(lCellNW, N);
    lCellNE->addNeighbor(lCellNW, W);
    lCellSE->addNeighbor(lCellSW, W);
    lCellNW->addNeighbor(lCellNE, E);
    lCellSW->addNeighbor(lCellSE, E);
    lCellNE->addNeighbor(lCellSE, S);
    lCellNW->addNeighbor(lCellSW, S);

    lCellNW->setVisited(true);
    lCellNE->setVisited(true);
    lCellSE->setVisited(true);
    lCellSW->setVisited(true);

    mCellContainer.push_back(lCellNW);
    mCellContainer.push_back(lCellNE);
    mCellContainer.push_back(lCellSW);
    mCellContainer.push_back(lCellSE);


    iCell->setToDelete();
}

double Embryo::setupController(const double* inVector) {
    // Setup the parameters of the model
    try {
        arrayd::copy(mController->parameters(), inVector, mController->nbParameters());
    } catch (embryo::Exception& inException) {
        cerr
                << "error while reading controller "
                << endl;
    }
}

double Embryo::getSimilarity(size_t iIndex, bool ibColor) {
    //    double lSimilarity1 = 0.0;
    //    double lSimilarity2 = 0.0;
    double lSimilarity = 0.0;
    getColours(mCandidatePic);

    if (ibColor) {
        //COLOR FITNESS (to minimize)
        //    lSimilarity1 = (distance(mCandidatePic, mTargetPic[iIndex])) / mMaxDist[iIndex];
        lSimilarity = (distance(mCandidatePic, mTargetPic[iIndex])) / mMaxDist[iIndex];
    } else {
        //CELLS STRUCTURE FITNESS
        //    lSimilarity2 = (distanceStructure(mCandidatePic, mTargetPic[iIndex]));
        lSimilarity = (distanceStructure(mCandidatePic, mTargetPic[iIndex]));
    }
    return lSimilarity;
    //    return (double) ((lSimilarity1 + lSimilarity2) / 2.0);
    //        return (double) ((lSimilarity1 > lSimilarity2) ? lSimilarity1 : lSimilarity2);
}

double Embryo::evaluate(const double* inVector, size_t& outNbSteps, double& outSimilarity)//, const Picture& mTargetPic) 
{

    // Setup the parameters of the model
    try {
        arrayd::copy(mController->parameters(), inVector, mController->nbParameters());
    } catch (embryo::Exception& inException) {
        cerr
                << "error while reading controller "
                << endl;
    }



    size_t lNbSteps = 0;

    outNbSteps = 0;
    outSimilarity = 0;
    double lSimilaritySum = 0;
    double lSimilarity;

    double lPenalty = double(outNbSteps) / (mPicsNo * nbStepsMax());

    double bestFitness = 1.0;
    double worstFitness = 0.0;

    for (size_t i = 0, initCells(true), init(i); i < mPicsNo; i++) {
        lNbSteps = 0;
        lSimilarity = 0;
        for (resetMonitor(i); update(false) && (lNbSteps < nbStepsMax()); lNbSteps += 1);
        for (resetMonitor(i); update(true) && (lNbSteps < nbStepsMax()); lNbSteps += 1);

        if (lNbSteps < nbStepsMax()) {
            getColours(mCandidatePic);

            lSimilarity = (distance(mCandidatePic, mTargetPic[i])) / mMaxDist[i];
            double lPenalty = double(lNbSteps) / nbStepsMax();
            lSimilarity *= (lPenalty * lPenalty + 1.0);
        } else
            lSimilarity = 1.0;

        if (lSimilarity < bestFitness)
            bestFitness = lSimilarity;
        if (lSimilarity > worstFitness)
            worstFitness = lSimilarity;

        lSimilaritySum += lSimilarity;
        outNbSteps += lNbSteps;
        lNbSteps = 0;
    }

    lPenalty = double(outNbSteps) / (mPicsNo * nbStepsMax());
    //00
    outSimilarity = lSimilaritySum / mPicsNo;


    if (outSimilarity > 1) {
        outSimilarity = 1;
    } else if (outSimilarity < 0) {
        outSimilarity = 0;
    }

    return outSimilarity;

}

void Embryo::loadCells(std::istream & inStream) {
    inStream.exceptions(ios::failbit | ios::badbit);

    inStream >> mNbCells;

    int lIndex;
    int lSize;
    int lX;
    int lY;
    bool lLeftBorder;
    bool lRightBorder;
    bool lUpBorder;
    bool lDownBorder;
    int lTF;
    Cell * lCell;
    double lValue = 0.0;

    try {
        for (size_t i = 0; i < mNbCells; i++) {
            inStream >> lIndex;
            inStream >> lSize;
            inStream >> lX;
            inStream >> lY;
            inStream >> lTF;
            //            if (lTF == )
            lLeftBorder = lTF;
            inStream >> lTF;
            lRightBorder = lTF;
            inStream >> lTF;
            lUpBorder = lTF;
            inStream >> lTF;
            lDownBorder = lTF;

            lCell = new Cell(mNbStates, mNbChemicals, lY, lX, lSize, lIndex, lLeftBorder, lRightBorder, lUpBorder, lDownBorder, 0.0);
            mCellContainer.push_back(lCell);

        }

        int lNumber;
        CellIterator ccItrB = mCellContainer.begin();
        while (ccItrB != mCellContainer.end()) {
            for (int d = 0; d < 8; d++) {
                if ((d % 2) == 0) {
                    inStream >> lNumber;
                    for (size_t i = 0; i < lNumber; i++) {
                        inStream >> lIndex;
                        if (!(findCell(lIndex, lCell)))
                            throw exceptionFinding();
                        (*ccItrB)->addNeighbor(lCell, (Direction) d);
                    }
                }
            }
            ++ccItrB;
        }
    } catch (exception& inException) {
        throw Exception(inException.what());
    }
}

bool Embryo::findCell(int iIndex, Cell *& oCell) {
    CellIterator ccItrB = mCellContainer.begin();
    while (ccItrB != mCellContainer.end()) {
        if ((*ccItrB)->getIndex() == iIndex) {
            oCell = (*ccItrB);
            return true;
        }
        ++ccItrB;
    }
    return false;
}

void Embryo::saveCells(std::ostream & inStream) {
    //    inStream.setf(ios::out | ios::binary);
    inStream << mCellContainer.size();
    CellIterator ccItrB = mCellContainer.begin();
    while (ccItrB != mCellContainer.end()) {
        inStream << ' ' << (*ccItrB)->getIndex() << ' ' << (*ccItrB)->getSize() << ' ' << (*ccItrB)->getPositionX() << ' ' << (*ccItrB)->getPositionY() << ' ' << (*ccItrB)->isOnTheBorder(W) << ' ' << (*ccItrB)->isOnTheBorder(E) << ' ' << (*ccItrB)->isOnTheBorder(N) << ' ' << (*ccItrB)->isOnTheBorder(S); // << endl;
        ++ccItrB;
    }
    // add neighbors
    CellContainer ccNeigbors;
    ccItrB = mCellContainer.begin();
    CellIterator ccItrNeig;
    while (ccItrB != mCellContainer.end()) {
        for (int d = 0; d < 8; d++) {
            if ((d % 2) == 0) {
                if ((*ccItrB)->getContainer((Direction) d, ccNeigbors)) {
                    inStream << ' ' << ccNeigbors.size();
                    ccItrNeig = ccNeigbors.begin();
                    while (ccItrNeig != ccNeigbors.end()) {
                        inStream << ' ' << (*ccItrNeig)->getIndex();
                        ++ccItrNeig;
                    }
                } else {
                    int nula = 0;
                    inStream << ' ' << nula;
                }
            }
        }
        ++ccItrB;
    }
    inStream << endl;
}

Embryo::Handle
Embryo::load(istream& inStream, uint32_t inSeed, const std::string & pathContent, bool & oGuiOuput) {

    // Read the input stream
    MapTreeParser lParser(inStream);
    HandleT<MapTree> lRoot = lParser.parse();

    lRoot->add("path", pathContent);

    // Retrieve the controller
    HandleT<Controller> lController;
    HandleT<MapTree> lControllerNode = lRoot->getChild("controller");

    if (lControllerNode->hasKey("mlp.controller")) {
        HandleT<MapTree> lNode = lControllerNode->getChild("mlp.controller");
        size_t lNbHiddenNeurons;
        lNode->get("nb.hidden.neurons", lNbHiddenNeurons);
        lController = new MLPController(lNbHiddenNeurons);
    } else if (lControllerNode->hasKey("esn.controller")) {
        HandleT<MapTree> lNode = lControllerNode->getChild("esn.controller");
        double lDensity, lSpectralRadius;
        lNode->get("density", lDensity);
        lNode->get("spectral.radius", lSpectralRadius);
    }
    // Retrieve the monitor
    HandleT<Monitor> lMonitor;
    HandleT<MapTree> lMonitorNode = lRoot->getChild("monitor");

    if (lMonitorNode->hasKey("fixed.monitor")) {
        HandleT<MapTree> lNode = lMonitorNode->getChild("fixed.monitor");
        size_t lNbSteps;
        lNode->get("max.nb.steps", lNbSteps);
        lMonitor = new FixedMonitor(lNbSteps);
    } else if (lMonitorNode->hasKey("energy.monitor")) {
        HandleT<MapTree> lNode = lMonitorNode->getChild("energy.monitor");
        size_t lWindowSize;
        lNode->get("window.size", lWindowSize);
        lMonitor = new EnergyMonitor(lWindowSize);
    }

    //    cout << "controller + monitor created \n ";

    // Retrieve the embryo constructor parameters
    bool lDiffuseChemicals, lbDifferentStatesInitialization;
    size_t lWidth, lHeight, lNbStates, lNbChemicals;
    std::string lPathTargetPic;
    std::string lPath;

    try {
        lRoot->get("width", lWidth);
        lRoot->get("height", lHeight);
        lRoot->get("nb.states", lNbStates);
        lRoot->get("nb.chemicals", lNbChemicals);
        lRoot->get("diffuse.chemicals", lDiffuseChemicals);
        lRoot->get("target.pic", lPathTargetPic);
        lRoot->get("path", lPath);
        lRoot->get("different.init.values", lbDifferentStatesInitialization);
        lRoot->get("gui.output", oGuiOuput);
    } catch (embryo::Exception& inException) {
        cerr
                << "Error while loading parameters from map tree '"
                << lWidth
                << lHeight
                << lNbStates
                << lNbChemicals
                << lDiffuseChemicals
                << lPathTargetPic
                << lbDifferentStatesInitialization
                << oGuiOuput
                << "' :\n  "
                << inException.getMessage()
                << endl;
    }
    cout << "# embryo config info: " << endl;
    cout << "# lWidth " << lWidth << endl;
    cout << "# lHeight " << lHeight << endl;
    cout << "# lNbStates " << lNbStates << endl;
    cout << "# lNbChemicals " << lNbChemicals << endl;
    cout << "# lDiffuseChemicals " << lDiffuseChemicals << endl;
    cout << "# lPathTargetPic name " << lPathTargetPic << endl;
    cout << "# lPath " << lPath << endl;
    cout << "# lbDifferentStatesInitialization " << lbDifferentStatesInitialization << endl;
    cout << "# oGuiOuput " << oGuiOuput << endl;


    // Load the target pictures

    const char DELIM = '|';

    int counter = 1;
    vector<string> names;
    int i = 0;
    do {
        if (lPathTargetPic[i] == DELIM) {
            names.push_back(lPathTargetPic.substr(0, i));
            cout << "# " << counter << "-th pic to read: " << lPathTargetPic.substr(0, i) << endl;
            if (i == lPathTargetPic.length())
                lPathTargetPic.erase(0, i);
            else
                lPathTargetPic.erase(0, i + 1);

            i = 0;
            counter++;
        }
        i++;
    } while (!lPathTargetPic.empty());


    Picture* lTargetPic = new Picture[names.size()];

    string targetPicPath;
    for (size_t i = 0; i < names.size(); i++) {
        targetPicPath = lPath + names[i] + ".pgm";
        try {
            ifstream lFile;
            safeOpen(lFile, targetPicPath);
            loadPGM(lFile, lTargetPic[i]);
            lTargetPic[i].setName(names[i]);

            lTargetPic[i].setBorder(i);

            lFile.close();
        } catch (embryo::Exception& inException) {
            cerr
                    << "Error while loading '"
                    << lPathTargetPic
                    << "' :\n  "
                    << inException.getMessage()
                    << endl;
        }
    }


    // Build the embryo
    Embryo* lResult;
    try {
        lResult = new Embryo(lWidth, lHeight,
                lNbStates, lNbChemicals,
                &lTargetPic[0],
                names.size(),
                lMonitor,
                lController,
                lDiffuseChemicals,
                lbDifferentStatesInitialization);
    } catch (embryo::Exception& inException) {
        cerr
                << "\n Error while building embryo \n"
                << inException.getMessage()
                << endl;
    }


    if (lRoot->hasKey("nb.steps.max")) {
        size_t lNbStepsMax;
        lRoot->get("nb.steps.max", lNbStepsMax);
        lResult->nbStepsMax() = lNbStepsMax;
        cout << "# nb steps max: ";
        cout << lNbStepsMax;
    }

    // Job done
    return lResult;
}


// --- EmbryoFactory ----------------------------------------------------------

EmbryoFactory::EmbryoFactory(const ObjectFactoryDealer & inDealer) : ObjectFactory("embryo", inDealer) {
}

EmbryoFactory::~EmbryoFactory() {
}

Object::Handle
EmbryoFactory::produce(MapTree::Handle inMapTree) const {
    // Retrieve the controller instance
    Controller::Handle lController;
    {
        pair<std::string, MapTree::Handle> lNode =
                inMapTree->getChild("controller")->getChild();
        lController = dealer().deserializeT<Controller > (lNode.first, lNode.second);
    }


    cout << "Embryo factory produce:" << endl;


    // Retrieve the monitor instance
    Monitor::Handle lMonitor;
    {
        pair<std::string, MapTree::Handle> lNode =
                inMapTree->getChild("monitor")->getChild();
        lMonitor = dealer().deserializeT<Monitor > (lNode.first, lNode.second);
    }

    bool lDiffuseChemicals;
    size_t lWidth, lHeight, lNbStates, lNbChemicals;
    bool lbDifferentStatesInitialization;
    std::string lPathTargetPic;
    std::string lPath;

    try {
        inMapTree->get("width", lWidth);
        inMapTree->get("height", lHeight);
        inMapTree->get("nb.states", lNbStates);
        inMapTree->get("nb.chemicals", lNbChemicals);
        inMapTree->get("diffuse.chemicals", lDiffuseChemicals);
        inMapTree->get("target.pic", lPathTargetPic);
        inMapTree->get("path", lPath);
        inMapTree->get("different.init.values", lbDifferentStatesInitialization);
    } catch (embryo::Exception& inException) {
        cerr
                << "Error while loading parameters from map tree '"
                << lWidth
                << lHeight
                << lNbStates
                << lNbChemicals
                << lDiffuseChemicals
                << lPathTargetPic
                << lbDifferentStatesInitialization
                << "' :\n  "
                << inException.getMessage()
                << endl;
    }
    cout << "embryo config info: " << endl;
    cout << "lWidth " << lWidth << endl;
    cout << "lHeight " << lHeight << endl;
    cout << "lNbStates " << lNbStates << endl;
    cout << "lNbChemicals " << lNbChemicals << endl;
    cout << "lDiffuseChemicals " << lDiffuseChemicals << endl;
    cout << "lPathTargetPic name " << lPathTargetPic << endl;
    cout << "lPath " << lPath << endl;
    cout << "lbDifferentStatesInitialization " << lbDifferentStatesInitialization << endl;



    // Load the target pictures
    cout << "pics to read: " << lPath << lPathTargetPic << ".pgm" << endl;

    const char DELIM = '|';
    vector<string> names;
    int i = 0;
    do {
        if (lPathTargetPic[i] == DELIM) {
            names.push_back(lPathTargetPic.substr(0, i));
            cout << i << "-th pic to read: " << lPathTargetPic.substr(0, i) << endl;
            if (i == lPathTargetPic.length())
                lPathTargetPic.erase(0, i);
            else
                lPathTargetPic.erase(0, i + 1);
            i = 0;
        }
        i++;
    } while (!lPathTargetPic.empty());

    cout << "path parsed" << endl;

    cout << " names.size(): " << names.size() << endl;
    Picture* lTargetPic = new Picture[names.size()];
    cout << " names.size(): " << sizeof (lTargetPic) << endl;

    string targetPicPath;
    for (size_t i = 0; i < names.size(); i++) {
        targetPicPath = lPath + names[i] + ".pgm";
        cout << " i: " << i << " name: " << names[i] << targetPicPath << endl;
        try {
            ifstream lFile;
            safeOpen(lFile, targetPicPath);
            loadPGM(lFile, lTargetPic[i]);
            lTargetPic[i].setName(names[i]);
            lTargetPic[i].setBorder(i);
            lFile.close();
        } catch (embryo::Exception& inException) {
            cerr
                    << "Error while loading '"
                    << lPathTargetPic
                    << "' :\n  "
                    << inException.getMessage()
                    << endl;
        }
    }


    // Build the embryo
    Embryo* lResult;
    try {
        lResult = new Embryo(lWidth, lHeight,
                lNbStates, lNbChemicals,
                &lTargetPic[0],
                names.size(),
                lMonitor,
                lController,
                lDiffuseChemicals,
                lbDifferentStatesInitialization);
    } catch (embryo::Exception& inException) {
        cerr
                << "\n Error while building embryo \n"
                << inException.getMessage()
                << endl;
    }

    if (inMapTree->hasKey("nb.steps.max")) {
        size_t lNbStepsMax;
        inMapTree->get("nb.steps.max", lNbStepsMax);
        lResult->nbStepsMax() = lNbStepsMax;
        cout << "nb steps max: ";
        cout << lNbStepsMax;
    }

    // Job done
    return lResult;
}
