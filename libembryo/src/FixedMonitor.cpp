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

#include "Embryo.h"
#include "FixedMonitor.h"

using namespace embryo;



// --- FixedMonitor -----------------------------------------------------------

FixedMonitor::FixedMonitor(size_t inNbSteps) : mNbSteps(inNbSteps) { }



FixedMonitor::~FixedMonitor() { }



void
FixedMonitor::init(const Embryo& inEmbryo) {
  mNbStepsConsumed = 0;
}



bool
FixedMonitor::hasNext(bool ibColor) {
  return mNbStepsConsumed < mNbSteps;
}



void
FixedMonitor::next(const Embryo& inEmbryo) {
  mNbStepsConsumed += 1;
}



// --- FixedMonitorFactory ----------------------------------------------------

FixedMonitorFactory::FixedMonitorFactory(const ObjectFactoryDealer& inDealer) : ObjectFactory("fixed.monitor", inDealer) { }



FixedMonitorFactory::~FixedMonitorFactory() { }



Object::Handle
FixedMonitorFactory::produce(MapTree::Handle inMapTree) const {
  size_t lNbSteps;
  inMapTree->get("max.nb.steps", lNbSteps);

  return new FixedMonitor(lNbSteps);
}
