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

#ifndef EMBRYO_ENERGY_MONITOR_H
#define EMBRYO_ENERGY_MONITOR_H

#include <Monitor.h>
#include <ObjectFactory.h>



namespace embryo {
  class EnergyMonitor : public Monitor {
  private:
    size_t mWindowSize, mNbStepsConsumed, mIndex;
    double mEnergyStdDev;
    double* mWindow;
    bool* mWindowCell;
    bool mCellChange;

  public:
    EnergyMonitor(size_t inWindowSize);

    virtual ~EnergyMonitor();

    virtual void init(const Embryo& inEmbryo);

    virtual bool hasNext(bool ibColor);

    virtual void next(const Embryo& inEmbryo);
  }
  ; // class EnergyMonitor



  class EnergyMonitorFactory : public ObjectFactory {
  public:
    EnergyMonitorFactory(const ObjectFactoryDealer& inDealer);

    virtual ~EnergyMonitorFactory();

    virtual Object::Handle produce(MapTree::Handle inMapTree) const;
  };
  // class EnergyMonitorFactory
} // namespace embryo



#endif /* EMBRYO_ENERGY_MONITOR_H */
