#include <sstream>
#include "Exception.h"
#include "ObjectFactoryDealer.h"
#include "Embryo.h"
#include "FixedMonitor.h"
#include "EnergyMonitor.h"
#include "MLPController.h"
#include "ESNController.h"

using namespace std;
using namespace embryo;



ObjectFactoryDealer::ObjectFactoryDealer(uint32_t inRandomSeed) : mRandomSeed(inRandomSeed) {
  add(new EmbryoFactory(*this));
  add(new FixedMonitorFactory(*this));
  add(new EnergyMonitorFactory(*this));
  add(new MLPControllerFactory(*this));
  add(new ESNControllerFactory(*this));
}



ObjectFactoryDealer::~ObjectFactoryDealer() { }



void
ObjectFactoryDealer::add(ObjectFactory::Handle inFactory) {
  map<std::string, ObjectFactory::Handle>::const_iterator it =
    mFactoryMap.find(inFactory->name());

  if (it == mFactoryMap.end())
    mFactoryMap.insert(make_pair(inFactory->name(), inFactory));
  else {
    ostringstream lBuffer;
    lBuffer << "a factory with name '" << inFactory->name() << "' already added";
    throw Exception(lBuffer.str());
  }
}



const ObjectFactory&
ObjectFactoryDealer::get(const std::string& inName) const {
  map<std::string, ObjectFactory::Handle>::const_iterator it =
    mFactoryMap.find(inName);

  if (it != mFactoryMap.end())
    return *((*it).second);
  else {
    ostringstream lBuffer;
    lBuffer << "any factory with name '" << inName << "'";
    throw Exception(lBuffer.str());
  }
}



Object::Handle
ObjectFactoryDealer::deserialize(const std::string& inName,
                                 MapTree::Handle inMapTree) const {
  Object::Handle lObj;
  try {
    const ObjectFactory& lFactory = get(inName);
    lObj = lFactory.produce(inMapTree);
  } catch(Exception inE) {
    std::ostringstream lBuffer;
    lBuffer << "At node '" << inName << "' :\n  " << inE.getMessage();
    throw Exception(lBuffer.str());
  }

  return lObj;
}

