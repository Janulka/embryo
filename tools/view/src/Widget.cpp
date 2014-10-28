#include <cmath>
#include "Widget.h"

using namespace embryo;



Widget::~Widget() { }



void
Widget::paint(cairo_t* inContext, bool ibInit) {
  cairo_set_line_width(inContext, 0.01);
  cairo_set_source_rgb(inContext, 0.0, 0.0, 1.0);
  cairo_arc(inContext, 0.5, 0.5, 0.5, 0, 2 * M_PI);
  cairo_stroke(inContext);

  cairo_move_to(inContext, 0.0, 0.0);
  cairo_line_to(inContext, 1.0, 0.0);
  cairo_line_to(inContext, 1.0, 1.0);
  cairo_line_to(inContext, 0.0, 1.0);
  cairo_line_to(inContext, 0.0, 0.0);
  cairo_stroke(inContext);
}
