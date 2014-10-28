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
#include <sstream>
#include "Exception.h"
#include "MapTreeParser.h"

using namespace std;
using namespace embryo;



namespace embryo {
  /*
   * The productions of the grammar, (Must be LL(1), checked by hand)
   * A production of length 0 is an empty production.
   */

  struct Production {
    int mId;
    const SymbolId* mLookup;
    int mLength;
    const SymbolId* mHead;
    const SymbolId* mProduction[4];
  }
  ; // struct Production



  // Human readable production ids
  enum ProductionId {
    Spec__TopTagList_EOF,
    TopTagList__Tag_TopTagList,
    TopTagList__empty,
    Tag__ID_TagBody,
    TagBody__EQ_Atom,
    TagBody__OPEN_TagList_CLOSE,
    TagList__Tag_TagList,
    TagList__empty,
    Atom__ID
  }; // enum ProductionId
} // namespace embryo



static struct Production grammarProductions[]
  = {
    /* Spec -> TopTagList <End-of-file> */
    {
      Spec__TopTagList_EOF,
      &SymbolId::IDENTIFIER, 2,
      &SymbolId::Spec, {&SymbolId::TopTagList, &SymbolId::END_OF_FILE, 0, 0}
    },

    /* TopTagList -> Tag TopTagList */
    {
      TopTagList__Tag_TopTagList,
      &SymbolId::IDENTIFIER, 2,
      &SymbolId::TopTagList, {&SymbolId::Tag, &SymbolId::TopTagList, 0, 0}
    },

    /* TopTagList -> (null) */
    {
      TopTagList__empty,
      &SymbolId::END_OF_FILE, 0,
      &SymbolId::TopTagList, {0, 0, 0, 0}
    },

    /* Tag -> <Identifier> TagBody */
    {
      Tag__ID_TagBody,
      &SymbolId::IDENTIFIER, 2,
      &SymbolId::Tag, {&SymbolId::IDENTIFIER, &SymbolId::TagBody, 0, 0}
    },

    /* TagBody -> <Equals> Atom */
    {
      TagBody__EQ_Atom,
      &SymbolId::EQUAL, 2,
      &SymbolId::TagBody, {&SymbolId::EQUAL, &SymbolId::Atom, 0 ,0}
    },

    /* TagBody -> <Opener> TagList <Closer> */
    {
      TagBody__OPEN_TagList_CLOSE,
      &SymbolId::PTH_OPEN, 3,
      &SymbolId::TagBody, {&SymbolId::PTH_OPEN, &SymbolId::TagList, &SymbolId::PTH_CLOSE, 0}
    },

    /* TagList -> Tag TagList */
    {
      TagList__Tag_TagList,
      &SymbolId::IDENTIFIER, 2,
      &SymbolId::TagList, {&SymbolId::Tag, &SymbolId::TagList, 0, 0}
    },

    /* TagList -> (null) */
    {
      TagList__empty,
      &SymbolId::PTH_CLOSE, 0,
      &SymbolId::TagList, {0, 0, 0, 0}
    },

    /* Atom -> <Identifier> */
    {
      Atom__ID,
      &SymbolId::IDENTIFIER, 1,
      &SymbolId::Atom, {&SymbolId::IDENTIFIER, 0, 0, 0}
    }
  };

#define NB_PRODUCTIONS \
(sizeof(grammarProductions) / sizeof(struct Production))



void
MapTreeParser::getTerminal() {
  if (mLexer.endReached())
    throwParseException(mLexer.lineNumber(), "Unexcepted end of file");
  else
    mLexer.next(mLookup);
}



MapTreeParser::MapTreeParser(istream& inStream) : mLexer(inStream) { }



void
MapTreeParser::throwParseException(size_t inLineNumber,
                                   const std::string& inMsg) {
  ostringstream lBuffer;
  lBuffer << "error at line " << inLineNumber << " : "<< inMsg;
  throw Exception(lBuffer.str());
}



void
MapTreeParser::onReduction(int inProductionId) {
  HandleT<MapTree> lTree;

  switch(inProductionId) {
  case Tag__ID_TagBody:
    mLabelStack.push_back(mLookup.lexem());
    break;

  case TagBody__OPEN_TagList_CLOSE:
    mMapTreeStack.push_back(new MapTree());
    break;

  case TagList__empty:
    lTree = mMapTreeStack.back();
    mMapTreeStack.pop_back();
    mMapTreeStack.back()->add(mLabelStack.back(), lTree);
    mLabelStack.pop_back();
    break;

  case Atom__ID:
    mMapTreeStack.back()->add(mLabelStack.back(), mLookup.lexem());
    mLabelStack.pop_back();
    break;

  default:
    break;
  }
}



HandleT<MapTree>
MapTreeParser::parse() {
  mMapTreeStack.clear();
  mMapTreeStack.push_back(new MapTree());

  // Put the start symbol in the self stack and read the lookup token
  mSymbolStack.clear();
  mSymbolStack.push_back(&SymbolId::Spec);
  getTerminal();

  // Until the symbol stack is empty
  while(!mSymbolStack.empty()) {

  //    cout << "mLookup.mId: ";
  //    cout << mLookup.mId;
  //    cout << " | mSymbolStack.back() : ";
  //    cout << mSymbolStack.back();

 //     cout << "mLookup.id().description(): ";
 //     cout << mLookup.id().description();
 //     cout << " | mSymbolStack.back()->description() : ";
 //     cout << mSymbolStack.back()->description();
   
	if (mSymbolStack.back()->isTerminal()) {
      // The terminal founded is not the excepted one
      if (mLookup.mId != mSymbolStack.back()) {
        ostringstream lBuffer;
        lBuffer
        << mLookup.id().description()
        << " was found, instead of "
        << mSymbolStack.back()->description();

        throwParseException(mLookup.lineNumber(), lBuffer.str());
      }
      // Ok, this is the good one
      else {
        mSymbolStack.pop_back();

        // Reduction ended
        if (!mSymbolStack.empty())
          getTerminal();
        else {
          // Except the end of the file
          if (!mLexer.endReached())
            throwParseException(mLexer.lineNumber(), "End of file excepted");
        }
      }
    } else {
      size_t i;


  //    cout << "mLookup.mId: ";
  //    cout << mLookup.mId;
  //    cout << " | mSymbolStack.back() : ";
  //    cout << mSymbolStack.back();

 //     cout << "mLookup.id().description(): ";
 //     cout << mLookup.id().description();
 //     cout << " | mSymbolStack.back()->description() : ";
 //     cout << mSymbolStack.back()->description();

      // Search for a matching production
      for(i = 0; i < NB_PRODUCTIONS; i++) {
	if ((mLookup.mId == grammarProductions[i].mLookup) &&
                (grammarProductions[i].mHead == mSymbolStack.back()))
          break;
      }

      // No matching production found
      if (i == NB_PRODUCTIONS) {
        ostringstream lBuffer;
        lBuffer
        << "waiting for " << mSymbolStack.back()->description()
        << " instead of " << mLookup.id().description();

        throwParseException(mLookup.lineNumber(), lBuffer.str());
      }
      // Apply the reduction corresponding to the production found
      else {
        onReduction(grammarProductions[i].mId);
        mSymbolStack.pop_back();
        for(int j = grammarProductions[i].mLength - 1; j >= 0; j--)
          mSymbolStack.push_back(grammarProductions[i].mProduction[j]);
      }
    }
  }
  
  // Job done
  return mMapTreeStack.back();
}
