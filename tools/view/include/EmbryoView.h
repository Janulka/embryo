#ifndef EMBRYO_VIEW_H
#define EMBRYO_VIEW_H

#include "Widget.h"
#include "EmbryoAspect.h"



namespace embryo {
  class EmbryoView : public Widget, public EmbryoListener {
  private:
    Picture mPicture;
//    Picture mPictureTarget; //only for showing target pattern on background
    EmbryoAspect::Handle mAspect;

  public:
    EmbryoView(Embryo& inEmbryo, EmbryoAspect::Handle inAspect, size_t iIndex);

    EmbryoView(Picture& inPicture, EmbryoAspect::Handle inAspect);

    virtual ~EmbryoView();

    virtual void paint(cairo_t* inContext, bool ibColor);

    virtual void onInit(const Embryo& inEmbryo);

    virtual void onUpdate(const Embryo& inEmbryo);

    inline Picture & getPicture() { return mPicture; }
    inline Picture * getPictureP() { return &mPicture; }
  }
  ; // class EmbryoView
} // namespace embryo



#endif /* EMBRYO_VIEW_H */
