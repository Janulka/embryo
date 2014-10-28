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

#include <cmath>
#include "MLPController.h"
#include <stdio.h>

using namespace embryo;

// --- MLPController ----------------------------------------------------------

MLPController::MLPController(size_t inNbHiddenNeurons) : mNbChemicals(0), mNbStates(0), mNbHiddenNeurons(inNbHiddenNeurons) {
    mHidden = new double[mNbHiddenNeurons];
}

MLPController::~MLPController() {
    delete[] mHidden;
}

size_t
MLPController::setup(size_t inNbStates, size_t inNbChemicals) {
    mNbChemicals = inNbChemicals;
    mNbStates = inNbStates;
    
    mNbColourWeights = mNbStates + mNbChemicals + 1;
    mNbStructureWeights = mNbStates + mNbChemicals + 1;

    mNbMLPWeights =
            (4 * mNbChemicals + mNbStates + 1) * mNbHiddenNeurons +
            (mNbHiddenNeurons + 1) * (mNbChemicals + mNbStates);

    return (mNbColourWeights + mNbStructureWeights + mNbMLPWeights);
}


// Update the current cell state
bool
MLPController::update(const double* &inOldState,
        double* const * &inChemicals,
        double* &outNewState,
        double* &outNewChemicals,
        bool * &ibIsNeighbor
        ) const {
    const double* lWeight = parameters() + mNbColourWeights + mNbStructureWeights;

    // Input to hidden layer
    for (size_t i = 0; i < mNbHiddenNeurons; ++i) {
        // Neuron bias
        double lSum = (*lWeight++);

        // Input states sum
        for (size_t j = 0; j < mNbStates; ++j) {
            lSum += (*lWeight++) * inOldState[j];
        }

        // Input chemicals sum
        for (size_t j = 0; j < 4; ++j) {
            if (ibIsNeighbor[j])
                for (size_t k = 0; k < (mNbChemicals); ++k) {
                    if (inChemicals[j][k] > 0) {
                        lSum += (*lWeight) * inChemicals[j][k];
                    }
                }
            lWeight++;
        }
        mHidden[i] = tanh(lSum);
    }

    //    double lDiffSum[4];

    // Hidden layer to new states
    for (size_t i = 0; i < mNbStates; ++i) {
        double lSum = (*lWeight++);
        for (size_t j = 0; j < mNbHiddenNeurons; ++j) {
            lSum += (*lWeight++) * mHidden[j];
        }
        outNewState[i] = tanh(lSum);
    }

    // Hidden layer to new chemicals
    for (size_t i = 0; i < (mNbChemicals); ++i) {
        double lSum = (*lWeight++);
        for (size_t j = 0; j < mNbHiddenNeurons; ++j) {
            lSum += (*lWeight++) * mHidden[j];
        }
        outNewChemicals[i] = tanh(inChemicals[4][i] + lSum);
    }
    return false;
}

double
MLPController::colour(const double* inState, const double* inChemicals) const {

    const double* lWeight = parameters() + mNbStructureWeights;

    // Add offset
    double lSum = (*lWeight++);

    // Add states weighted sum
    {
        const double* lOffset = inState;
        for (size_t i = mNbStates; i != 0; ++lOffset, --i)
            lSum += (*lWeight++) * (*lOffset);
    }

    //  Add chemicals weighted sum
    {
        const double* lOffset = inChemicals;
        for (size_t i = mNbChemicals; i != 0; ++lOffset, --i)
            lSum += (*lWeight++) * (*lOffset);
    }

    // Job done
    return 0.5 * (tanh(lSum) + 1.0);
}

double
MLPController::structure(const double* inState, const double* inChemicals) const {

    const double* lWeight = parameters();

    // Add offset
    double lSum = (*lWeight++);

    // Add states weighted sum
    {
        const double* lOffset = inState;
        for (size_t i = mNbStates; i != 0; ++lOffset, --i)
            lSum += (*lWeight++) * (*lOffset);
    }

    //  Add chemicals weighted sum
    {
        const double* lOffset = inChemicals;
        for (size_t i = mNbChemicals; i != 0; ++lOffset, --i)
            lSum += (*lWeight++) * (*lOffset);
    }

    // Job done
    return tanh(lSum);
}

// --- MLPControllerFactory ---------------------------------------------------

MLPControllerFactory::MLPControllerFactory(const ObjectFactoryDealer& inDealer) : ObjectFactory("mlp.controller", inDealer) {
}

MLPControllerFactory::~MLPControllerFactory() {
}

Object::Handle
MLPControllerFactory::produce(MapTree::Handle inMapTree) const {
    size_t lNbHiddenNeurons;
    inMapTree->get("nb.hidden.neurons", lNbHiddenNeurons);

    return new MLPController(lNbHiddenNeurons);
}
