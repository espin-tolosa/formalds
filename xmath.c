/*
 * Model of Float and Double representation in a little endian machine
 *
 * FLOAT______________________________________________
 *
 * |  31 | 30      23 | 22                         0 |
 * |  S  | EEEE EEEE  | MMMM MMMM MMMM MMMM MMMM MMM |
 *
 * S = Sign bit (1 bit), offset 31
 * E = Exponent (8 bits), offsets 30-23
 * M = Mantissa (23 bits), offsets 22-0
 * 
 * DOUBLE______________________________________________________________________________________
 *
 * |  63 | 62          52 | 51                                                             0 |
 * |  S  | EEEE EEEE EEE  | MMMM MMMM MMMM MMMM MMMM MMMM MMMM MMMM MMMM MMMM MMMM MMMM MMMM |
 *
 * S = Sign bit (1 bit), offset 63
 * E = Exponent (11 bits), offsets 62-52
 * M = Mantissa (52 bits), offsets 51-0
 *
 * Unions
 *
 * FLOAT N/A
 *
 * DOUBLE______________________________________________________________________________________
 *
 * Unsigned short [4] representation:
 *
 * A double can be accessed as an array of four 16 bits values.
 *
 * |SEEEEEEEEEEEMMMM|MMMMMMMMMMMMMMMM|MMMMMMMMMMMMMMMM|MMMMMMMMMMMMMMMM|
 * |    16  bits    |    16  bits    |    16  bits    |    16  bits    |
 * |    D0 = 3      |    D1 = 2      |    D2 = 1      |    D3 = 0      |
 *                                                                   LSB
 *
 * Handling gradual underflow
 *
 * Gradual underflow is designed to avoid from abrupt underflow.
 *
 *         Min. Subnorm. , Min. Normal
 * Float : 1.4 x 10-45   , 1.2 x 10-38
 * Double: 4.9 x 10-324  , 2.2 x 10-308 
 */

/*
 * Definitions:
 *
 * Zero
 *		A value that sets bits [62:0] to zero. The sign bit (bit 63) is optional.
 *
 * Inf
 *		A value that sets all bits of the exponent field to one and all other bits to zero. The sign bit is optional.
 *
 * NaN 
 *		A value that sets all bits of the exponent field to one and at least one bit of the significand field to one. The sign bit is optional.
 *
 * Finite
 *		Any value that does not fall into the categories of Zero, Inf, or NaN.
 */

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

static const double invsqrt2 = 0.70710678118654752440084436210485;

#define W0   	3
#define W1   	2
#define W2   	1
#define W3   	0

#define DOFF 	4 		/* Number of fractional bits (M) in D0 */
#define INF  	0x7FF0 /* d_type INF, Sets to 1 all the exponential bits (E) in D0 */
#define NAN  	0x7FFF /* d_type NAN, Same as INF plus the four fractional bits (M) in D0 */
#define DMASK 	0x7FF0 /* A mask for the exponential bits (E) in D0 */
#define DMAX    0x07FF /* Counts the 11 bits of the exponent */ 
#define DFRAC	0x000F /* Counts the 4 bits of the fractional bits (M) in D0 */
#define FINITE	0x8000 /* Identifies a number which is either not zero, inf or nan */
#define SMMASK  0x800F /* A mask for the sign (S) in D0 plus the four fractinal bits (M) in D0 */
#define ZERO    0x0000 /* d_type Zero */
#define GRADZ   0x0001 /* d_Type Gradual underflow */
#define DBIAS   0x03FE /* The value emax−1 is subtracted from the bits of the exponent of a double to obtain the fraction 0.FFF... */
#define ONEHALF 0x3FE0 /* (DBIAS << DOFF) */
#define SMASK   0x8000 /* A mask for the sign in D0 */
#define MMASK   0x000F /* Amas for the four fractional bits (M) in D0 */

typedef union
{
	u16_t	w[ 4 ];
	u64_t   u;
	f64_t	d;
} dw_t;

#include <stdio.h>
#include <assert.h>

u16_t d_type ( double x )
{
	u16_t result;

	const dw_t 		px 					= { .d = x };
	const i16_t 	exponent			= ( px.w[ W0 ] & DMASK ) >> DOFF;
	const bool_t 	mantisa_not_zero	= ( px.w[ W0 ] & DFRAC ) || px.w[ W1 ] || px.w[ W2 ] || px.w[ W3 ];

	if ( exponent == DMAX )
	{
		if ( mantisa_not_zero )
		{
			result = NAN;			/* exponent == DMAX	, mantissa != 0 */
		}
		else
		{
			result = INF;			/* exponent == DMAX	, mantissa == 0 */
		}
	}
	
	else
	{
		if ( exponent != 0 )
		{
			result = FINITE;		/* exponent != 0	, mantissa N/A,  */
		}
		
		else if ( mantisa_not_zero )
		{
			result = GRADZ;			/* exponent == 0	, mantissa != 0 */
		}

		else
		{
			result = ZERO;			/* exponent == 0	, mantissa == 0 */
		}
	}

	return (result);
}

/*
 * |SEEEEEEEEEEEMMMM|MMMMMMMMMMMMMMMM|MMMMMMMMMMMMMMMM|MMMMMMMMMMMMMMMM|
 * |    16  bits    |    16  bits    |    16  bits    |    16  bits    |
 * |    D0 = 3      |    D1 = 2      |    D2 = 1      |    D3 = 0      |
 */

typedef struct
{
	dw_t	f;
	i16_t 	e;
	u16_t	type;
} dnorm_t;

/*
 * NORMALIZE
 * x = f * 2^e with 0.5 <= |f| < 1
 */
 
dnorm_t d_normalize ( double x )
{
	dnorm_t result = { 0 };

	result.type 		= d_type( x );
	result.f.w[ W0 ] 	= result.type;

	if( result.type == FINITE )
	{
		const dw_t px = { .d = x };

		result.e = ( ( px.w[ W0 ] & DMASK ) >> DOFF ) - DBIAS;

		result.f.w[ W0 ] = (px.w[ W0 ] & SMMASK) | ONEHALF ;
		result.f.w[ W1 ] = px.w[ W1 ];
		result.f.w[ W2 ] = px.w[ W2 ];
		result.f.w[ W3 ] = px.w[ W3 ];
	}

	else if ( result.type == GRADZ )
	{
		const dw_t px = { .d = x };

		result.f.w[ W0 ] = (px.w[ W0 ] & MMASK);
		result.f.w[ W1 ] = px.w[ W1 ];
		result.f.w[ W2 ] = px.w[ W2 ];
		result.f.w[ W3 ] = px.w[ W3 ];

		u64_t mantissa = result.f.u;

		result.e = -DBIAS;
	
		i32_t found = 0;

		for( i32_t i = 51; i >= 0 && !found; i = -1 + i )
		{
			found 		= (mantissa & (1ULL << i)) != 0;

			result.e 	= result.e - 1;
			mantissa 	= mantissa << 1;
		}

		result.e 			= result.e - 1;
		result.f.u 			= ( mantissa & 0xFFFFFFFFFFFFF ) | ((u64_t)(px.w[ W0 ] & SMASK) | ONEHALF) << 48;
	}
	
	return (result);
}

/*
 * GRADUAL UNDERFLOW NORMALIZE
 * x = f * 2^e with 0.5 <= |f| < 1
 */

dnorm_t d_gnormalize ( double x )
{
	return ( (dnorm_t) { 0 } );
}

double my_sqrt_imp( double x )
{
	const double a = 0.42578;
	const double b = 0.57422;
	
	double yn = a + b * x; /* Lyusternik 1965, first approximation of sqrt(x) */

	yn = 0.5 * ( yn + x / yn);
	yn = 0.5 * ( yn + x / yn);
	yn = 0.5 * ( yn + x / yn);

	return yn;
}

double d_sqrt( double x )
{
	dnorm_t xn = d_normalize( x );
	
	dnorm_t result = { 0 };

	result.type 		= d_type( x );
	result.f.w[ W0 ] 	= result.type;

	if ( result.type == FINITE || result.type == GRADZ )
	{
		const float scale	= xn.e < 0 ? 0.5 : 2.0;

		int e 		= xn.e % 2 ? xn.e / 2 : ( 1 + xn.e ) / 2;
		
		e = e < 0 ? -e : e;

		result.f.d = my_sqrt_imp( xn.f.d );
		
		if( xn.e % 2 != 0 )
		{
			result.f.d = result.f.d * invsqrt2;
		}
		
		for(int i = 0; i < e; i++)
		{
			result.f.d = result.f.d * scale;
		}
	}
	
	return ( result.f.d );
}

/*
 * ------------------------------------------
 * |				EXP						|
 * ------------------------------------------
 */

/*
 * x = g + e * ln2; exp(x) = 2**e * exp(g)
 */

double d_exp_imp( double x )
{	
	int neg;
	
	if( x < 0.0 )
	{
		x = -x;
		neg = 1;
	}
	else
	{
		neg = 0;
	}

	/* Cody & Waite, Chapter 6, page 69 */
	const double p0 	= 0.25000000000000000000e+0;
	const double p1 	= 0.75753180159422776666e-2;
	const double p2 	= 0.31555192765684646356e-4;
	const double q0 	= 0.50000000000000000000e+0;
	const double q1 	= 0.56817302698551221787e-1;
	const double q2 	= 0.63121894374398503557e-3;
	const double q3 	= 0.75104028399870046114e-6;
	const double invln2 = 1.442695040888963407;
	const double c1 	= 22713.0 / 32768.0;
	const double c2 	= 1.428606820309417232e-6;

	/* x = g + e * ln2 */
	const int  		e = (int)(x * invln2 + 0.5);
	const double 	g = (x - (double)e * c1) - (double)e * c2; /* Cody & Waite, Chapter 6, page 68, machines with guard digit */

	const double z = g * g;
	
	const double gP = g * ( (p2 * z + p1) * z + p0 );
	const double Q  = (q3 * z + q2) * z + q1;
	const double R 	= 0.5 + gP / (Q * z + q0 - gP);

	double k2 = 1.0;
	
	for( int i = 0; i < (1 + e); ++i ) { k2 = 2.0 * k2;	}

	if( neg )
	{
		return 1.0 / ( R * k2 );
	}
	else
	{
		return R * k2;
	}
}

double d_exp( double x )
{
	dnorm_t xn = d_normalize( x );
	
	dnorm_t result = { 0 };

	result.type 		= d_type( x );
	result.f.w[ W0 ] 	= result.type;

	if		( result.type == INF && x < 0 )	result.f.w[ W0 ] = ZERO;
	
	else 	  /* emin < x <= MAX_FLOAT */				result.f.d =  d_exp_imp( x );

	return result.f.d;
}

/*
 * ------------------------------------------
 * |				LOG						|
 * ------------------------------------------
 */

double d_log_imp( double x )
{
	const double ln2 = 0.69314718055994530941723212145818;

	dnorm_t xn = d_normalize( x );
	dnorm_t gn = d_normalize( xn.f.d );

	dnorm_t result = { 0 };

	result.type 		= d_type( x );
	result.f.w[ W0 ] 	= result.type;
	
	double f = gn.f.d;

	double z = 2.0 * ( f - 1.0 ) / ( f + 1.0 );

	double yn = z;

	z = z * z * z;
	yn = yn + z / 12.0;
	
	z = z * z * z;
	yn = yn + z / 80.0;

	z = z * z * z;
	yn = yn + z / 448.0;
	
	z = z * z * z;
	yn = yn + z / 2304.0;
	
	z = z * z * z;
	yn = yn + z / 11264.0;

	z = z * z * z;
	yn = yn + z / 49811.282184751;

	return ( ( /*xn.esign * */xn.e - /*gn.esign* */gn.e ) * ln2 + yn );
}

double d_log( double x )
{
	return d_log_imp( x );
}

double sqrt(double x);

void test_sqrt( void )
{
	int total = 0;
	int equal = 0;
	
	double xmin = 0.5;
	double xmax = 1.0;
	double delta = 0.0000001;

	for(double x = xmin; x < xmax; x+= delta)
	{
		total++;

		volatile double sqrtx2  = d_sqrt( x * x );
		double res =  sqrtx2 - x;

		res = res < 0.0 ? -res : res;

		if ( res < 1.e-15 )
		{
			equal++;
		}
		else
		{
			printf("FAILED: sqrt(%.12f) = %.12f\n", x, sqrtx2); 
		}
	}

	assert( equal == total );
//	printf("SQRT(x)^2 x = [%f, %f), equal: %d of %d, (%f x100)\n", xmin, xmax, equal, total, ((double)equal/(double)total)*100.0);
}

void test_log( void )
{
	int total = 0;
	int equal = 0;
	
	double xmin = 16.0;
	double xmax = 240.0;
	double delta = 0.001;

	printf("TODO: tight up log tolerance\n");

	for(double x = xmin; x < xmax; x+= delta)
	{
		total++;

		double zero  = d_log( x * x ) - 2.0 * d_log( x );
		double res =  zero;

		res = res < 0.0 ? -res : res;

		if ( res < 1.e-2 )
		{
			equal++;
		}
		else
		{
			printf("FAILED: log(%.12f) = %.12f, zero = %.12f\n", x, d_log(x), res); 
		}
	}

	assert( equal == total );
//	printf("SQRT(x)^2 x = [%f, %f), equal: %d of %d, (%f x100)\n", xmin, xmax, equal, total, ((double)equal/(double)total)*100.0);
}

void test_exp( void )
{
	int total = 0;
	int equal = 0;
	
	double xmin = -0.35;
	double xmax = +0.35;
	double delta = 0.0000001;

	for(double x = xmin; x < xmax; x+= delta)
	{
		total++;

		volatile double one  = d_exp( x ) * d_exp( -x );
		double res =  one - 1.0;

		res = res < 0.0 ? -res : res;

		if ( res < 1.e-15 )
		{
			equal++;
		}
		else
		{
			printf("FAILED: exp(%.12f) = %.12f\n", x, d_exp(x)); 
		}
	}

	assert( equal == total );
//	printf("SQRT(x)^2 x = [%f, %f), equal: %d of %d, (%f x100)\n", xmin, xmax, equal, total, ((double)equal/(double)total)*100.0);
}

double pow(double x, double y);

int main( void )
{
	printf("Testing _test(x)\n");

	printf("TODO: review gradual underflow: sqrt(%.12e) = %.12e\n", 1.e-320, d_sqrt(1.e-320));

	test_sqrt();
	test_exp();
	test_log();

	double x;
	dnorm_t xn;

	x = -1.e-309; xn = d_normalize(x); printf("%.12e = %.12e x 2 ^ %d = %.12e\n", x, xn.f.d, xn.e, xn.f.d*pow(2.0, xn.e));
	
	x = 0.0; 			assert( ZERO 	== d_type(x));
	x = 1./0.; 			assert( INF 	== d_type(x));
	x = -1./0.; 		assert( INF 	== d_type(x));
	x = -1./-0.; 		assert( INF 	== d_type(x));
	x = 1./0. + 1./0.; 	assert( INF 	== d_type(x));
	x = 1.e-320; 		assert( GRADZ 	== d_type(x));
	x = 1./0. - 1./0.; 	assert( NAN 	== d_type(x));
	x = 0.0*(1./0.); 	assert( NAN 	== d_type(x));
	x = 1.0; 			assert( FINITE 	== d_type(x));
	x = -1.E12; 		assert( FINITE 	== d_type(x));
	x = 1. - 1.; 		assert( ZERO 	== d_type(x));

	x = +120.0; xn = d_normalize(x); assert( xn.f.d == +0.9375 && xn.e == 7 && xn.type == FINITE ); //printf("frac = %.12f, exp = %d\n", xn.f, xn.e);
	x = -120.0; xn = d_normalize(x); assert( xn.f.d == -0.9375 && xn.e == 7 && xn.type == FINITE );
	x = -0.01 ; xn = d_normalize(x); assert( xn.f.d == -0.64   && xn.e == -6 && xn.type == FINITE );
	x = 0.5e-10 ; xn = d_normalize(x); assert( xn.f.d == 0.8589934592 && xn.e == -34 && xn.type == FINITE );
	x = 1./0. ; xn = d_normalize(x); assert( xn.f.d == 1./0.   && xn.e == 0 && xn.type == INF );

	x = 4.9E-324;
	xn = d_normalize(x);

	return 0;
}

