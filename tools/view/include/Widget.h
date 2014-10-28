#ifndef WIDGET_H
#define WIDGET_H

#include <SDL/SDL.h>
#include <cairo.h>
#include <embryo.h>



namespace embryo {
  class Widget : public Object {
  public:
    typedef HandleT<Widget> Handle;



    virtual ~Widget();

    virtual void paint(cairo_t* inContext, bool ibInit);
  }
  ; // class Widget
} // namespace embryo



#endif /* WIDGET_H */
