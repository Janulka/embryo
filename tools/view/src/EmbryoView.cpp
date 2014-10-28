#include <cmath>
#include "EmbryoView.h"

using namespace embryo;


//EmbryoView::EmbryoView(Embryo& inEmbryo, EmbryoAspect::Handle inAspect) : mPicture(inEmbryo.width(), inEmbryo.height()), mPictureTarget(inEmbryo.width(), inEmbryo.height()), mAspect(inAspect) {

EmbryoView::EmbryoView(Embryo& inEmbryo, EmbryoAspect::Handle inAspect, size_t iIndex) : mPicture(inEmbryo.width(), inEmbryo.height()), mAspect(inAspect) {
    //EmbryoView::EmbryoView(Embryo& inEmbryo, EmbryoAspect::Handle inAspect) : mPicture(inEmbryo.getTargetPicC(lTargetIndex)), mAspect(inAspect) {
    //    std::cout << "EmbryoView::constructor " << std::endl;
    //size_t lTargetIndex = 0;
    //    mPictureTarget = inEmbryo.getTargetPic(lTargetIndex);
    //    Picture lP = inEmbryo.getTargetPic(0);
    const double * lTargetPixels = (inEmbryo.getTargetPicPixelsC(iIndex));
    //    double * lPixels = mPictureTarget.pixels();
    double * lPixels = mPicture.pixels();
    size_t lNbPixels = inEmbryo.width() * inEmbryo.height();
    ////    double * lTargetPixels = (inEmbryo.getTargetPic((size_t)0)).pixels();
    ////    mPicture.setPixels(lTargetPixels);
    arrayd::copy(lPixels, lTargetPixels, lNbPixels);
    inEmbryo.addListener(this);
}

EmbryoView::EmbryoView(Picture& inPicture, EmbryoAspect::Handle inAspect) : mPicture(inPicture), mAspect(inAspect) {
    //std::cout << "EmbryoView::constructor " << std::endl;
    //    inEmbryo.addListener(this);
}

EmbryoView::~EmbryoView() {
}

void
EmbryoView::onInit(const Embryo& inEmbryo) {
    //std::cout << "EmbryoView::onInit " << std::endl;
    //mPicture.fill(0.0);
}

void
EmbryoView::onUpdate(const Embryo& inEmbryo) {
    mAspect->set(inEmbryo, mPicture);
}


//TODO: change to drawing mAspectCells list<AspectCell> or list<ColorAspect> ??

void
EmbryoView::paint(cairo_t* inContext, bool ibColor) {


    //    double lRectWidth = 0.100;//iLength / (double) mPicture.width(); // size of the cell of drawn aspect object (color, state, chemical)
    double lRectWidth = 1.00 / (double) mPicture.width(); // size of the cell of drawn aspect object (color, state, chemical)
    double lShift = 0.1 * lRectWidth; // space between cells

    //    printf("\n pic w: %d | rect: %e | sh: %e\n", mPicture.width(), lRectWidth, lShift);



    if (!ibColor) {
        const double* lPixel = mPicture.pixels();
        for (size_t i = 0; i < mPicture.height(); ++i) {
            double lY = i * lRectWidth;
            for (size_t j = 0; j < mPicture.width(); ++j, ++lPixel) {
                double lX = j * lRectWidth;
                cairo_set_source_rgba(inContext, *lPixel, *lPixel, *lPixel, 0.5);
                //                cairo_rectangle(inContext, lX + lShift, lY + lShift, lRectWidth - lShift, lRectWidth - lShift);   //to see the initial grid
                cairo_rectangle(inContext, lX, lY, lRectWidth, lRectWidth);
                cairo_fill(inContext);
            }
        }
        
        cairo_set_source_rgba(inContext, 0.0, 0.0, 0.0, 0.8);
    }
    
    AspectCellContainer acc;
    AspectCellIterator aciBegin;
    AspectCellIterator aciEnd;

    mPicture.getAspectCellsContainer(acc);
    //  mPicture.getAspectCells(aciBegin, aciEnd);
    aciBegin = acc.begin();
    aciEnd = acc.end();

    int c = 0;
    while (aciBegin != aciEnd) {

        double lY = (double) ((*aciBegin)->getPositionY()) * lRectWidth + lShift;
        double lX = (double) ((*aciBegin)->getPositionX()) * lRectWidth + lShift;
        int lSize = (*aciBegin)->getSize();
        double lAspect = (*aciBegin)->getAspect();

        //        printf("%d> x: %d, y: %d, s: %d, a: %e \n", ++c, lX, lY, lSize, lAspect);
        double lLength = (double) (sqrt(lSize)) * lRectWidth - lShift;

        //        printf("size: %d, x: %e, y: %e, length: %e\n", lSize, lX + lShift, lY + lShift, lLength);

//        cairo_set_source_rgb(inContext, lAspect, lAspect, lAspect);
        cairo_rectangle(inContext, lX, lY, lLength, lLength);
//        if (ibColor)
            cairo_set_source_rgba(inContext, lAspect, lAspect, lAspect, 0.8);

        cairo_fill(inContext);
        ++aciBegin;
    }
}
