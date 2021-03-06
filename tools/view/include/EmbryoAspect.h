#ifndef EMBRYO_ASPECT_H
#define EMBRYO_ASPECT_H

#include <embryo.h>



namespace embryo {
  class EmbryoAspect : public Object {
  public:
    typedef HandleT<EmbryoAspect> Handle;



    virtual ~EmbryoAspect();

    virtual void set(const Embryo& inEmbryo, Picture& outPicture) = 0;
  }
  ; // class EmbryoAspect
} // namespace embryo



#endif /* EMBRYO_ASPECT_H */
