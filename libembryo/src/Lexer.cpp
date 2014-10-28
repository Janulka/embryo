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

#include <cassert>
#include "Lexer.h"

using namespace std;
using namespace embryo;



enum eLexerState {
  s_IDLE,
  s_END,
  s_COMMENT_START,
  s_ID_DIGIT
}; /* enum eLexerState */



Lexer::Lexer(istream& inStream) : mStream(inStream) {
  mLineNum = 1;
  mEndReached = false;
  mStream.get(mCurrent);
}



void
Lexer::next(Symbol& inSymbol) {
  mBuffer.str("");
  inSymbol.mLexem = "";

  enum eLexerState lCurrState = s_IDLE;
  while(lCurrState != s_END) {
    if (mStream.eof()) {
      inSymbol.mId = &SymbolId::END_OF_FILE;
      lCurrState = s_END;
      mEndReached = true;
      continue;
    }

    switch(lCurrState) {
    case s_IDLE:
      switch(mCurrent) {
      case '#':
        lCurrState = s_COMMENT_START;
        mStream.get(mCurrent);
        break;

      case '(':
        inSymbol.mId = &SymbolId::PTH_OPEN;
        mBuffer.put(mCurrent);
        lCurrState = s_END;
        mStream.get(mCurrent);
        break;

      case ')':
        inSymbol.mId = &SymbolId::PTH_CLOSE;
        mBuffer.put(mCurrent);
        lCurrState = s_END;
        mStream.get(mCurrent);
        break;

      case '=':
        inSymbol.mId = &SymbolId::EQUAL;
        mBuffer.put('=');
        lCurrState = s_END;
        mStream.get(mCurrent);
        break;

      case '\n':
        mStream.get(mCurrent);
        mLineNum++;
        break;

      case ' ':
      case '\t':
        mStream.get(mCurrent);
        break;
	
      case '|':
        inSymbol.mId = &SymbolId::TagBody; //DELIM;
      //  lCurrState = s_DELIM;
	mBuffer.put(mCurrent);        
	mStream.get(mCurrent);
	break;

      default:
        if (isalnum(mCurrent))
          lCurrState = s_ID_DIGIT;
        else {
          inSymbol.mId = &SymbolId::UNKNOW;
          lCurrState = s_END;
        }

        mBuffer.put(mCurrent);
        mStream.get(mCurrent);
        break;
      }
      break;

    case s_COMMENT_START:
      lCurrState = s_IDLE;
      while((!mStream.eof()) && (mCurrent != '\n'))
        mStream.get(mCurrent);
      break;

    case s_ID_DIGIT:
      if (isalnum(mCurrent) || (mCurrent == '.') || (mCurrent == '|') || (mCurrent == '/')) {
        mBuffer.put(mCurrent);
        mStream.get(mCurrent);
      } else {
        inSymbol.mLexem = mBuffer.str();
        inSymbol.mId = &SymbolId::IDENTIFIER;
        lCurrState = s_END;
      }
      break;

    default:
      assert(1);
    }
  }

  if (inSymbol.mLexem.empty())
    inSymbol.mLexem = mBuffer.str();

  inSymbol.mLine = mLineNum;
}
