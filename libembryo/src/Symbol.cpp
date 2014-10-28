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

#include "Symbol.h"

using namespace embryo;



SymbolId SymbolId::PTH_OPEN = SymbolId(0, "'('");
SymbolId SymbolId::PTH_CLOSE = SymbolId(1, "')'");
SymbolId SymbolId::IDENTIFIER = SymbolId(2, "an identifier");
SymbolId SymbolId::EQUAL = SymbolId(3, "'='");
SymbolId SymbolId::END_OF_FILE = SymbolId(4, "the end of the file");
SymbolId SymbolId::UNKNOW = SymbolId(5, "unknow");
SymbolId SymbolId::Spec = SymbolId(6, "an identifier", false);
SymbolId SymbolId::Tag = SymbolId(7, "an identifier", false);
SymbolId SymbolId::TagBody = SymbolId(8, "'=' or '('", false);
SymbolId SymbolId::TagList = SymbolId(9, "an identifier", false);
SymbolId SymbolId::TopTagList = SymbolId(10, "an identifier", false);
SymbolId SymbolId::Atom = SymbolId(11, "an identifier", false);
SymbolId SymbolId::DELIM = SymbolId(12, "'|'");


SymbolId::SymbolId(int inId,
                   const std::string& inStr,
                   bool inIsTerminal) {
  mDescription = inStr;
  mId = inId;
  mIsTerminal = inIsTerminal;
}
