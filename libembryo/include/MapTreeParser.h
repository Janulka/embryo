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

#ifndef EMBRYO_MAP_TREE_PARSER_H
#define EMBRYO_MAP_TREE_PARSER_H

#include <vector>
#include <Lexer.h>
#include <MapTree.h>



namespace embryo {
  /*
   * Build a MapTree instance from ASCII files.
   */

  class MapTreeParser {
  private:
    Lexer mLexer;
    std::vector<const SymbolId*> mSymbolStack;
    std::vector<std::string> mLabelStack;
    std::vector< HandleT<MapTree> > mMapTreeStack;
    Symbol mLookup;

  public:
    MapTreeParser(std::istream& inStream);

    HandleT<MapTree> parse();

  private:
    void getTerminal();

    void onReduction(int inProductionId);

    void throwParseException(size_t inLineNumber, const std::string& inMsg);
  };
  // class MapTreeParser
} // namespace embryo



#endif /* EMBRYO_MAP_TREE_PARSER_H */
