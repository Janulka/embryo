/*
 *  libembryo
 *  Copyright (C) 2010 by Jana Hlavacikova
 *
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

#ifndef EMBRYO_EMBRYO_H
#define EMBRYO_EMBRYO_H

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <Picture.h>
#include <Monitor.h>
#include <Controller.h>
#include <Randomizer.h>
#include <EmbryoListener.h>
#include <PGM.h>
#include <Cell.h>

using namespace std;

namespace embryo {

    class Embryo : public Object {
    private:
        bool mDiffuseChemicals;
        size_t mWidth, mHeight, mNbStates, mNbChemicals, mNbCells, /*mNbChemicalsPerRow,*/ mNbStepsMax, mPicsNo;//, mMaxNeigbors;
        const Picture * mTargetPic;
        Picture mCandidatePic;

        double* mMaxDist;

        std::vector<EmbryoListener*> mListeners;

        HandleT<Monitor> mMonitor;
        HandleT<Controller> mController;

        double* mBorderVector;

        CellContainer mCellContainer;

        int mCellNumber;
        size_t mChemicalSize;
        size_t mStateSize;

        bool mCellChange;
        bool mbDifferentStatesInitialization;

        size_t mStructureSteps;
    public:
        typedef HandleT<Embryo> Handle;

        Embryo(size_t inWidth, size_t inHeight,
                size_t inNbStates,
                size_t inNbChemicals,
                const Picture * inTargetPic,
                size_t inSize,
                HandleT<Monitor> inMonitor,
                HandleT<Controller> inController,
                bool inDiffuseChemicals,
                bool ibDifferentStatesInitialization);

        virtual ~Embryo();

        inline size_t cellsNumber() const {
            return mNbCells;
        }
        
        inline size_t& structureSteps() {
            return mStructureSteps;
        }

        inline size_t structureSteps() const {
            return mStructureSteps;
        }

        inline size_t getBorderValue(string inName) const {
            size_t border = (size_t) 0;
            for (size_t i = 0; i < mPicsNo; i++) {
                border = mTargetPic[i].getBorder(inName);
                if (border > 0)
                    return border;
            }
            return border;
        }

        inline size_t width() const {
            return mWidth;
        }

        inline size_t height() const {
            return mHeight;
        }

        inline size_t nbStepsMax() const {
            return mNbStepsMax;
        };

        inline size_t& nbStepsMax() {
            return mNbStepsMax;
        };

        inline size_t nbStates() const {
            return mNbStates;
        }

        inline size_t nbChemicals() const {
            return mNbChemicals;
        }

        inline const Controller& controller() const {
            return *mController;
        }

        inline Controller& controller() {
            return *mController;
        }

        inline Picture& getCandidatePic() {
            return mCandidatePic;
        }

        inline Picture getTargetPic(size_t iIndex) {
            return mTargetPic[iIndex];
        }

        inline const Picture getTargetPicC(size_t iIndex) const {
            return mTargetPic[iIndex];
        }

        inline Picture getTargetPicC(std::string iName) const {
            for (size_t t = 0; t < mPicsNo; t++)
                if (mTargetPic[t].getName() == iName)
                    return mTargetPic[t];
        }
        
        inline size_t getTargetIndex(std::string iName) const {
            for (size_t t = 0; t < mPicsNo; t++)
                if (mTargetPic[t].getName() == iName)
                    return t;
        }

        inline const double * getTargetPicPixelsC(size_t iIndex) const {
            return mTargetPic[iIndex].pixels();
        }


        inline double getMaxDistance(size_t iIndex) {
            return mMaxDist[iIndex];
        }

        inline size_t getPicsNumber() {
            return mPicsNo;
        }

        inline bool isCellChange() const {
            return mCellChange;
        }


        void addListener(EmbryoListener* inListener);

        void removeListener(EmbryoListener* inListener);

        void init(size_t initState);

        void resetMonitor(size_t inBorder);

        // Allow to modify the width and size of the embryo, init should be called after
        void reshape(size_t inWidth, size_t inHeight);

        //TODO: move to Monitor
        bool checkConstraints(Cell * iCell);

        void updateStructure();
        void updateStructureSingle(Cell *& iCell);

        // Update the cells states
        bool update(bool ibColor);

        // Scramble the cells states
        void scramble(Randomizer& inRandomizer, double inStdDev);

        // Give the energy level of the embryo
        double energy() const;

        //double getColorOnPixel(const size_t x, const size_t y, const Picture& pic);
        double getColorOnPixel(const size_t x, const size_t y, Picture& pic);

        // Write the color of the cells to a picture
        void getColours(Picture& inPicture) const;

        // Write a given state of the cells to a picture
        void getStates(Picture& inPicture, size_t inIndex) const;

        // Write a given chemical of the cells to a picture
        void getChemicals(Picture& inPicture, size_t inIndex) const;

        // Load an embryo from a file. init should be called after
        static Embryo::Handle load(std::istream& inStream, uint32_t inSeed, const std::string& pathContent, bool & oGuiOuput);

        double setupController(const double* inVector);

        double getSimilarity(size_t iIndex, bool ibColor);

        double evaluate(const double* inVector, size_t& outNbSteps, double& outSimilarity);

        void removeMeGetState(Cell *& iCellP, double *& iStateVector);

        void changeBorderStatus(Cell *& iNewCell, Cell * iC1, Cell * iC2, Cell * iC3);

        bool findCell(int iIndex, Cell *& oCell);

        void loadCells(std::istream& inStream);
        void saveCells(std::ostream& inStream);
        void initCells(size_t initState);
	void printout();
    private:
        void setup();

        void diffuseChemicals();

        bool changeNeighborhood(Cell *& iNewCell, Direction & iDir1, Direction & iDir2);

        void divide(Cell *& iCell);
    }
    ; // class Embryo

    class EmbryoFactory : public ObjectFactory {
    public:
        EmbryoFactory(const ObjectFactoryDealer& inDealer);

        virtual ~EmbryoFactory();

        virtual Object::Handle produce(MapTree::Handle inMapTree) const;
    };
    // class EmbryoFactory
} // namespace embryo



#endif /* EMBRYO_EMBRYO_H */
