#include <stdbool.h>
#include <complex.h>
int printf(const char * format, ...);
double sqrt( double x );
double sin( double x );
double fabs( double x );

bool Fft_transformRadix2(double complex vec[], size_t n, bool inverse);

typedef _Bool		   		bool_t;

typedef unsigned char  		u8_t;
typedef signed   char  		i8_t;

typedef signed   short 		i16_t;
typedef unsigned short 		u16_t;

typedef unsigned int   		u32_t;
typedef signed   int   		i32_t;

typedef unsigned long long 	u64_t;
typedef signed   long long 	i64_t;

typedef float		   		f32_t;
typedef double         		f64_t;
typedef long double    		f128_t;

typedef struct
{
	f64_t re;
	f64_t im;
} complex_t;

typedef struct
{
	complex_t * record;
	u64_t      len;
} fatcomplex_t;

f64_t math_surface_similarity ( fatcomplex_t f1, fatcomplex_t f2 )
{
	f64_t f1_sum2 = 0.0;
	f64_t f2_sum2 = 0.0;
	f64_t fd_sum2 = 0.0;
	
	for ( u64_t i = 0; i < f1.len; ++i )
	{
		const f64_t f1_re = f1.record[ i ].re;
		const f64_t f1_im = f1.record[ i ].im;
		const f64_t f2_re = f2.record[ i ].re;
		const f64_t f2_im = f2.record[ i ].im;

		const f64_t f1_re2 = f1_re * f1_re;
		const f64_t f1_im2 = f1_im * f1_im;

		const f64_t f2_re2 = f2_re * f2_re;
		const f64_t f2_im2 = f2_im * f2_im;
		
		const f64_t fd_re2 = f1_re2 + f2_re2 - 2.0 * f1_re * f2_re;
		const f64_t fd_im2 = f1_im2 + f2_im2 - 2.0 * f1_im * f2_im;

		f1_sum2 = f1_sum2 + f1_re2 + f1_im2;
		f2_sum2 = f2_sum2 + f2_re2 + f2_im2;
		fd_sum2 = fd_sum2 + fd_re2 + fd_im2;
	}
	
	const f64_t f1_l2norm = sqrt( f1_sum2 );
	const f64_t f2_l2norm = sqrt( f2_sum2 );
	const f64_t fd_l2norm = sqrt( fd_sum2 );
	
	return ( 1.0 - ( fd_l2norm / ( f1_l2norm + f2_l2norm ) ) );
}		

#define _RE 1.0
#define _IM 2.0

#define COMPUTE_SURFACE( scale ) { \
	complex_t f1 [ 1 ] = { {_RE        , _IM        } }; \
	complex_t f2 [ 1 ] = { {_RE * scale, _IM * scale} }; \
	f64_t surf = math_surface_similarity( (fatcomplex_t){ .record = f1, .len = 1 }, (fatcomplex_t){ .record = f2, .len = 1 } ); \
	printf("%f, %.12f, %.12f\n", scale, surf, (1. - (fabs(1. - scale)/(1+scale) ))); \
}

complex_t f1[16] =
{
	{0               ,  0             }, 
	{1.498004601725	 , -7.530977693637},
	{-0.288537028635 , 0.696590007777 },
	{-0.236488255403 , 0.353929685838 },
	{-0.222614342654 , 0.222614342654 },
	{-0.216932372844 , 0.144949577408 },
	{-0.214217111147 , 0.088731632729 },
	{-0.212937210207 , 0.042355844585 },
	{-0.21255656167	 , 0              },
	{-0.212937210207 , -0.042355844585},
	{-0.214217111147 , -0.088731632729},
	{-0.216932372844 , -0.144949577408},
	{-0.222614342654 , -0.222614342654},
	{-0.236488255403 , -0.353929685838},
	{-0.288537028635 , -0.696590007777},
	{1.498004601725	 , 7.530977693637 },
};

complex_t f2[16] =
{
	{ 0	             , 0              },
	{-1.498004601725 , 7.530977693637 },
	{0.288537028635	 , -0.696590007777},
	{0.236488255403	 , -0.353929685838},
	{0.222614342654	 , -0.222614342654},
	{0.216932372844	 , -0.144949577408},
	{0.214217111147	 , -0.088731632729},
	{0.212937210207	 , -0.042355844585},
	{0.21255656167	 , 0              },
	{0.212937210207	 , 0.042355844585 },
	{0.214217111147	 , 0.088731632729 },
	{0.216932372844	 , 0.144949577408 },
	{0.222614342654	 , 0.222614342654 },
	{0.236488255403	 , 0.353929685838 },
	{0.288537028635	 , 0.696590007777 },
	{-1.498004601725 , -7.530977693637},
};


void compute_surface( void )
{
	COMPUTE_SURFACE(0.0);
	COMPUTE_SURFACE(0.1);
	COMPUTE_SURFACE(0.2);
	COMPUTE_SURFACE(0.3);
	COMPUTE_SURFACE(0.4);
	COMPUTE_SURFACE(0.5);
	COMPUTE_SURFACE(0.6);
	COMPUTE_SURFACE(0.7);
	COMPUTE_SURFACE(0.8);
	COMPUTE_SURFACE(0.9);

	COMPUTE_SURFACE(1.0);
	COMPUTE_SURFACE(1.1);
	COMPUTE_SURFACE(1.2);
	COMPUTE_SURFACE(1.3);
	COMPUTE_SURFACE(1.4);
	COMPUTE_SURFACE(1.5);
	COMPUTE_SURFACE(1.6);
	COMPUTE_SURFACE(1.7);
	COMPUTE_SURFACE(1.8);
	COMPUTE_SURFACE(1.9);

	COMPUTE_SURFACE(2.0);
	COMPUTE_SURFACE(2.1);
	COMPUTE_SURFACE(2.2);
	COMPUTE_SURFACE(2.3);
	COMPUTE_SURFACE(2.4);
	COMPUTE_SURFACE(2.5);
	COMPUTE_SURFACE(2.6);
	COMPUTE_SURFACE(2.7);
	COMPUTE_SURFACE(2.8);
	COMPUTE_SURFACE(2.9);

	COMPUTE_SURFACE(5.0);


	{ /* phase 0 */
		complex_t f1[16] =
		{
			{ 0               , 0               },
			{ 1.498004601725  , -7.530977693637 },
			{ -0.288537028635 , 0.696590007777  },
			{ -0.236488255403 , 0.353929685838  },
			{ -0.222614342654 , 0.222614342654  },
			{ -0.216932372844 , 0.144949577408  },
			{ -0.214217111147 , 0.088731632729  },
			{ -0.212937210207 , 0.042355844585  },
			{ -0.21255656167  , 0               },
			{ -0.212937210207 , -0.042355844585 },
			{ -0.214217111147 , -0.088731632729 },
			{ -0.216932372844 , -0.144949577408 },
			{ -0.222614342654 , -0.222614342654 },
			{ -0.236488255403 , -0.353929685838 },
			{ -0.288537028635 , -0.696590007777 },
			{ 1.498004601725  , 7.530977693637  },
		};

		complex_t f2[16] =
		{
			{ 0               , 0               },
			{ 1.498004601725  , -7.530977693637 },
			{ -0.288537028635 , 0.696590007777  },
			{ -0.236488255403 , 0.353929685838  },
			{ -0.222614342654 , 0.222614342654  },
			{ -0.216932372844 , 0.144949577408  },
			{ -0.214217111147 , 0.088731632729  },
			{ -0.212937210207 , 0.042355844585  },
			{ -0.21255656167  , 0               },
			{ -0.212937210207 , -0.042355844585 },
			{ -0.214217111147 , -0.088731632729 },
			{ -0.216932372844 , -0.144949577408 },
			{ -0.222614342654 , -0.222614342654 },
			{ -0.236488255403 , -0.353929685838 },
			{ -0.288537028635 , -0.696590007777 },
			{ 1.498004601725  , 7.530977693637  },
		};

		f64_t surf = math_surface_similarity( (fatcomplex_t){ .record = f1, .len = 16 }, (fatcomplex_t){ .record = f2, .len = 16 } );
		printf("%f, %.12f\n", 0.0, surf);
	}

	{ /* phase 22.5 */
		complex_t f1[16] =
		{
			{	0	            ,	0	            },
			{	1.498004601725	,	-7.530977693637	},
			{	-0.288537028635	,	0.696590007777	},
			{	-0.236488255403	,	0.353929685838	},
			{	-0.222614342654	,	0.222614342654	},
			{	-0.216932372844	,	0.144949577408	},
			{	-0.214217111147	,	0.088731632729	},
			{	-0.212937210207	,	0.042355844585	},
			{	-0.21255656167	,	0	            },
			{	-0.212937210207	,	-0.042355844585	},
			{	-0.214217111147	,	-0.088731632729	},
			{	-0.216932372844	,	-0.144949577408	},
			{	-0.222614342654	,	-0.222614342654	},
			{	-0.236488255403	,	-0.353929685838	},
			{	-0.288537028635	,	-0.696590007777	},
			{	1.498004601725	,	7.530977693637	},
		};

		complex_t f2[16] =
		{
			{	0.382683432365	,	0	            },
			{	4.463642737836	,	-6.345132307843	},
			{	-0.403367472034	,	0.586903313683	},
			{	-0.261572897295	,	0.298199088575	},
			{	-0.223776702764	,	0.187560966879	},
			{	-0.208297519627	,	0.122125477465	},
			{	-0.200900431209	,	0.074759742023	},
			{	-0.197413643755	,	0.035686394096	},
			{	-0.196376656828	,	0	            },
			{	-0.197413643755	,	-0.035686394096	},
			{	-0.200900431209	,	-0.074759742023	},
			{	-0.208297519627	,	-0.122125477465	},
			{	-0.223776702764	,	-0.187560966879	},
			{	-0.261572897295	,	-0.298199088575	},
			{	-0.403367472034	,	-0.586903313683	},
			{	4.463642737836	,	6.345132307843	},
		};

		f64_t surf = math_surface_similarity( (fatcomplex_t){ .record = f1, .len = 16 }, (fatcomplex_t){ .record = f2, .len = 16 } );
		printf("%f, %.12f\n", 22.5, surf);
	}

	{ /* phase 45 */
		complex_t f1[16] =
		{
			{	0	            ,	0	            },
			{	1.498004601725	,	-7.530977693637	},
			{	-0.288537028635	,	0.696590007777	},
			{	-0.236488255403	,	0.353929685838	},
			{	-0.222614342654	,	0.222614342654	},
			{	-0.216932372844	,	0.144949577408	},
			{	-0.214217111147	,	0.088731632729	},
			{	-0.212937210207	,	0.042355844585	},
			{	-0.21255656167	,	0	            },
			{	-0.212937210207	,	-0.042355844585	},
			{	-0.214217111147	,	-0.088731632729	},
			{	-0.216932372844	,	-0.144949577408	},
			{	-0.222614342654	,	-0.222614342654	},
			{	-0.236488255403	,	-0.353929685838	},
			{	-0.288537028635	,	-0.696590007777	},
			{	1.498004601725	,	7.530977693637	},
		};

		complex_t f2[16] =
		{
			{	0.707106781187	,	0	            },
			{	6.749731730135	,	-4.193298046946	},
			{	-0.456788874352	,	0.387865910372	},
			{	-0.246835436738	,	0.197070383257	},
			{	-0.190871088419	,	0.123953134141	},
			{	-0.167951257268	,	0.080708880649	},
			{	-0.156998481787	,	0.049406358293	},
			{	-0.151835639601	,	0.023584013604	},
			{	-0.150300186143	,	0	            },
			{	-0.151835639601	,	-0.023584013604	},
			{	-0.156998481787	,	-0.049406358293	},
			{	-0.167951257268	,	-0.080708880649	},
			{	-0.190871088419	,	-0.123953134141	},
			{	-0.246835436738	,	-0.197070383257	},
			{	-0.456788874352	,	-0.387865910372	},
			{	6.749731730135	,	4.193298046946	},
		};

		f64_t surf = math_surface_similarity( (fatcomplex_t){ .record = f1, .len = 16 }, (fatcomplex_t){ .record = f2, .len = 16 } );
		printf("%f, %.12f\n", 45.0, surf);
	}

	{ /* phase 90 */
		complex_t f1[16] =
		{
			{	0	,	0	},
			{	1.498004601725	,	-7.530977693637	},
			{	-0.288537028635	,	0.696590007777	},
			{	-0.236488255403	,	0.353929685838	},
			{	-0.222614342654	,	0.222614342654	},
			{	-0.216932372844	,	0.144949577408	},
			{	-0.214217111147	,	0.088731632729	},
			{	-0.212937210207	,	0.042355844585	},
			{	-0.21255656167	,	0	},
			{	-0.212937210207	,	-0.042355844585	},
			{	-0.214217111147	,	-0.088731632729	},
			{	-0.216932372844	,	-0.144949577408	},
			{	-0.222614342654	,	-0.222614342654	},
			{	-0.236488255403	,	-0.353929685838	},
			{	-0.288537028635	,	-0.696590007777	},
			{	1.498004601725	,	7.530977693637	},
		};

		complex_t f2[16] =
		{
			{	1	            ,	0	            },
			{	8.047557553412	,	1.600758724573	},
			{	-0.357459992615	,	-0.148064776947	},
			{	-0.112589766906	,	-0.075230077095	},
			{	-0.047318139253	,	-0.047318139253	},
			{	-0.020586573003	,	-0.030809983789	},
			{	-0.007812271068	,	-0.018860490764	},
			{	-0.001790810568	,	-0.009003012692	},
			{	0	,	0	},
			{	-0.001790810568	,	0.009003012692	},
			{	-0.007812271068	,	0.018860490764	},
			{	-0.020586573003	,	0.030809983789	},
			{	-0.047318139253	,	0.047318139253	},
			{	-0.112589766906	,	0.075230077095	},
			{	-0.357459992615	,	0.148064776947	},
			{	8.047557553412	,	-1.600758724573	},
		};

		f64_t surf = math_surface_similarity( (fatcomplex_t){ .record = f1, .len = 16 }, (fatcomplex_t){ .record = f2, .len = 16 } );
		printf("%f, %.12f\n", 90.0, surf);
	}

	{ /* phase 180 */
		complex_t f1[16] =
		{
			{0               ,  0             }, 
			{1.498004601725	 , -7.530977693637},
			{-0.288537028635 , 0.696590007777 },
			{-0.236488255403 , 0.353929685838 },
			{-0.222614342654 , 0.222614342654 },
			{-0.216932372844 , 0.144949577408 },
			{-0.214217111147 , 0.088731632729 },
			{-0.212937210207 , 0.042355844585 },
			{-0.21255656167	 , 0              },
			{-0.212937210207 , -0.042355844585},
			{-0.214217111147 , -0.088731632729},
			{-0.216932372844 , -0.144949577408},
			{-0.222614342654 , -0.222614342654},
			{-0.236488255403 , -0.353929685838},
			{-0.288537028635 , -0.696590007777},
			{1.498004601725	 , 7.530977693637 },
		};

		complex_t f2[16] =
		{
			{ 0	             , 0              },
			{-1.498004601725 , 7.530977693637 },
			{0.288537028635	 , -0.696590007777},
			{0.236488255403	 , -0.353929685838},
			{0.222614342654	 , -0.222614342654},
			{0.216932372844	 , -0.144949577408},
			{0.214217111147	 , -0.088731632729},
			{0.212937210207	 , -0.042355844585},
			{0.21255656167	 , 0              },
			{0.212937210207	 , 0.042355844585 },
			{0.214217111147	 , 0.088731632729 },
			{0.216932372844	 , 0.144949577408 },
			{0.222614342654	 , 0.222614342654 },
			{0.236488255403	 , 0.353929685838 },
			{0.288537028635	 , 0.696590007777 },
			{-1.498004601725 , -7.530977693637},
		};

		f64_t surf = math_surface_similarity( (fatcomplex_t){ .record = f1, .len = 16 }, (fatcomplex_t){ .record = f2, .len = 16 } );
		printf("%f, %.12f\n", 180.0, surf);
	}

	{ /* phase 270 */
		complex_t f1[16] =
		{
			{	0	            ,	0	            },
			{	1.498004601725	,	-7.530977693637	},
			{	-0.288537028635	,	0.696590007777	},
			{	-0.236488255403	,	0.353929685838	},
			{	-0.222614342654	,	0.222614342654	},
			{	-0.216932372844	,	0.144949577408	},
			{	-0.214217111147	,	0.088731632729	},
			{	-0.212937210207	,	0.042355844585	},
			{	-0.21255656167	,	0	},
			{	-0.212937210207	,	-0.042355844585	},
			{	-0.214217111147	,	-0.088731632729	},
			{	-0.216932372844	,	-0.144949577408	},
			{	-0.222614342654	,	-0.222614342654	},
			{	-0.236488255403	,	-0.353929685838	},
			{	-0.288537028635	,	-0.696590007777	},
			{	1.498004601725	,	7.530977693637	},
		};

		complex_t f2[16] =
		{
			{	-1	,	0	},
			{	-8.047557553412	,	-1.600758724573	},
			{	0.357459992615	,	0.148064776947	},
			{	0.112589766906	,	0.075230077095	},
			{	0.047318139253	,	0.047318139253	},
			{	0.020586573003	,	0.030809983789	},
			{	0.007812271068	,	0.018860490764	},
			{	0.001790810568	,	0.009003012692	},
			{	0	,	0	},
			{	0.001790810568	,	-0.009003012692	},
			{	0.007812271068	,	-0.018860490764	},
			{	0.020586573003	,	-0.030809983789	},
			{	0.047318139253	,	-0.047318139253	},
			{	0.112589766906	,	-0.075230077095	},
			{	0.357459992615	,	-0.148064776947	},
			{	-8.047557553412	,	1.600758724573	},
		};

		f64_t surf = math_surface_similarity( (fatcomplex_t){ .record = f1, .len = 16 }, (fatcomplex_t){ .record = f2, .len = 16 } );
		printf("%f, %.12f\n", 270.0, surf);
	}
}

f64_t square_fourier( f64_t t, u32_t p )
{
	f64_t sum = 0;
	
	for( u32_t n = 0; n < p; n++ )
	{
		sum += (1.0 / (2. * n + 1.) ) * sin( (2.* n + 1.) * t );
	}
	
	return ( 1.2732395447351 * sum );
}

//bool Fft_transformRadix2(double complex vec[], size_t n, bool inverse);

f64_t surface_square( int p )
{
	complex y1[1024];
	complex y2[1024];

	complex_t f1[1024];
	complex_t f2[1024];

	for( u32_t i = 0; i < 1024; i++ )
	{
		const double t = (double)i * 6.283185307179 / 1023.;
		y1[i] = t < 3.14159265358 ? +1 : -1;
		y2[i] = square_fourier(t,p);
	}
	
	Fft_transformRadix2(y1, 1024, 0);
	Fft_transformRadix2(y2, 1024, 0);
	
	for( u32_t i = 0; i < 1024; i++ )
	{
		f1[i].re = creal(y1[i]);
		f1[i].im = cimag(y1[i]);

		f2[i].re = creal(y2[i]);
		f2[i].im = cimag(y2[i]);
	}

	f64_t surf = math_surface_similarity( (fatcomplex_t){ .record = f1, .len = 1024 }, (fatcomplex_t){ .record = f2, .len = 1024 } );
	
	return surf;
}

void test_surface_square( void )
{
	for(int p = 1; p < 100; p++)
	{
		printf("%f, %f\n", 1.0 - 1.0 / (3.14159265358*sqrt(2.*p)), surface_square(p));
	}
}

int main( void )
{
//	compute_surface();
	test_surface_square();

	return 0;
}


















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

#include <complex.h>
#include <stdbool.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#define restrict
#endif


/* 
 * Computes the discrete Fourier transform (DFT) of the given complex vector, storing the result back into the vector.
 * The vector can have any length. This is a wrapper function. The inverse transform does not perform scaling,
 * so it is not a true inverse. Returns true if successful, false otherwise (out of memory).
 */
bool Fft_transform(double complex vec[], size_t n, bool inverse);


/* 
 * Computes the discrete Fourier transform (DFT) of the given complex vector, storing the result back into the vector.
 * The vector's length must be a power of 2. Uses the Cooley-Tukey decimation-in-time radix-2 algorithm.
 * Returns true if successful, false otherwise (n is not a power of 2, or out of memory).
 */
bool Fft_transformRadix2(double complex vec[], size_t n, bool inverse);


/* 
 * Computes the discrete Fourier transform (DFT) of the given complex vector, storing the result back into the vector.
 * The vector can have any length. This requires the convolution function, which in turn requires the radix-2 FFT function.
 * Uses Bluestein's chirp z-transform algorithm. Returns true if successful, false otherwise (out of memory).
 */
bool Fft_transformBluestein(double complex vec[], size_t n, bool inverse);


/* 
 * Computes the circular convolution of the given complex vectors. Each vector's length must be the same.
 * Returns true if successful, false otherwise (out of memory).
 */
bool Fft_convolve(const double complex xvec[restrict], const double complex yvec[restrict], double complex outvec[restrict], size_t n);


#ifdef __cplusplus
#undef restrict
}
#endif





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

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Private function prototypes
static size_t reverse_bits(size_t val, int width);
static void *memdup(const void *src, size_t n);


bool Fft_transform(double complex vec[], size_t n, bool inverse) {
	if (n == 0)
		return true;
	else if ((n & (n - 1)) == 0)  // Is power of 2
		return Fft_transformRadix2(vec, n, inverse);
	else  // More complicated algorithm for arbitrary sizes
		return Fft_transformBluestein(vec, n, inverse);
}


bool Fft_transformRadix2(double complex vec[], size_t n, bool inverse) {
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


bool Fft_transformBluestein(double complex vec[], size_t n, bool inverse) {
	bool status = false;
	
	// Find a power-of-2 convolution length m such that m >= n * 2 + 1
	size_t m = 1;
	while (m / 2 <= n) {
		if (m > SIZE_MAX / 2)
			return false;
		m *= 2;
	}
	
	// Allocate memory
	if (SIZE_MAX / sizeof(double complex) < n || SIZE_MAX / sizeof(double complex) < m)
		return false;
	double complex *exptable = malloc(n * sizeof(double complex));
	double complex *avec = calloc(m, sizeof(double complex));
	double complex *bvec = calloc(m, sizeof(double complex));
	double complex *cvec = malloc(m * sizeof(double complex));
	if (exptable == NULL || avec == NULL || bvec == NULL || cvec == NULL)
		goto cleanup;
	
	// Trigonometric tables
	for (size_t i = 0; i < n; i++) {
		uintmax_t temp = ((uintmax_t)i * i) % ((uintmax_t)n * 2);
		double angle = (inverse ? M_PI : -M_PI) * temp / n;
		exptable[i] = cexp(angle * I);
	}
	
	// Temporary vectors and preprocessing
	for (size_t i = 0; i < n; i++)
		avec[i] = vec[i] * exptable[i];
	bvec[0] = exptable[0];
	for (size_t i = 1; i < n; i++)
		bvec[i] = bvec[m - i] = conj(exptable[i]);
	
	// Convolution
	if (!Fft_convolve(avec, bvec, cvec, m))
		goto cleanup;
	
	// Postprocessing
	for (size_t i = 0; i < n; i++)
		vec[i] = cvec[i] * exptable[i];
	status = true;
	
	// Deallocation
cleanup:
	free(exptable);
	free(avec);
	free(bvec);
	free(cvec);
	return status;
}


bool Fft_convolve(const double complex xvec[restrict], const double complex yvec[restrict],
		double complex outvec[restrict], size_t n) {
	
	bool status = false;
	if (SIZE_MAX / sizeof(double complex) < n)
		return false;
	double complex *xv = memdup(xvec, n * sizeof(double complex));
	double complex *yv = memdup(yvec, n * sizeof(double complex));
	if (xv == NULL || yv == NULL)
		goto cleanup;
	
	if (!Fft_transform(xv, n, false))
		goto cleanup;
	if (!Fft_transform(yv, n, false))
		goto cleanup;
	for (size_t i = 0; i < n; i++)
		xv[i] *= yv[i];
	if (!Fft_transform(xv, n, true))
		goto cleanup;
	for (size_t i = 0; i < n; i++)  // Scaling (because this FFT implementation omits it)
		outvec[i] = xv[i] / n;
	status = true;
	
cleanup:
	free(xv);
	free(yv);
	return status;
}


static size_t reverse_bits(size_t val, int width) {
	size_t result = 0;
	for (int i = 0; i < width; i++, val >>= 1)
		result = (result << 1) | (val & 1U);
	return result;
}


static void *memdup(const void *src, size_t n) {
	void *dest = malloc(n);
	if (n > 0 && dest != NULL)
		memcpy(dest, src, n);
	return dest;
}
