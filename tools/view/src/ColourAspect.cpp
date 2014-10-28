#include "ColourAspect.h"

using namespace embryo;



//Embryo::Handle lEmbryo; Picture lPic(lEmbryo->width(), lEmbryo->height()); lEmbryo->getColours(lPic)

ColourAspect::~ColourAspect() { }




void
ColourAspect::set(const Embryo& inEmbryo, Picture& outPicture) {
  inEmbryo.getColours(outPicture);
}
