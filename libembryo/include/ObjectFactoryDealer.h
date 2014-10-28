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

#ifndef EMBRYO_OBJECT_FACTORY_DEALER_H
#define EMBRYO_OBJECT_FACTORY_DEALER_H

#include <map>
#include <stdint.h>
#include <ObjectFactory.h>



namespace embryo {
  /*
   * An ObjectFactoryDealer owns one instance of all the concrete ObjectFactory
   * implementation of libembryo.
   */

  class ObjectFactoryDealer {
  private:
    uint32_t mRandomSeed;
    std::map<std::string, ObjectFactory::Handle> mFactoryMap;

  public:
    ObjectFactoryDealer(uint32_t inRandomSeed);

    virtual ~ObjectFactoryDealer();

    inline uint32_t randomSeed() const {
      return mRandomSeed;
    }

    void add(ObjectFactory::Handle inFactory);

    const ObjectFactory& get(const std::string& inName) const;

    Object::Handle deserialize(const std::string& inName,
                               MapTree::Handle inMapTree) const;

    template <class T>
    HandleT<T> deserializeT(const std::string& inName,
                            MapTree::Handle inMapTree) const {
      return dynamic_pointer_cast<T, Object>(deserialize(inName, inMapTree));
    }
  }
  ; // class ObjectFactoryDealer
} // namespace embryo



#endif /* EMBRYO_OBJECT_FACTORY_DEALER_H */
