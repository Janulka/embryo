//test
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

#include <stdio.h>
#include <cmath>

#include <ArrayOps.h>
#include <Exception.h>
#include <Randomizer.h>

using namespace std;
using namespace embryo;

template <typename Real>
void
array<Real>::zero(Real* inArray, size_t inSize) {
    Real* lOffset = inArray;
    for (size_t i = inSize; i != 0; ++lOffset, --i)
        *lOffset = (Real) 0.0;
}

template <typename Real>
void
array<Real>::ones(Real* inArray, size_t inSize) {
    Real* lOffset = inArray;
    for (size_t i = inSize; i != 0; ++lOffset, --i)
        *lOffset = (Real) 1.0;
}

template <typename Real>
void
array<Real>::fill(Real* inArray, size_t inSize, double inValue) {
    Real* lOffset = inArray;
    for (size_t i = inSize; i != 0; ++lOffset, --i)
        *lOffset = (Real) inValue;
}

template <typename Real>
void array<Real>::fillRandomly(Real* inArray, size_t inSize) {
    Real* lOffset = inArray;
    Randomizer inRandomizer;
    inRandomizer.init(inSize);
    for (size_t i = inSize; i != 0; ++lOffset, --i) {
        *lOffset = inRandomizer.real();
    }
}

template <typename Real>
void
array<Real>::fillBorder(Real* inArray, size_t inSize, size_t inValue, size_t inPicsNumber) {
    if (inPicsNumber < 2)
        return;
    Real* lOffset = inArray;
    for (size_t i = inSize; i != inSize / (inPicsNumber - 1) * inValue; ++lOffset, --i) {
        *lOffset = (Real) 1.0;
        //cout << "i: " << i << = 1 << endl;
    }
    for (size_t i = inSize / (inPicsNumber - 1) * inValue; i != 0; ++lOffset, --i) {
        *lOffset = (Real) 0.0;
        //cout << "i: " << i << " = 0 " << endl;
    }
}


/*
template <typename Real>
void
array<Real>::fillBorder(Real* inArray, size_t inSize, size_t inValue, size_t inPicsNumber) {
  Real* lOffset = inArray;
  for(size_t i = inSize; i != 0; ++lOffset, --i) {
        cout << "(i % inPicsNumber): " << i % inSize << " inValue : " <<  inValue << endl;
        if ((i % inPicsNumber) == inValue) {
                cout << "i: " << i << " = 1 " << endl;
 *lOffset = (Real)1.0;
        }
        else {
                cout << "i: " << i << " = 0 " << endl;
 *lOffset = (Real)0.0;
        }
        }
}
 */

/*
template <typename Real>
void
array<Real>::fillBorder(Real* inArray, size_t inSize, size_t inValue, size_t inPicsNumber) {
  Real* lOffset = inArray;
        size_t oneEight = inSize/(inPicsNumber*2);
//cout << "inSize : " << inSize << endl;
//cout << "oneEighT : " << oneEight << endl;
//cout << "oneEight*inValue*2+oneEight : " << oneEight*inValue*2+oneEight << endl;
//cout << "oneEight*inValue*2-oneEight : " << oneEight*inValue*2-oneEight << endl;
//	cout << "oneEighT : " << oneEight << endl; 
        if (inValue == 0) {
//		cout << "zero pic : " << inValue << endl;
                for(size_t i = inSize; i != oneEight*(inPicsNumber*2-1); ++lOffset, --i) {
//            		cout << "i: " << i << " = 1 " << endl;
 *lOffset = (Real)1.0;
                }
                for(size_t i = oneEight*(inPicsNumber*2-1); i != oneEight; ++lOffset, --i) {
 //                       cout << "i: " << i << " = 0 " << endl;
 *lOffset = (Real)0.0;
                }
                for (size_t i = oneEight; i != 0; ++lOffset, --i) {
  //              	cout << "i: " << i << " = 1 " << endl; 
 *lOffset = (Real)1.0;
                }
        } else {
//		cout << "pic no.: " << inValue << endl;
                for(size_t i = inSize; i != oneEight*inValue*2+oneEight; ++lOffset, --i) {
   //                     cout << "i: " << i << " = 0 " << endl;
 *lOffset = (Real)0.0;
                }
                for (size_t i = oneEight*inValue*2+oneEight; i != oneEight*inValue*2-oneEight; ++lOffset, --i) {
 //	                 cout << "i: " << i << " = 1 " << endl;
 *lOffset = (Real)1.0;
                }
                for(size_t i = oneEight*inValue*2-oneEight; i != 0; ++lOffset, --i) {
 //		         cout << "i: " << i << " = 0 " << endl;
 *lOffset = (Real)0.0;
                }
        }
}
 */


template <typename Real>
void
array<Real>::copy(Real* outDst, const Real* inSrc, size_t inSize) {
    Real* lDst = outDst;
    const Real* lSrc = inSrc;
    for (size_t i = inSize; i != 0; ++lSrc, ++lDst, --i) {
        *lDst = *lSrc;
    }
}

template <typename Real>
void
array<Real>::scale(Real* inArray, size_t inSize, Real inValue) {
    Real* lOffset = inArray;
    for (size_t i = inSize; i != 0; ++lOffset, --i)
        *lOffset *= inValue;
}



namespace embryo {

    template <typename Real>
    int IncComparator(const void* inX, const void* inY) {
        const Real& lX = *((const Real*) inX);
        const Real& lY = *((const Real*) inY);

        if (lX < lY)
            return -1;
        if (lX == lY)
            return 0;
        else
            return 1;
    }

    template <typename Real>
    int DecComparator(const void* inX, const void* inY) {
        const Real& lX = *((const Real*) inX);
        const Real& lY = *((const Real*) inY);

        if (lX < lY)
            return 1;
        if (lX == lY)
            return 0;
        else
            return -1;
    }
} // namespace embryo

template <typename Real>
void
array<Real>::quicksort(Real* inArray, size_t inSize, bool inIncOrder) {
    if (inIncOrder)
        qsort(inArray, inSize, sizeof (Real), IncComparator<Real>);
    else
        qsort(inArray, inSize, sizeof (Real), DecComparator<Real>);
}

template <typename Real>
void
array<Real>::load(std::istream& inStream,
        Real* inArray, size_t inSize) {
    inStream.exceptions(ios::failbit | ios::badbit);
    try {
        for (size_t i = 0; i < inSize; ++i)
            inStream >> inArray[i];
    } catch (exception& inException) {
        throw Exception(inException.what());
    }
}

template <typename Real>
void
array<Real>::save(std::ostream& inStream,
        const Real* inArray, size_t inSize) {
    inStream.precision(15);
    inStream.setf(ios::scientific);

    inStream << inArray[0];
    for (size_t i = 1; i < inSize; ++i)
        inStream << ' ' << inArray[i];
    inStream << endl;
}

template <typename Real>
Real
array<Real>::maxValue(const Real* inArray, size_t inSize) {
    Real lMax = inArray[0];
    const Real* lOffset = inArray + 1;

    for (size_t i = inSize - 1; i != 0; ++lOffset, --i)
        if ((*lOffset) > lMax)
            lMax = *lOffset;

    return lMax;
}

template <typename Real>
size_t
array<Real>::maxIndex(const Real* inArray, size_t inSize) {
    size_t lMaxIndex = 0;
    Real lMax = inArray[lMaxIndex];
    const Real* lOffset = inArray + 1;

    for (size_t i = 1; i < inSize; ++lOffset, ++i)
        if ((*lOffset) > lMax) {
            lMax = *lOffset;
            lMaxIndex = i;
        }

    return lMaxIndex;
}

template <typename Real>
Real
array<Real>::minValue(const Real* inArray, size_t inSize) {
    Real lMin = inArray[0];
    const Real* lOffset = inArray + 1;

    for (size_t i = inSize - 1; i != 0; ++lOffset, --i)
        if ((*lOffset) < lMin)
            lMin = *lOffset;

    return lMin;
}

template <typename Real>
size_t
array<Real>::minIndex(const Real* inArray, size_t inSize) {
    size_t lMinIndex = 0;
    Real lMin = inArray[lMinIndex];
    const Real* lOffset = inArray + 1;

    for (size_t i = 1; i < inSize; ++lOffset, ++i)
        if ((*lOffset) < lMin) {
            lMin = *lOffset;
            lMinIndex = i;
        }

    return lMinIndex;
}

template <typename Real>
Real
array<Real>::sum(const Real* inArray, size_t inSize) {
    Real lSum = inArray[0];
    const Real* lOffset = inArray + 1;

    for (size_t i = inSize - 1; i != 0; ++lOffset, --i)
        lSum += *lOffset;

    return lSum;
}

template <typename Real>
Real
array<Real>::isAnyTrue(const Real* inArray, size_t inSize) {
    for (size_t i = 0; i < inSize; ++i) {
        if (inArray[i])
            return true;
    }
    return false;
}

template <typename Real>
Real
array<Real>::stdDev(const Real* inArray, size_t inSize) {
    Real lMean, lStdDev;
    array<Real>::computeStats(inArray, inSize, lMean, lStdDev);
    return lStdDev;
}

template <typename Real>
void
array<Real>::computeStats(const Real* inArray, size_t inSize,
        Real& outMean, Real& outStdDev) {
    Real lMeanEstimate = array<Real>::sum(inArray, inSize) / inSize;

    /*
     * Algorithm to compute the variance from D. Knuth, much more reliable
     * than the direct implementation of the variance mathematical definition.
     *
     * The Art of Computer Programming, volume 2: Seminumerical Algorithms,
     * 3rd edn., p. 232. Boston: Addison-Wesley.
     */

    size_t n = 0;
    Real lMean = 0;
    Real lM2 = 0;

    for (size_t i = 0; i < inSize; ++i) {
        n = n + 1;
        Real lX = inArray[i] - lMeanEstimate;
        Real lDelta = lX - lMean;
        lMean = lMean + lDelta / n;
        lM2 = lM2 + lDelta * (lX - lMean);
    }

    outMean = lMeanEstimate;
    outStdDev = sqrt(lM2 / (n - 1));
}



// --- template instanciation -------------------------------------------------

template class array<bool>;
template class array<float>;
template class array<double>;
