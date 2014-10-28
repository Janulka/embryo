/*
 *  libembryo
 *  Copyright (C) 2010 by Jana Hlavacikova
 *
 *
 */

#include <iostream>
#include "AspectCell.h"

using namespace embryo;

AspectCell::AspectCell() : mPositionX(0), mPositionY(0), mSize(0), mAspect(0.0) { }

AspectCell::AspectCell(int iPositionX, int iPositionY, int iSize, double iAspect)// : mPositionX(iPositionX), mPositionY(iPositionY), mSize(iSize), mAspect(iAspect)
{
    mPositionX = iPositionX;
    mPositionY = iPositionY;
    mSize = iSize;
    mAspect = iAspect;
}

AspectCell::~AspectCell() { }

void AspectCell::setCell(int iPositionX, int iPositionY, int iSize, double iAspect) {
  mPositionX = iPositionX;
  mPositionY = iPositionY;
  mSize = iSize;
  mAspect = iAspect;
}