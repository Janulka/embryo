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

#ifndef EMBRYO_LISTENER_H
#define EMBRYO_LISTENER_H



namespace embryo {
  class Embryo;

  /*
   * Interface that defines the capacity to be noticed of some events occured
   * to an Embryo instance :
   *      - an execution of the Embryo::init method
   *      - an execution of the Embryo::update method
   */
 
  class EmbryoListener {
  public:
    virtual ~EmbryoListener();

    virtual void onInit(const Embryo& inEmbryo) = 0;

    virtual void onUpdate(const Embryo& inEmbryo) = 0;
  }
  ; // class EmbryoListener
} // namespace embryo



#endif /* EMBRYO_LISTENER_H */
