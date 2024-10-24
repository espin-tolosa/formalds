#include <stdio.h>
#include <math.h>

typedef float f32_t;
typedef unsigned int u32_t;
typedef _Bool bool_t;


#define MY_INF ( 1.0f / 0.0f )
#define MY_NAN ( MY_INF - MY_INF )

#define fp_MANTISSA 0x007FFFFF
#define fp_EXPONENT 0x7F800000
#define fp_SIGN		0x80000000

#define fp_ZERO     	0x00 /* 0xb0000 */
#define fp_POSFINITE	0x01 /* 0xb0001 */
#define fp_NEGFINITE	0x02 /* 0xb0010 */
#define fp_FINITE   	0x03 /* 0xb0011 */
#define fp_POSINF   	0x04 /* 0xb0100 */
#define fp_NEGINF   	0x08 /* 0xb1000 */
#define fp_INF			0x0C /* 0xb1100 */
#define fp_NAN      	0x0F /* 0xb1111 */

double my_exp_imp( double x );
double my_log_imp( double x );
f32_t my_exp_legacy( f32_t x );

typedef struct
{
	unsigned char is_zero;
	unsigned char is_inf;
	unsigned char is_nan;
	unsigned char is_finite;
			 char sign;
} fp_types_t;

typedef struct
{
    double f;
    double B;
    int e;
    int fsign;
    int esign;
} normalized_fp_t;

/*
 *	x = f * B^e
 */

normalized_fp_t normalize_fp( double x, double radix )
{
    normalized_fp_t result;
    result.B = radix;
	result.e = 0;

    if( x < 0.0f )
    {
        x = -x;
        result.fsign = -1;
    }
    else
    {
        result.fsign = +1;
    }

    if( x >= 1.0 )
    {
        while( x >= 1.0 )
        {
            x = x / result.B;
            result.e++;
        }

        result.f = x;
        result.esign = +1;
    }
    else if( 1.0 / result.B <= x && x < 1.0 )
    {
        result.f = x;
        result.e = 0;
        result.esign = +1;
    }
    else
    {
        result.esign = -1;
        while( x <= 1.0 / result.B )
        {
            x = x * result.B;
            result.e++;
        }

        result.f = x;
    }

    return result;
}

/* f = g * 2**k */

normalized_fp_t normalize_gp( double x )
{
	const double sqrt2 = 1.4142135623730950488016887242097;
	const double isqrt2 = 0.70710678118654752440084436210485;

    normalized_fp_t result;
    result.B = 2.0; /* 2^-n */
	result.e = 0;

    if( x < 0.0f )
    {
        x = -x;
        result.fsign = -1;
    }
    else
    {
        result.fsign = +1;
    }

    if( x >= sqrt2 )
    {
        while( x >= sqrt2 )
        {
            x = x / result.B;
            result.e++;
        }

        result.f = x;
        result.esign = -1;
    }
    else if( isqrt2 <= x && x < sqrt2 )
    {
        result.f = x;
        result.e = 0;
        result.esign = -1;
    }
    else
    {
        result.esign = +1;
        while( x <= isqrt2 )
        {
            x = x * result.B;
            result.e++;
        }

        result.f = x;
    }

    return result;
}

double my_abs( double x )
{
	return x < 0.0 ? -x : x;
}



fp_types_t get_type( float x )
{
	union
	{
		float 			x;
		unsigned int	u;
	} input;
	
	input.x = x;

	unsigned int get_mantissa	= input.u & fp_MANTISSA	;
	unsigned int get_exponent	= input.u & fp_EXPONENT	;
	unsigned int get_sign		= input.u & fp_SIGN		;

	fp_types_t result =
	{
		.is_zero = ( get_exponent == 0 )			&& ( get_mantissa == 0 ),
		.is_inf  = ( get_exponent == fp_EXPONENT ) 	&& ( get_mantissa == 0 ),
		.is_nan  = ( get_exponent == fp_EXPONENT )	&& ( get_mantissa != 0 ),
		.is_finite = get_exponent != fp_EXPONENT,
		.sign      = get_sign == 0 ? '+' : '-',
	};

	return ( result );
}

/*
 *	+Inf -> +Inf
 *	-Inf -> NAN
 *	NAN	 -> NAN
 *  Zero -> 0.0
 *  
 */

double my_sqrt_imp( double x )
{
	double yn = 0.5 * x;

	for(int i = 0; (i < 70); i++)
	{
		yn = 0.5 * ( yn + x / yn);
	}
		
	return yn;
}

double my_sqrt( double x )
{
	double result;

	fp_types_t xtype = get_type( x );

	if		( xtype.is_inf && xtype.sign == '+' )	result = MY_INF;
	else if	( xtype.sign == '-' 				)	result = MY_NAN;
	else if ( xtype.is_nan						)	result = MY_NAN;
	else 	  /* 0 < x <= MAX_FLOAT */				result = my_sqrt_imp( x );
	
	return result; /* casting can't overflow because sqrt(x) < x */
}

double my_log_imp( double x )
{
	const double ln2 = 0.69314718055994530941723212145818;

	normalized_fp_t xn = normalize_fp( x, 2.0 );
	normalized_fp_t gn = normalize_gp( xn.f );
	
	double f = gn.f;

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

	return ( ( xn.esign*xn.e - gn.esign*gn.e ) * ln2 + yn );
}

double my_exp( double x )
{
	double result;
	
	fp_types_t xtype = get_type( x );

	if		( xtype.is_inf && xtype.sign == '+' )	result =  MY_INF;
	else if	( xtype.is_inf && xtype.sign == '-' )	result =  0.0;
	else if ( xtype.is_nan						)	result =  MY_NAN;
	
	else 	  /* 0 < x <= MAX_FLOAT */				result =  my_exp_imp( x );

	return result;
}

/* x = g + k * ln2 */
normalized_fp_t normalize_exp( double x )
{
	normalized_fp_t gn;

	const double half_ln2	= 0.34657359027997265470861606072909; /* ln(2) / 2 */
	const double ln2		= 0.69314718055994530941723212145818; /* ln(2) */

    if( x < 0.0f )
    {
        x = -x;
        gn.fsign = -1;
    }
    else
    {
        gn.fsign = +1;
    }

	gn.f = x;
	gn.e = 0;
	gn.esign = 1;
	gn.B = ln2;
	
	while( my_abs(gn.f) > half_ln2 ) // f = 0.40 > 0.34
	{
		gn.e++;
		gn.f = x - gn.e * ln2;
	}
	
	return (gn);
}

/*
 * x = g + e * ln2; exp(x) = 2**e * exp(g)
 */

double my_exp_imp( double x )
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

/* x**y = e**(yln(x)) */

double my_pow_imp( double x, double y )
{
	const double w = y * my_log_imp( x ); /* TODO: replace by my_log( x ) */

	return ( my_exp( w ) );
}

f32_t my_exp_legacy( f32_t x )				// x 	= -1.0;
{
	bool_t neg = x < 0.0f;
	if( neg )
	{
		x = -x;
	}

	f32_t term		= x; 					// term = -1.0;
	u32_t i			= 2;					// i	= 2;
	f32_t result	= 1.0f + x;				// res 	= 0.0;
	bool_t tol		= term < 1.0E-5f;		// tol  = 
	
	while( !tol && ( i < 300 ) )
	{
		term 	= term / (f32_t) i;
		term 	= term * x;
		result 	= result + term;
		i 		= 1u + i;
		tol		= term < 1.0E-5f;
	}
	
	if( neg )
	{
		return ( 1.0 / result );
	}
	else
	{
		return ( result );
	}
}

int main ( void )
{
	float x;
	fp_types_t y;
	
	normalized_fp_t xn;
#if 1
	x = 1.e38; printf("LOG(%f): %f, %f\n",x, logf( x ), my_log_imp( x ));

	x = -0.0001123; xn = normalize_exp( x );
	printf("%f = %f + %d * ln(2) = %f\n", x, xn.f, xn.e, xn.fsign*(xn.e * log(2.0) + xn.f));

	x = 5.5; printf("EXP(%f): %f, %f\n",x, expf( x ), my_exp( x ));
	x = 10.0; printf("EXP(%f): %f, %f\n",x, expf( x ), my_exp( x ));
	x = 15.0; printf("EXP(%f): %f, %f\n",x, expf( x ), my_exp( x ));
	x = 20.0; printf("EXP(%f): %f, %f\n",x, expf( x ), my_exp( x ));
	x = 25.0; printf("EXP(%f): %f, %f\n",x, exp( x ), my_exp( x ));
	
	x = 25.0; printf("POW(%f, 7.0): %f, %f\n",x, pow( x, 7.0 ), my_pow_imp( x, 7.0 ));
	x = 50.0; printf("POW(%f, 7.0): %f, %f\n",x, pow( x, 7.0 ), my_pow_imp( x, 7.0 ));

	x = 25.0; printf("EXP(%f): %f, %f, diff = %f\n", x, exp( x ), my_exp_legacy( x ), fabs(exp( x ) - my_exp_legacy( x )) );
	x = 5.0; printf("EXP(%f) * EXP(-%f) = 1.0: exp(x) = %f, exp(-x) = %f, x= %f, diff = %f\n", x, x, exp( x ), exp( -x ), exp( x )*exp( -x ), fabs(1.0f - exp( x )*exp( -x )) );
	x = 5.0; printf("EXP(%f) * EXP(-%f) = 1.0: exp(x) = %f, exp(-x) = %f, x= %f, diff = %f\n", x, x, my_exp( x ), my_exp( -x ), my_exp( x )*my_exp( -x ), fabs(1.0f - my_exp( x )*my_exp( -x )) );
	x = 5.0; printf("EXP(%f) * EXP(-%f) = 1.0: exp(x) = %f, exp(-x) = %f, x= %f, diff = %f\n", x, x, my_exp_legacy( x ),my_exp_legacy( -x ), my_exp_legacy( x )*my_exp_legacy( -x ), fabs(1.0f - my_exp_legacy( x )*my_exp_legacy( -x )) );
#endif

#if 0
	printf("x,                 my_exp,         math_H,         my_legacy\n");
	for(int i = -100; i <= 100; i++)
	{
		double delta = 1.e-12;
		printf("%.12f, %.12f, %.12f, %.12f\n", (double) i + delta, (my_exp( (double) i + delta )), (exp( (double) i + delta )), (my_exp_legacy( (double) i + delta )) );
	}
#endif

#if 0	
	x = 0.0f; y = get_type( x ); printf("%.1f\tis\tZERO=%d,\tINF=%d,\tNAN=%d,\tFINITE=%d,\tSIGN=:%c1\n",x, y.is_zero, y.is_inf, y.is_nan, y.is_finite, y.sign  );
	x = 1.0f; y = get_type( x ); printf("%.1f\tis\tZERO=%d,\tINF=%d,\tNAN=%d,\tFINITE=%d,\tSIGN=:%c1\n",x, y.is_zero, y.is_inf, y.is_nan, y.is_finite, y.sign  );
	x = 1./0. - 1./0.; y = get_type( x ); printf("%.1f\tis\tZERO=%d,\tINF=%d,\tNAN=%d,\tFINITE=%d,\tSIGN=:%c1\n",x, y.is_zero, y.is_inf, y.is_nan, y.is_finite, y.sign  );
	x = -1./0.; y = get_type( x ); printf("%.1f\tis\tZERO=%d,\tINF=%d,\tNAN=%d,\tFINITE=%d,\tSIGN=:%c1\n",x, y.is_zero, y.is_inf, y.is_nan, y.is_finite, y.sign  );
	x = 0./0.; y = get_type( x ); printf("%.1f\tis\tZERO=%d,\tINF=%d,\tNAN=%d,\tFINITE=%d,\tSIGN=:%c1\n",x, y.is_zero, y.is_inf, y.is_nan, y.is_finite, y.sign  );
	x = +1./0.; y = get_type( x ); printf("%.1f\tis\tZERO=%d,\tINF=%d,\tNAN=%d,\tFINITE=%d,\tSIGN=:%c1\n",x, y.is_zero, y.is_inf, y.is_nan, y.is_finite, y.sign  );

	x = 1.0/0.0; printf("SQRT(%f): %f, %f\n",x, sqrtf( x ), my_sqrt( x ));
	x = 0.1; printf("SQRT(%f): %f, %f\n",x, sqrtf( x ), my_sqrt( x ));
	x = 1.E38; printf("SQRT(%f): %f, %f\n",x, sqrtf( x ), my_sqrt( x ));
	x = 1.E14; printf("SQRT(%f): %f, %f\n",x, sqrtf( x ), my_sqrt( x ));
	
	x = 4.5; printf("EXP(%f): %f, %f\n",x, expf( x ), my_exp( x ));
#endif
	return 0;
}

#if 0
void check_differences( float * expected, float * input, float * given, int len )
{
	for( int i = 0; i < len; ++i )
	{
		if( isnan(expected[i]) && isnan(given[i]) )
		{
			continue; /* break the iteration */
		}
		
		if( isinf(expected[i]) && isinf(given[i]) )
		{
			continue; /* break the iteration */
		}
			
#endif

// x = f * B^e, 2 -> 2/2 = 1/2 = 0.5 * 2^2 = 0.5 * 2 * 2
