/*
 *  libembryo
 *  Copyright (C) 2008 by Alexandre Devert
 *
 *           DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *                  Version 2, December 2004
 *
 *  Copyright (C) 2004 Sam Hocevar
 *  14 rue de Plaisance, 75014 Paris, France
 *  Everyone is permitted to copy and distribute verbatim or modified
 *  copies of this license document, and changing it is allowed as long
 *  as the name is changed.
 *
 *            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *  TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
 *
 *  0. You just DO WHAT THE FUCK YOU WANT TO.
 */

#include <png.h>
#include "PNG.h"

#include "Exception.h"

using namespace std;
using namespace embryo;



static void
errorCallback(png_structp png_ptr, png_const_charp error_msg) {
  throw Exception(error_msg);
}



static void
warningCallback(png_structp png_ptr, png_const_charp warning_msg) {
  cerr << warning_msg << endl;
}



static void
readDataCallback(png_structp png_ptr, png_bytep data, png_size_t length) {
  istream* lInput = (istream*)png_get_io_ptr(png_ptr);
  lInput->read((char*)data, length);
}



static void
writeDataCallback(png_structp png_ptr, png_bytep data, png_size_t length) {
  ostream* lOutput = (ostream*)png_get_io_ptr(png_ptr);
  lOutput->write((char*)data, length);
}



static void
flushCallback(png_structp png_ptr) {
  ostream* lOutput = (ostream*)png_get_io_ptr(png_ptr);
  lOutput->flush();
}



Picture
embryo::loadPNG(std::istream& inStream) {
  // Setup libpng
  png_structp lPngPtr =
    png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
  if (!lPngPtr)
    throw Exception("png_create_read_struct failure");

  png_infop lInfoPtr =
    png_create_info_struct(lPngPtr);
  if (!lInfoPtr) {
    png_destroy_read_struct(&lPngPtr, 0, 0);
    throw Exception("png_create_info_struct failure");
  }

  png_set_error_fn(lPngPtr, 0, &errorCallback, &warningCallback);
  png_set_read_fn(lPngPtr, &inStream, &readDataCallback);

  // Read the file info
  png_read_info(lPngPtr, lInfoPtr);

  int lBitDepth = png_get_bit_depth(lPngPtr, lInfoPtr);
  if (lBitDepth == 16)
    png_set_strip_16(lPngPtr);
  else if (lBitDepth < 8)
    png_set_expand(lPngPtr);

  switch(png_get_color_type(lPngPtr, lInfoPtr)) {
  case PNG_COLOR_TYPE_RGB:
    png_set_rgb_to_gray_fixed(lPngPtr, 1, -1, -1);
    break;

  case PNG_COLOR_TYPE_RGB_ALPHA:
    png_set_strip_alpha(lPngPtr);
    png_set_rgb_to_gray_fixed(lPngPtr, 1, -1, -1);
    break;

  case PNG_COLOR_TYPE_PALETTE:
    png_set_rgb_to_gray_fixed(lPngPtr, 1, -1, -1);
    break;

  case PNG_COLOR_TYPE_GRAY:
    break;

  case PNG_COLOR_TYPE_GRAY_ALPHA:
    png_set_strip_alpha(lPngPtr);
    break;
  }

  // Read pictures data
  size_t lWidth = png_get_image_width(lPngPtr, lInfoPtr);
  size_t lHeight = png_get_image_height(lPngPtr, lInfoPtr);

  png_bytep lRaster = new png_byte[lWidth * lHeight];
  png_bytep* lRasterRows = new png_bytep[lHeight];
  {
    png_bytep lOffset = lRaster;
    for(size_t i = 0; i < lHeight; lOffset += lWidth, ++i)
      lRasterRows[i] = lOffset;
  }
  png_read_image(lPngPtr, lRasterRows);
  png_read_end(lPngPtr, 0);

  // Convert to double
  Picture lPicture(lWidth, lHeight);
  {
    png_bytep lSrc = lRaster;
    double* lDst = lPicture.pixels();
    for(size_t i = lWidth * lHeight; i != 0; ++lSrc, ++lDst, --i)
      (*lDst) = double(*lSrc) / 255.0;
  }
  
  // Job done
  delete[] lRasterRows;
  delete[] lRaster;

  png_destroy_read_struct(&lPngPtr, &lInfoPtr, 0);

  return lPicture;
}



void
embryo::savePNG(ostream& inStream, const Picture& inPicture) {
  // Setup libpng
  png_structp lPngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
  if (!lPngPtr)
    throw Exception("Out of memory");

  png_infop lInfoPtr = png_create_info_struct(lPngPtr);
  if (!lInfoPtr) {
    png_destroy_write_struct(&lPngPtr, 0);
    throw Exception("Out of memory");
  }

  png_set_error_fn(lPngPtr, 0, &errorCallback, &warningCallback);
  png_set_write_fn(lPngPtr, &inStream, &writeDataCallback, &flushCallback);

  // Setup and write the picture infos
  png_set_IHDR(lPngPtr, lInfoPtr,
                 inPicture.width(),
                 inPicture.height(),
                 8,
                 PNG_COLOR_TYPE_GRAY,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
  png_write_info(lPngPtr, lInfoPtr);
 
  // Write the picture datas
  png_bytep lRasterRow = new png_byte[inPicture.width()];
  const double* lSrc = inPicture.pixels();

  for(size_t i = 0; i < inPicture.height(); ++i) {
    png_bytep lDst = lRasterRow;
    for(size_t j = inPicture.width(); j != 0; ++lSrc, ++lDst, --j)
      (*lDst) = 255.0 * (*lSrc);

    png_write_row(lPngPtr, lRasterRow);
  }

  png_write_end(lPngPtr, lInfoPtr);

  // Job done
  delete[] lRasterRow;
  png_destroy_write_struct(&lPngPtr, &lInfoPtr);
}
