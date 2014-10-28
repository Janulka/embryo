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

#include <cmath>
#include <algorithm>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_complex_math.h>
#include "ESNController.h"
#include "ObjectFactoryDealer.h"

using namespace std;
using namespace embryo;



// --- ESNController ----------------------------------------------------------

ESNController::ESNController(double inDensity, double inSpectralRadius,
        uint32_t inSeed) : mSeed(inSeed), mDensity(inDensity), mSpectralRadius(inSpectralRadius), mDriveTable(0), mReservoirWeights(0) {
}

ESNController::~ESNController() {
    if (!mReservoirWeights) {
        delete[] mReservoirWeights;
        delete[] mDriveTable;
    }
}

double
ESNController::spectralRadius(const gsl_matrix* inMatrix, size_t inSize) {
    // Need a tmp matrix because the eigen value routine destroys it argument
    gsl_matrix* lTmp = gsl_matrix_alloc(inSize, inSize);
    gsl_matrix_memcpy(lTmp, inMatrix);

    // Compute the eigen values
    gsl_vector_complex* lEigenValues = gsl_vector_complex_alloc(inSize);
    gsl_eigen_nonsymm_workspace* lEigenSys = gsl_eigen_nonsymm_alloc(inSize);

    gsl_eigen_nonsymm(lTmp, lEigenValues, lEigenSys);

    // Search the one with the greatest magnitude
    double lResult = gsl_complex_abs(gsl_vector_complex_get(lEigenValues, 0));
    for (size_t i = 1; i < inSize; ++i) {
        double lMagnitude =
                gsl_complex_abs(gsl_vector_complex_get(lEigenValues, i));

        if (lMagnitude > lResult)
            lResult = lMagnitude;
    }

    // Job done
    gsl_eigen_nonsymm_free(lEigenSys);
    gsl_vector_complex_free(lEigenValues);
    gsl_matrix_free(lTmp);

    return lResult;
}

void
ESNController::setupReservoir(gsl_matrix* outMatrix,
        vector< pair<size_t, size_t> >& outCoords,
        Randomizer& inRnd) {
    // Generate random non-zero coeffs positions
    outCoords.clear();
    size_t lNbWeights = mNbStates * mNbStates;
    outCoords.reserve(lNbWeights);
    for (size_t i = 0; i < mNbStates; ++i)
        for (size_t j = 0; j < mNbStates; ++j)
            outCoords.push_back(make_pair(i, j));

    random_shuffle(outCoords.begin(), outCoords.end(), inRnd);
    size_t lNbNonZeroWeights =
            (size_t)
            rint(max(min(mDensity, 1.0), 0.0) * double(lNbWeights));
    outCoords.resize(lNbNonZeroWeights);

    // Setup the non-zero coeffs positions
    gsl_matrix_set_zero(outMatrix);
    vector< pair<size_t, size_t> >::const_iterator it = outCoords.begin();
    for (; it != outCoords.end(); ++it) {
        double lValue = 1.0;
        if (inRnd.real() > 0.5)
            lValue = -lValue;
        gsl_matrix_set(outMatrix, (*it).first, (*it).second, lValue);
    }

    // Scale the reservoir
    double lScale = mSpectralRadius / spectralRadius(outMatrix, mNbStates);
    gsl_matrix_scale(outMatrix, lScale);
}

size_t
ESNController::setup(size_t inNbStates, size_t inNbChemicals) {
    mNbChemicals = inNbChemicals;
    mNbStates = inNbStates;

    // Generate a reservoir matrix
    Randomizer lRnd;
    lRnd.init(mSeed);

    gsl_matrix* lReservoirMatrix = gsl_matrix_alloc(mNbStates, mNbStates);
    vector< pair<size_t, size_t> > lCoords;
    setupReservoir(lReservoirMatrix, lCoords, lRnd);

    // Generate the reservoir traversal datas
    if (!mReservoirWeights) {
        delete[] mReservoirWeights;
        delete[] mDriveTable;
    }

    mReservoirWeights = new double[lCoords.size()];
    mDriveTable = new size_t[mNbStates + lCoords.size()];

    vector< vector<size_t> > lTable(mNbStates);

    {
        double* lOffset = mReservoirWeights;
        vector< pair<size_t, size_t> >::const_iterator it = lCoords.begin();
        for (; it != lCoords.end(); ++it, ++lOffset) {
            lTable[(*it).first].push_back((*it).second);
            *lOffset = gsl_matrix_get(lReservoirMatrix, (*it).first, (*it).second);
        }
    }

    {
        size_t* lOffset = mDriveTable;
        for (size_t i = 0; i < mNbStates; ++i) {
            (*lOffset++) = lTable[i].size();
            for (size_t j = 0; j < lTable[i].size(); ++j)
                (*lOffset++) = lTable[i][j];
        }
    }

    // Compute the number of parameters
    size_t lNbColourWeights = mNbStates + 1;
    size_t lNbESNWeights =
            (4 * mNbChemicals) * mNbStates + (mNbStates + 1) * mNbChemicals;

    // Job done
    gsl_matrix_free(lReservoirMatrix);
    return lNbColourWeights + lNbESNWeights;
}

bool
ESNController::update(const double* &inOldState,
        double* const * &inChemicals,
        double* &outNewState,
        double* &outNewChemicals,
        bool * &ibIsNeighbor)
const {
    /*
     * States update
     */

    const double* lWeight = mReservoirWeights;
    const size_t* lDrive = mDriveTable;

    for (size_t i = 0; i < mNbStates; ++i) {
        // Neuron bias
        double lSum = (*lWeight++);

        // Input states sum
        size_t lNbNeurons = (*lDrive++);
        for (size_t j = 0; j < lNbNeurons; ++j)
            lSum += (*lWeight++) * inOldState[(*lDrive++)];

        // Input chemicals sum
        for (size_t j = 0; j < 4; ++j)
            if (inChemicals[j] != 0)
                for (size_t k = 0; k < mNbChemicals; ++k)
                    lSum += (*lWeight++) * inChemicals[j][k];

        outNewState[i] = tanh(lSum);
    }

    /*
     * Chemicals update
     */

    lWeight = parameters() + (mNbStates + 1);
    for (size_t i = 0; i < mNbChemicals; ++i) {
        double lSum = (*lWeight++);
        for (size_t j = 0; j < mNbStates; ++j)
            lSum += (*lWeight++) * outNewState[j];

        outNewChemicals[i] = tanh(inChemicals[4][i] + 0.1 * lSum);
    }

    return true;
}

double
ESNController::colour(const double* inState, const double* inChemicals) const {
    const double* lOffset = inState;
    const double* lWeight = parameters();

    double lSum = (*lWeight++);
    for (size_t i = mNbStates; i != 0; ++lWeight, ++lOffset, --i)
        lSum += (*lWeight) * (*lOffset);

    return 0.5 * (tanh(lSum) + 1.0);
}


double ESNController::structure(const double* inState, const double* inChemicals) const {
 ;
}

// --- ESNControllerFactory ---------------------------------------------------

ESNControllerFactory::ESNControllerFactory(const ObjectFactoryDealer& inDealer) : ObjectFactory("esn.controller", inDealer) {
}

ESNControllerFactory::~ESNControllerFactory() {
}

Object::Handle
ESNControllerFactory::produce(MapTree::Handle inMapTree) const {
    double lDensity, lSpectralRadius;
    inMapTree->get("density", lDensity);
    inMapTree->get("spectral.radius", lSpectralRadius);

    return (new ESNController(lDensity, lSpectralRadius, dealer().randomSeed()));
}
