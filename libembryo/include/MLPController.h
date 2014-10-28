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

#ifndef EMBRYO_MLP_CONTROLLER_H
#define EMBRYO_MLP_CONTROLLER_H

#include <Controller.h>
#include <ObjectFactory.h>



namespace embryo {

    class MLPController : public Controller {
    protected:
        size_t mNbChemicals, mNbStates, mNbHiddenNeurons;
        size_t mNbColourWeights, mNbStructureWeights, mNbMLPWeights;
        double* mMLPWeights;
        double* mColourWeights;

        mutable double* mHidden;

    public:
        MLPController(size_t inNbHiddenNeurons);

        virtual ~MLPController();



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
    }
    ; // class MLPController

    class MLPControllerFactory : public ObjectFactory {
    public:
        MLPControllerFactory(const ObjectFactoryDealer& inDealer);

        virtual ~MLPControllerFactory();

        virtual Object::Handle produce(MapTree::Handle inMapTree) const;
    };
    // class MLPControllerFactory
} // namespace embryo



#endif /* EMBRYO_MLP_CONTROLLER_H */
