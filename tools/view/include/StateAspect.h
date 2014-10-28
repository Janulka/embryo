#ifndef STATE_ASPECT_H
#define STATE_ASPECT_H

#include "EmbryoAspect.h"




namespace embryo {
  class StateAspect : public EmbryoAspect {
  private:
    size_t mStateIndex;

  public:
    StateAspect(size_t inStateIndex);

    virtual ~StateAspect();

    virtual void set(const Embryo& inEmbryo, Picture& outPicture);
  }
  ; // class StateAspect
} // namespace embryo



#endif /* STATE_ASPECT_H */
