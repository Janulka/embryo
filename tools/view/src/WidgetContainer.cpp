#include <iostream>

#include <omp.h>

#include "WidgetContainer.h"

using namespace std;
using namespace embryo;

void
WidgetContainerNode::paint(cairo_t* inContext,
        double inX, double inY,
        double inWidth, double inHeight, bool ibInit) {

    //std::cout << "WidgetContainerNode painting " << std::endl;

    if (mWidget != 0) {
        double lSize = 0.9 * min(inWidth, inHeight);
        double lX = inX + (inWidth - lSize) / 2;
        double lY = inY + (inHeight - lSize) / 2;

        cairo_matrix_t lGlobalMatrix;

        cairo_get_matrix(inContext, &lGlobalMatrix);
        cairo_translate(inContext, lX, lY);
        cairo_scale(inContext, lSize, lSize);

        mWidget->paint(inContext, ibInit);
        cairo_set_matrix(inContext, &lGlobalMatrix);
    } else {
        if (inWidth >= inHeight) {
            double lXInc = inWidth / mChildren.size();

            #pragma omp parallel for
            for (size_t i = 0; i < mChildren.size(); ++i)
                mChildren.at(i)->paint(inContext,
                    inX + double(i) * lXInc, inY,
                    lXInc, inHeight, ibInit);
        } else {
            double lYInc = inHeight / mChildren.size();

            #pragma omp parallel for
            for (size_t i = 0; i < mChildren.size(); ++i)
                mChildren.at(i)->paint(inContext,
                    inX, inY + double(i) * lYInc,
                    inWidth, lYInc, ibInit);
        }
    }
}

WidgetContainer::WidgetContainer() {
    WidgetContainerNode* lNode = new WidgetContainerNode();
    mNodes.push_back(lNode);
    mStack.push_back(lNode);
}

WidgetContainer::~WidgetContainer() {
    vector<WidgetContainerNode*>::iterator it = mNodes.begin();
    for (; it != mNodes.end(); ++it)
        delete *it;
}

void
WidgetContainer::addWidget(Widget::Handle inWidget) {
    WidgetContainerNode* lLeaf = new WidgetContainerNode(inWidget);
    mNodes.push_back(lLeaf);
    mStack.back()->mChildren.push_back(lLeaf);
}

void
WidgetContainer::push() {
    WidgetContainerNode* lNode = new WidgetContainerNode();
    mNodes.push_back(lNode);
    mStack.back()->mChildren.push_back(lNode);
    mStack.push_back(lNode);
}

void
WidgetContainer::pop() {
    mStack.pop_back();
}

void
WidgetContainer::clear(SDL_Surface* inSurface) {
     cairo_surface_t* lSurface =
            cairo_image_surface_create_for_data((unsigned char*) inSurface->pixels,
            CAIRO_FORMAT_ARGB32,
            inSurface->w,
            inSurface->h,
            inSurface->pitch);

    cairo_t* lContext = cairo_create(lSurface);
    cairo_set_source_rgb(lContext, 0.8, 0.8, 1.0);
//    cairo_paint_with_alpha(lContext, 1.0);
    cairo_paint(lContext);
    cairo_destroy(lContext);
}

void
WidgetContainer::paint(SDL_Surface* inSurface, bool ibInit) {

    //std::cout << "WidgetContainer painting " << std::endl;

    cairo_surface_t* lSurface =
            cairo_image_surface_create_for_data((unsigned char*) inSurface->pixels,
            CAIRO_FORMAT_ARGB32,
            inSurface->w,
            inSurface->h,
            inSurface->pitch);

    cairo_t* lContext = cairo_create(lSurface);
    //cairo_set_source_surface(lContext, lSurface, 0, 0);
    cairo_set_source_rgb(lContext, 0.8, 0.8, 1.0);
    cairo_paint_with_alpha(lContext, 1.0);
    // cairo_set_source_rgba(lContext, 0.8, 0.8, 1.0, 1.0);
    paint(lContext, 0, 0, inSurface->w, inSurface->h, ibInit);
    cairo_destroy(lContext);
}

void
WidgetContainer::paint(cairo_t* inContext,
        double inX, double inY,
        double inWidth, double inHeight, bool ibInit) {
    mStack.front()->paint(inContext, inX, inY, inWidth, inHeight, ibInit);
}

//void WidgetContainer::requestAspects() {
//    std::vector<WidgetContainerNode*>::iterator it = mNodes.begin();
//    while(it != mNodes.end()) {
////        (EmbryoView)((*it)->mWidget).getPicture();
//        ++it;
//    }

//    mStack.front()->
//}
