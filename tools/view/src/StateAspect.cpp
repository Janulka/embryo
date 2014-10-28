#include "StateAspect.h"

using namespace embryo;



StateAspect::StateAspect(size_t inStateIndex) : mStateIndex(inStateIndex) { }



StateAspect::~StateAspect() { }



void
StateAspect::set(const Embryo& inEmbryo, Picture& outPicture) {
  inEmbryo.getStates(outPicture, mStateIndex);
  outPicture.affineTransform(0.5, 0.5);
}
