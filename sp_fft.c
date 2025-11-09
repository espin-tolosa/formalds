#include "sp_private.h"

/*
 * Free FFT and convolution (C)
 *
 * Copyright (c) 2021 Project Nayuki. (MIT License)
 * https://www.nayuki.io/page/free-small-fft-in-multiple-languages
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * - The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 * - The Software is provided "as is", without warranty of any kind, express or
 *   implied, including but not limited to the warranties of merchantability,
 *   fitness for a particular purpose and noninfringement. In no event shall the
 *   authors or copyright holders be liable for any claim, damages or other
 *   liability, whether in an action of contract, tort or otherwise, arising from,
 *   out of or in connection with the Software or the use or other dealings in the
 *   Software.
 */

#define __USE_MISC

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static size_t reverse_bits(size_t val, int width);
static bool Fft_transformRadix2(double complex vec[], size_t n, bool inverse);

extern bool sp_fft ( double complex * vec, size_t n )
{
    return Fft_transformRadix2( vec, n, false );
}

extern bool sp_ifft ( double complex * vec, size_t n )
{
    return Fft_transformRadix2( vec, n, true );
}

static bool Fft_transformRadix2(double complex vec[], size_t n, bool inverse) {
	// Length variables
	int levels = 0;  // Compute levels = floor(log2(n))
	for (size_t temp = n; temp > 1U; temp >>= 1)
		levels++;
	if ((size_t)1U << levels != n)
		return false;  // n is not a power of 2

	// Trigonometric tables
	if (SIZE_MAX / sizeof(double complex) < n / 2)
		return false;
	double complex *exptable = malloc((n / 2) * sizeof(double complex));
	if (exptable == NULL)
		return false;
	for (size_t i = 0; i < n / 2; i++)
		exptable[i] = cexp((inverse ? 2 : -2) * M_PI * i / n * I);

	// Bit-reversed addressing permutation
	for (size_t i = 0; i < n; i++) {
		size_t j = reverse_bits(i, levels);
		if (j > i) {
			double complex temp = vec[i];
			vec[i] = vec[j];
			vec[j] = temp;
		}
	}

	// Cooley-Tukey decimation-in-time radix-2 FFT
	for (size_t size = 2; size <= n; size *= 2) {
		size_t halfsize = size / 2;
		size_t tablestep = n / size;
		for (size_t i = 0; i < n; i += size) {
			for (size_t j = i, k = 0; j < i + halfsize; j++, k += tablestep) {
				size_t l = j + halfsize;
				double complex temp = vec[l] * exptable[k];
				vec[l] = vec[j] - temp;
				vec[j] += temp;
			}
		}
		if (size == n)  // Prevent overflow in 'size *= 2'
			break;
	}

	free(exptable);
	return true;
}

static size_t reverse_bits(size_t val, int width) {
	size_t result = 0;
	for (int i = 0; i < width; i++, val >>= 1)
		result = (result << 1) | (val & 1U);
	return result;
}
