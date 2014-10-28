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

#include "Embryo.h"
#include "ArrayOps.h"
#include "EnergyMonitor.h"

using namespace embryo;



// --- EnergyMonitor ----------------------------------------------------------

EnergyMonitor::EnergyMonitor(size_t inWindowSize) : mWindowSize(inWindowSize) {
    mWindow = new double[mWindowSize];
    mWindowCell = new bool[mWindowSize];
}

EnergyMonitor::~EnergyMonitor() {
    delete[] mWindow;
    delete[] mWindowCell;
}

void
EnergyMonitor::init(const Embryo& inEmbryo) {
    mIndex = 0;
    mNbStepsConsumed = 0;
    mEnergyStdDev = 1.0;
    mCellChange = true;
}

bool
EnergyMonitor::hasNext(bool ibColor) {
    //  return ((mEnergyStdDev > 1e-10) || mCellChange);
    if (ibColor)
        return (mEnergyStdDev > 1e-10);
    else
        return mCellChange;
}

void
EnergyMonitor::next(const Embryo& inEmbryo) {
    // Update the embryo
    mNbStepsConsumed += 1;

    // Fill the window
    double lEnergy = inEmbryo.energy();
    //  printf("embryo energy: %e\n", lEnergy);
    bool lCellChange = inEmbryo.isCellChange();
    mWindow[mIndex] = lEnergy;
    mWindowCell[mIndex++] = lCellChange;

    if (mIndex >= mWindowSize)
        mIndex = 0;

    // If enougth values, compute energy standard deviation (robust & slow)
    if (mNbStepsConsumed >= mWindowSize) {
        mEnergyStdDev = arrayd::stdDev(mWindow, mWindowSize);
        mCellChange = arrayb::isAnyTrue(mWindowCell, mWindowSize);
    }
}



// --- EnergyMonitorFactory ---------------------------------------------------

EnergyMonitorFactory::EnergyMonitorFactory(const ObjectFactoryDealer& inDealer) : ObjectFactory("energy.monitor", inDealer) {
}

EnergyMonitorFactory::~EnergyMonitorFactory() {
}

Object::Handle
EnergyMonitorFactory::produce(MapTree::Handle inMapTree) const {
    size_t lWindowSize;
    inMapTree->get("window.size", lWindowSize);

    return new EnergyMonitor(lWindowSize);
}
