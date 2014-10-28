#include "ObjectFactory.h"
#include "ObjectFactoryDealer.h"

using namespace embryo;



ObjectFactory::ObjectFactory(const std::string& inName,
                             const ObjectFactoryDealer& inDealer) : mName(inName), mDealer(&inDealer) { }



ObjectFactory::~ObjectFactory() { }

