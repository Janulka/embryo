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

#ifndef EMBRYO_LEXER_H
#define EMBRYO_LEXER_H

#include <iostream>
#include <sstream>
#include <Symbol.h>



namespace embryo {
  class Lexer {
  private:
    std::ostringstream mBuffer;
    std::istream& mStream;
    size_t mLineNum;
    bool mEndReached;
    char mCurrent;

  public:
    Lexer(std::istream& inStream);

    void next(Symbol& inSymbol);

    inline bool endReached() const {
      return mEndReached;
    }

    inline size_t lineNumber() const {
      return mLineNum;
    }

    inline std::istream& stream() {
      return mStream;
    }

    inline const std::istream& stream() const {
      return mStream;
    }
  };
  // class Lexer
} // namespace embryo



#endif /* EMBRYO_LEXER_H */