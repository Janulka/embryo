#ifndef WIDGET_CONTAINER_H
#define WIDGET_CONTAINER_H

#include <vector>
#include "Widget.h"



namespace embryo {

    struct WidgetContainerNode {
        Widget::Handle mWidget;
        std::vector<WidgetContainerNode*> mChildren;

        WidgetContainerNode() : mWidget(0) {
        }

        WidgetContainerNode(Widget::Handle inWidget) : mWidget(inWidget) {
        }

        void paint(cairo_t* inContext,
                double inX, double inY,
                double inWidth, double inHeight, bool ibInit);
    }
    ; // struct WidgetContainerNode

    class WidgetContainer {
    private:
        std::vector<WidgetContainerNode*> mStack;
        std::vector<WidgetContainerNode*> mNodes;

    public:
        WidgetContainer();

        virtual ~WidgetContainer();

        void addWidget(Widget::Handle inWidget);

        void push();

        void pop();

        void clear(SDL_Surface* inSurface);

        void paint(cairo_t* inContext,
                double inX, double inY,
                double inWidth, double inHeight, bool ibInit);

        void paint(SDL_Surface* inSurface, bool ibInit);

        void requestAspects();

    }
    ; // class WidgetContainer
} //



#endif /* WIDGET_CONTAINER_H */
