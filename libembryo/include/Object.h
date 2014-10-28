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

#ifndef EMBRYO_OBJECT_H
#define EMBRYO_OBJECT_H

#include <HandleT.h>



namespace embryo {
  class Object {
  private:
    unsigned int mRefCount;

  public:
    typedef HandleT<Object> Handle;



    Object() : mRefCount(0) { }

    Object(const Object& inObject) : mRefCount(0) { }

    virtual ~Object();

    inline void inc_ref_count() {
      mRefCount++;
    }

    inline void dec_ref_count() {
      if ((--mRefCount) == 0)
        delete this;
    }
  }
  ; // class Object



  inline void inc_ref_count(Object* inObject) {
    inObject->inc_ref_count();
  }

  inline void dec_ref_count(Object* inObject) {
    inObject->dec_ref_count();
  }
} // namespace embryo



#endif /* EMBRYO_OBJECT_H */
