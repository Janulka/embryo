/*
 *  libembryo
 *  Copyright (C) 2010 by Jana Hlavacikova
 *                  Version 3, April 2010
 *
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

#include <stdio.h>
#include <vector>
#include <sstream>
#include <algorithm>
#include <math.h>
#include "PGM.h"
#include "Exception.h"

using namespace std;
using namespace embryo;



namespace embryo {

    long int
    loadPGM_parseInteger(const std::string& inToken) {
        char* lEndPtr;
        long int lValue = strtol(inToken.c_str(), &lEndPtr, 10);

        if (((*lEndPtr) != '\0') || (lValue < 0))
            throw Exception("PGM integer number format error");

        return lValue;
    }

    long int
    loadPGM_parseInteger(const std::string& inToken, long int inMax) {
        long int lValue = loadPGM_parseInteger(inToken);
        if (lValue > inMax)
            throw Exception("PGM integer number format error");

        return lValue;
    }

    static void
    loadPGM_dataChunk_binary(istream& inStream, Picture& inPic, size_t inFactor) {

        double* lOffset = inPic.pixels();

        // Read the bytes one by one
        double lInvFactor = 1.0 / double(inFactor);

        for (size_t x = 0; x < inPic.width(); x++) {
            for (size_t y = 0; y < inPic.height(); y++, lOffset++) {

                // Read a byte
                char lRawByte;
                inStream.get(lRawByte);

                // Check if end of file is reached
                if (inStream.eof())
                    throw Exception("Unexcepted end of file");

                // Copy the byte as a pixel value
                int lPixel = (unsigned char) lRawByte;
                inPic.setNextAspectCell(x, y, 1, lInvFactor * double(lPixel));

                (*lOffset) = lInvFactor * double(lPixel);
            }
        }
    }

    static void
    loadPGM_dataChunk_ascii(istream& inStream, Picture& inPic, size_t inFactor) {
        double* lOffset = inPic.pixels();

        // Skip read the bytes one by one
        double lInvFactor = 1.0 / double(inFactor);

        for (size_t x = 0; x < inPic.width(); x++) {
            for (size_t y = 0; y < inPic.height(); y++, lOffset++) {
                // Read a pixel value
                unsigned short lPixel;
                inStream >> lPixel;

                // Check if end of file is reached
                if (inStream.eof())
                    throw Exception("Unexcepted end of file");

                // Copy the pixel value
                inPic.setNextAspectCell(x, y, 1, lInvFactor * double(lPixel));
                (*lOffset) = lInvFactor * double(lPixel);
            }
        }
    }

    enum PGMTokeniserState {
        idle, token, comment
    }; // enum PGMTokeniserState

    static void
    loadPGM_infoChunk(istream& inStream,
            size_t& inX, size_t& inY, size_t& inFactor) {
        // Tokenize the info chunk
        ostringstream lBuffer;
        vector<std::string> lTokenList;

        enum PGMTokeniserState lState = idle;
        while (lTokenList.size() < 3) {
            // Read a byte
            char lChar;
            inStream.get(lChar);

            // End of file reached
            if (inStream.eof())
                throw Exception("Unexcepted end of file");

            // Parser automaton
            switch (lState) {
                case idle:
                    if (lChar == '#')
                        lState = comment;
                    else if (!isspace(lChar)) {
                        lState = token;
                        lBuffer << lChar;
                    }
                    break;

                case comment:
                    if (lChar == '\n')
                        lState = idle;
                    break;

                case token:
                    if (isspace(lChar) || (lChar == '#')) {
                        if (lChar == '#')
                            lState = comment;
                        else
                            lState = idle;

                        lTokenList.push_back(lBuffer.str());
                        lBuffer.str("");
                    } else
                        lBuffer << lChar;
                    break;
            }
        }

        // Parse the info from the tokens
        inX = loadPGM_parseInteger(lTokenList[0]);
        inY = loadPGM_parseInteger(lTokenList[1]);
        inFactor = loadPGM_parseInteger(lTokenList[2], 65535);
    }
} // namespace embryo

//Picture

void
embryo::loadPGM(istream& inStream, Picture & iPic) {
    // Check the PGM "magic number"
    std::string lMagicNumber;
    inStream >> lMagicNumber;

    bool lBinaryFormat = true;

    if (lMagicNumber == "P2")
        lBinaryFormat = false;
    else if (lMagicNumber != "P5") {
        cout << "lMagicNumber: ";
        cout << lMagicNumber;
        cout << " ; should be P5 !";
        throw Exception("PGM magic number not found");
    }

    // Read the picture informations
    size_t lX, lY, lFactor;
    loadPGM_infoChunk(inStream, lX, lY, lFactor);

    // Load the pixels data
    iPic.setup(lX, lY);

    if (lBinaryFormat)
        loadPGM_dataChunk_binary(inStream, iPic, lFactor);
    else
        loadPGM_dataChunk_ascii(inStream, iPic, lFactor);
}

void
embryo::savePGM(ostream& inStream, Picture& inPicture) {
    // PGM file format "magic number"
    inStream << "P5" << '\n';

    // Picture dimensions
    inStream << inPicture.width() << ' ' << inPicture.height() << '\n';

    // Discretization, 255 here
    inStream << 255 << '\n';

    size_t lNbPixels = inPicture.width() * inPicture.height();

    // The picture datas, binary form
    AspectCellIterator aciBegin;
    AspectCellIterator aciEnd;
    inPicture.getAspectCells(aciBegin, aciEnd);

    int pix = 0;
    int x, y, maxX, maxY;
    x = (*aciBegin)->getPositionX();
    y = (*aciBegin)->getPositionY();
    maxX = x + sqrt((*aciBegin)->getSize());
    maxY = y + sqrt((*aciBegin)->getSize());

    while (pix < lNbPixels) {
        int myX = pix % inPicture.width();
        int myY = pix / inPicture.height();

        if (!((myX >= x) && (myX < (x + sqrt((*aciBegin)->getSize()))) && (myY >= y) && (myY < (y + sqrt((*aciBegin)->getSize()))))) {
            inPicture.getAspectCells(aciBegin, aciEnd);
            while (aciBegin != aciEnd) {
                x = (*aciBegin)->getPositionX();
                y = (*aciBegin)->getPositionY();
                maxX = x + sqrt((*aciBegin)->getSize());
                maxY = y + sqrt((*aciBegin)->getSize());

                if ((myX >= x) && (myX < (x + sqrt((*aciBegin)->getSize()))) && (myY >= y) && (myY < (y + sqrt((*aciBegin)->getSize()))))
                    continue;
                else
                    ++aciBegin;
            }
        }

        int lByte = max(0, min(255, int(255.0f * (*aciBegin)->getAspect())));
        inStream.put(lByte);

        ++pix;
    }
}
