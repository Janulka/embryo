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

#ifndef EMBRYO_ESN_CONTROLLER_H
#define EMBRYO_ESN_CONTROLLER_H

#include <vector>
#include <stdint.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include <Controller.h>
#include <Randomizer.h>
#include <ObjectFactory.h>



namespace embryo {

    class ESNController : public Controller {
    protected:
        uint32_t mSeed;
        double mDensity, mSpectralRadius;
        size_t mNbChemicals, mNbStates;
        double* mESNWeights;
        double* mColourWeights;

        size_t* mDriveTable;
        double* mReservoirWeights;

    public:
        ESNController(double inDensity,
                double inSpectralRadius,
                uint32_t inSeed);

        virtual ~ESNController();



        virtual bool update(const double* &inOldState,
                double* const* &inChemicals,
                double* &outNewState,
                double* &outNewChemicals,
                bool * &ibIsNeighbor)
        const;

        virtual double colour(const double* inState, const double* inChemicals) const;
        virtual double structure(const double* inState, const double* inChemicals) const;

    protected:
        virtual size_t setup(size_t inNbStates, size_t inNbChemicals);

    private:
        void setupReservoir(gsl_matrix* outMatrix,
                std::vector< std::pair<size_t, size_t> >& outCoords,
                Randomizer& inRnd);

        static double spectralRadius(const gsl_matrix* inMatrix, size_t inSize);
    }
    ; // class ESNController

    class ESNControllerFactory : public ObjectFactory {
    public:
        ESNControllerFactory(const ObjectFactoryDealer& inDealer);

        virtual ~ESNControllerFactory();

        virtual Object::Handle produce(MapTree::Handle inMapTree) const;
    };
    // class ESNControllerFactory
} // namespace embryo



#endif /* EMBRYO_ESN_CONTROLLER_H */
