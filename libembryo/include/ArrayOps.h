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

#ifndef EMBRYO_ARRAY_OPS_H
#define EMBRYO_ARRAY_OPS_H

#include <cstdlib>
#include <iostream>



namespace embryo {
  template <typename Real>
  class array {
  public:
    // Fill an array with 0
    static void zero(Real* inArray, size_t inSize);

    // Fill an array with 1
    static void ones(Real* inArray, size_t inSize);
    
    // Fill an array with a given value
    static void fill(Real* inArray, size_t inSize, double inValue);

	// Fill an array border according to the number of picture and its order
	static void fillBorder(Real* inArray, size_t inSize, size_t inValue, size_t inPicsNumber);

    // Fill an array with random values
    static void fillRandomly(Real* inArray, size_t inSize);

    // Array copy
    static void copy(Real* outDst, const Real* inSrc, size_t inSize);

    // Multiply all the array values by a given value
    static void scale(Real* inArray, size_t inSize, Real inValue);

    // Return the sum of all the array values
    static Real sum(const Real* inArray, size_t inSize);

    // Sort the array, with the QuickSort algorithm
    static void quicksort(Real* inArray, size_t inSize, bool inIncOrder);

    // Load an array from a file, plain ASCII (Throws an embryo::Exception)
    static void load(std::istream& inStream, Real* inArray, size_t inSize);

    // Save an array from a file, plain ASCII
    static void save(std::ostream& inStream, const Real* inArray, size_t inSize);

    // Return the minimum value of the array
    static Real minValue(const Real* inArray, size_t inSize);

    // Return the index of the minimum value of the array
    static size_t minIndex(const Real* inArray, size_t inSize);

    // Return the maximum value of the array
    static Real maxValue(const Real* inArray, size_t inSize);

    // Return the index of the maximum value of the array
    static size_t maxIndex(const Real* inArray, size_t inSize);

    //return true if any value in array is true
    static Real isAnyTrue(const Real* inArray, size_t inSize);

    // Return the standart deviation of the array values
    static Real stdDev(const Real* inArray, size_t inSize);

    // Return the mean and the standart deviation of the array values
    static void computeStats(const Real* inArray, size_t inSize,
                             Real& outMean, Real& outStdDev);
  }
  ; // class array



  typedef array<float> arrayf;
  typedef array<double> arrayd;
  typedef array<bool> arrayb;
} // namespace embryo



#endif /* EMBRYO_ARRAY_OPS_H */
