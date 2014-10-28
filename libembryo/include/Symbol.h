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

#ifndef EMBRYO_SYMBOL_H
#define EMBRYO_SYMBOL_H

#include <string>


namespace embryo {
  class SymbolId {
  private:
    int mId;
    bool mIsTerminal;
    std::string mDescription;

  public:
    static SymbolId PTH_OPEN;
    static SymbolId PTH_CLOSE;
    static SymbolId IDENTIFIER;
    static SymbolId EQUAL;
    static SymbolId END_OF_FILE;
    static SymbolId UNKNOW;
    static SymbolId Spec;
    static SymbolId Tag;
    static SymbolId TagBody;
    static SymbolId TagList;
    static SymbolId TopTagList;
    static SymbolId Atom;
    static SymbolId DELIM;


    inline bool operator == (const SymbolId& inSymbolId) const {
      return mId == inSymbolId.mId;
    }

    inline bool operator != (const SymbolId& inSymbolId) const {
      return mId != inSymbolId.mId;
    }

    inline bool isTerminal() const {
      return mIsTerminal;
    }

    inline const std::string& description() const {
      return mDescription;
    }

  private:
    SymbolId(int inId,
             const std::string& inStr,
             bool inIsTerminal = true);
  }
  ; // class SymbolId



  struct Symbol {
    const SymbolId* mId;
    size_t mLine;
    std::string mLexem;



    inline const std::string& lexem() const {
      return mLexem;
    }

    inline const SymbolId& id() const {
      return *mId;
    }

    inline size_t lineNumber() const {
      return mLine;
    }
  }
  ; // struct Symbol
} // embryo



#endif /* EMBRYO_SYMBOL_H */
