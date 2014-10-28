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

#ifndef EMBRYO_FIXED_MONITOR_H
#define EMBRYO_FIXED_MONITOR_H

#include <Monitor.h>
#include <ObjectFactory.h>



namespace embryo {
  class FixedMonitor : public Monitor {
  private:
    size_t mNbSteps, mNbStepsConsumed;

  public:
    FixedMonitor(size_t inNbSteps);

    virtual ~FixedMonitor();

    virtual void init(const Embryo& inEmbryo);

    virtual bool hasNext(bool ibColor);

    virtual void next(const Embryo& inEmbryo);
  }
  ; // class FixedMonitor



  class FixedMonitorFactory : public ObjectFactory {
  public:
    FixedMonitorFactory(const ObjectFactoryDealer& inDealer);

    virtual ~FixedMonitorFactory();

    virtual Object::Handle produce(MapTree::Handle inMapTree) const;
  };
  // class FixedMonitorFactory
} // namespace embryo



#endif /* EMBRYO_FIXED_MONITOR_H */
