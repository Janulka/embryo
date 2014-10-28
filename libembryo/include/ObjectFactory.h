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

#ifndef EMBRYO_OBJECT_FACTORY_H
#define EMBRYO_OBJECT_FACTORY_H

#include <MapTree.h>



namespace embryo {
  class ObjectFactoryDealer;



  /*
   * This is an abstract class, part of the deserialization system. 
   * A concrete ObjectFactory implementation build instances of a given Object
   * class, from a MapTree. For instance, a FooFactory will generates Foo
   * instances from a MapTree, since Foo is an Object.
   */

  class ObjectFactory : public Object {
  private:
    std::string mName;
    const ObjectFactoryDealer* mDealer;

  public:
    typedef HandleT<ObjectFactory> Handle;



    virtual ~ObjectFactory();

    virtual Object::Handle produce(MapTree::Handle inMapTree) const = 0;

    inline const std::string& name() const {
      return mName;
    }

  protected:
    ObjectFactory(const std::string& inName,
                  const ObjectFactoryDealer& inDealer);

    inline const ObjectFactoryDealer& dealer() const {
      return *mDealer;
    }
  }
  ; // class ObjectFactory
} // namespace embryo



#endif /* EMBRYO_OBJECT_FACTORY_H */
