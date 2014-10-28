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

#ifndef EMBRYO_MAP_TREE_H
#define EMBRYO_MAP_TREE_H

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <Object.h>



namespace embryo {
  /*
   * A MapTree is a tree structure to store data structures in ASCII files.
   * A node of a MapTree is a map that associate strings, the "keys", to
   * "values". Values are either child MapTree, or atomic values, like numbers.
   * The "keys" within a MapTree should be unique.
   *
   * The MapTreeParser class allow to read files as MapTrees.
   */

  class MapTree : public Object {
  private:
    typedef std::map<std::string, HandleT<MapTree> > NodeMap;
    typedef std::map<std::string, std::string> LeafMap;

    NodeMap mNodesMap;
    LeafMap mLeavesMap;

  public:
    typedef HandleT<MapTree> Handle;



    virtual ~MapTree();

    // Add a couple <key, value>
    void add(const std::string& inKey, const std::string& inValue);

    // Add a couple <key, MapTree>
    void add(const std::string& inKey, MapTree::Handle inTree);

    // Checks if a key exists
    bool hasKey(const std::string& inKey) const;

    // Retrieves a boolean value associated to a given key
    void get(const std::string& inKey, bool& outValue) const;

    // Retrieves a size_t value associated to a given key
    void get(const std::string& inKey, size_t& outValue) const;

    // Retrieves a float value associated to a given key
    void get(const std::string& inKey, float& outValue) const;

    // Retrieves a double value associated to a given key
    void get(const std::string& inKey, double& outValue) const;

    // Retrieves a std::string value associated to a given key
    void get(const std::string& inKey, std::string& outValue) const;

    // Retrieves a child MapTree associated to a given key
    MapTree::Handle getChild(const std::string& inKey);

    /* Retrieves the unique child of a MapTree. If there is multiple childrenn
     *  an exception is thrown
     */
    std::pair<std::string, HandleT<MapTree> > getChild();

    // Outputs the MapTree to an output stream
    void save(std::ostream& inStream) const;

  private:
    LeafMap::const_iterator getLeafIterator(const std::string& inKey) const;

    void throwKeyAddedTwice(const std::string& inKey) const;

    void checkDuplicates(const std::string& inKey) const;
  }
  ; // class MapTree
} // namespace embryo



#endif /* EMBRYO_MAP_TREE_H */
