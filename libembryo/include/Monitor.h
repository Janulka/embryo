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

#ifndef EMBRYO_MONITOR_H
#define EMBRYO_MONITOR_H

#include <Object.h>



namespace embryo {
  class Embryo;

  class Monitor : public Object {
  public:
    typedef HandleT<Monitor> Handle;



    virtual ~Monitor();

    virtual void init(const Embryo& inEmbryo) = 0;

    virtual bool hasNext(bool ibColor) = 0;

    virtual void next(const Embryo& inEmbryo) = 0;
  }
  ; // class Monitor
} // namespace embryo



#endif /* EMBRYO_MONITOR_H */
