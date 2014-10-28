/*
 *  libembryo
 *  Copyright (C) 2009 by Jana Hlavacikova
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

#include <climits>
#include <sstream>
#include "Exception.h"
#include "MapTree.h"
#include <stdlib.h>

using namespace std;
using namespace embryo;



MapTree::~MapTree() { }



void
MapTree::add(const std::string& inKey, const std::string& inValue) {
  checkDuplicates(inKey);
  mLeavesMap.insert(make_pair(inKey, inValue));
}



void
MapTree::add(const std::string& inKey, MapTree::Handle inTree) {
  checkDuplicates(inKey);
  mNodesMap.insert(make_pair(inKey, inTree));
}



void
MapTree::throwKeyAddedTwice(const std::string& inKey) const {
  ostringstream lBuffer;
  lBuffer << "key '" << inKey << "' added twice";
  throw Exception(lBuffer.str());
}



void
MapTree::checkDuplicates(const std::string& inKey) const {
  // Check if there already a node for that key
  NodeMap::const_iterator lNodeIt = mNodesMap.find(inKey);
  if (lNodeIt != mNodesMap.end())
    throwKeyAddedTwice(inKey);

  // Check if there already a leaf for that key
  LeafMap::const_iterator lLeafIt = mLeavesMap.find(inKey);
  if (lLeafIt != mLeavesMap.end())
    throwKeyAddedTwice(inKey);
}



MapTree::LeafMap::const_iterator
MapTree::getLeafIterator(const std::string& inKey) const {
  LeafMap::const_iterator lLeafIt = mLeavesMap.find(inKey);
  if (lLeafIt == mLeavesMap.end()) {
    ostringstream lBuffer;
    lBuffer << "key '" << inKey << "' not found";
    throw Exception(lBuffer.str());
  }
  
  return lLeafIt;
}



bool
MapTree::hasKey(const std::string& inKey) const {
  return
    (mNodesMap.find(inKey) != mNodesMap.end()) ||
    (mLeavesMap.find(inKey) != mLeavesMap.end());
}



MapTree::Handle
MapTree::getChild(const std::string& inKey) {
  NodeMap::iterator lNodeIt = mNodesMap.find(inKey);
  if (lNodeIt == mNodesMap.end()) {
    ostringstream lBuffer;
    lBuffer << "key '" << inKey << "' not found";
    throw Exception(lBuffer.str());
  }

  return (*lNodeIt).second;
}



pair<std::string, HandleT<MapTree> >
MapTree::getChild() {
  if (mNodesMap.empty())
    throw Exception("not child nodes found");

  if (mNodesMap.size() > 1)
    throw Exception("more than one child node found");

  return *(mNodesMap.begin());
}



void
MapTree::get(const std::string& inKey, bool& outValue) const {
  LeafMap::const_iterator it = getLeafIterator(inKey);

  if ((*it).second == "true")
    outValue = true;
  else if ((*it).second == "false")
    outValue = false;
  else {
    ostringstream lBuffer;
    lBuffer << "Value associated to key '" << inKey << "' is not a valid boolean";
    throw Exception(lBuffer.str());
  }
}



void
MapTree::get(const std::string& inKey, size_t& outValue) const {
  LeafMap::const_iterator it = getLeafIterator(inKey);

  char* lEndPtr;
  long long lParsedValue = strtoll((*it).second.c_str(), &lEndPtr, 10);

  if ((*lEndPtr) != '\0') {
    ostringstream lBuffer;
    lBuffer << "Value associated to key '" << inKey << "' is not a valid integer value";
    throw Exception(lBuffer.str());
  }

  if ((lParsedValue < 0) || (lParsedValue > UINT_MAX)) {
    ostringstream lBuffer;
    lBuffer << "Value associated to key '" << inKey << "' is out of range [0, " << UINT_MAX << "]";
    throw Exception(lBuffer.str());
  }
  
  outValue = size_t(lParsedValue);
}



void
MapTree::get(const std::string& inKey, float& outValue) const {
  LeafMap::const_iterator it = getLeafIterator(inKey);

  char* lEndPtr;
  outValue = strtof((*it).second.c_str(), &lEndPtr);

  if ((*lEndPtr) != '\0') {
    ostringstream lBuffer;
    lBuffer << "Value associated to key '" << inKey << "' is not a valid floating point  value";
    throw Exception(lBuffer.str());
  }
}



void
MapTree::get(const std::string& inKey, double& outValue) const {
  LeafMap::const_iterator it = getLeafIterator(inKey);

  char* lEndPtr;
  outValue = strtod((*it).second.c_str(), &lEndPtr);

  if ((*lEndPtr) != '\0') {
    ostringstream lBuffer;
    lBuffer << "Value associated to key '" << inKey << "' is not a valid floating point  value";
    throw Exception(lBuffer.str());
  }
}

void
MapTree::get(const std::string& inKey, std::string& outValue) const {
  LeafMap::const_iterator it = getLeafIterator(inKey);

  outValue = (*it).second; 
}

void
MapTree::save(ostream& inStream) const {
  // Print the leaves
  LeafMap::const_iterator lLeafIt = mLeavesMap.begin();
  for( ; lLeafIt != mLeavesMap.end(); ++lLeafIt)
    inStream << (*lLeafIt).first << " = " << (*lLeafIt).second << endl;

  // Print the nodes
  NodeMap::const_iterator lNodeIt = mNodesMap.begin();
  for( ; lNodeIt != mNodesMap.end(); ++lNodeIt) {
    inStream << (*lNodeIt).first << '(' << endl;
    (*lNodeIt).second->save(inStream);
    inStream << ')' << endl;
  }
}
