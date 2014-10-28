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

#ifndef EMBRYO_CONTROLLER_H
#define EMBRYO_CONTROLLER_H

#include <sys/types.h>
#include <Object.h>



namespace embryo {

    /*
     * Defines an abstract controller for a cell.
     */

    class Controller : public Object {
    private:
        bool mWasInitialized;
        double* mParameters;
        size_t mNbParameters;

    public:
        typedef HandleT<Controller> Handle;



        Controller();

        virtual ~Controller();

        inline size_t nbParameters() const {
            return mNbParameters;
        }

        inline double* parameters() {
            return mParameters;
        }

        inline const double* parameters() const {
            return mParameters;
        }



        /*
         * Setup a controller, called before any call to the update() and colour()
         * methods occurs.
         */
        void init(size_t inNbStates, size_t inNbChemicals);

        /*
         * Compute the state update of a cell
         *
         * inOldState : the internal states of the cell
         * inChemicals : the chemicals of a cell neighbours (indexes 0,1,2,3) and it
         *               own chemicals (index 4)
         * outNewState : the updated internal states of the cell
         * outNexChemicals : the updated chemicals of the cell
         *
         */
        virtual bool update(const double* &inOldState,
                double* const* &inChemicals,
                double* &outNewState,
                double* &outNewChemicals,
                bool * &ibIsNeighborg)
        const = 0;

        /*
         * Compute the color of a cell
         *
         * inState : the internal states of the cell
         * inChemicals : the chemicals of the cell
         *
         * Returns : a value in the [0,1] range.
         */
        virtual double colour(const double* inState, const double* inChemicals) const = 0;
        virtual double structure(const double* inState, const double* inChemicals) const = 0;

    protected:
        /*
         * This method is called at the controller init and returns it number of
         * parameters.
         */
        virtual size_t setup(size_t inNbStates, size_t inNbChemicals) = 0;
    }
    ; // class Controller
} // namespace embryo



#endif /* EMBRYO_CONTROLLER_H */
