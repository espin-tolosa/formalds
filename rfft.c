#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <immintrin.h> // For AVX2
#include <complex.h>

typedef _Bool                   bool_t;

typedef unsigned char           u8_t;
typedef signed   char           i8_t;

typedef signed   short          i16_t;
typedef unsigned short          u16_t;

typedef unsigned int            u32_t;
typedef signed   int            i32_t;

typedef unsigned long long      u64_t;
typedef signed   long long      i64_t;
typedef __int128_t              i128_t;

typedef float                   f32_t;
typedef double                  f64_t;
typedef long double             f128_t;

#define W0                  (3)
#define W1                  (2)
#define W2                  (1)
#define W3                  (0)

#define EMAX                (1023)
#define EMIN                (1-EMAX)
#define DOFF                (4)                             /* Number of fractional bits (M) in W0 */
#define MOFF                (48)                            /* Number of fractional bits (M) in W1 to W3*/
#define DMASK               (0x7FF0)                        /* A mask for the exponential bits (E) in W0 */
#define EDMASK              (0x7FFF)                        /* A mask for the exponential bits (E) in W0 plus the fractional bits */
#define DMAX                (0x07FF)                        /* Counts the 11 bits of the exponent */
#define DFRAC               (0x000F)                        /* Counts the 4 bits of the fractional bits (M) in W0 */
#define SMMASK              (0x800F)                        /* A mask for the sign (S) in W0 plus the four fractinal bits (M) in W0 */
#define DBIAS               (0x03FE)                        /* The value emax−1 is subtracted from the bits of the exponent of a double to obtain the fraction 0.FFF... */
#define EXPHALF             (0x3FE0)                        /* Exponent bit pattern corresponding to the range [0.5, 1.0) */
#define EXPONE              (0x3FF0)                        /* Exponent bit pattern corresponding to the range [1.0, 2.0) */
#define SMASK               (0x8000)                        /* A mask for the sign in W0 */
#define MMASK               (0x000F)                        /* A mask for the four fractional bits (M) in W0 */
#define NBITS               (MOFF+DOFF)                     /* Number of significand bits, also called precission p*/
#define INF                 (0x7FF0)                        /* math_type +INF, Sets to 1 all the exponential bits (E) in W0 */
#define NINF                (INF | SMASK)                   /* math_type -INF, Sets to 1 all the exponential bits (E) in W0 and also SIGN bit */
#define NIL                 (0x7FFF)                        /* math_type NIL, Same as INF plus the four fractional bits (M) in W0 */
#define FINITE              (0x8000)                        /* Identifies a number which is either not zero, inf or nan */
#define ZERO                (0x0000)                        /* math_type Zero */
#define GRADZ               (0x0001)                        /* math_Type Gradual underflow */
#define DBL_EPS             (2.2204460492503131e-16)        /* Machine epsilon */
#define DBL_SQE             (1.50e-08)                      /* Square Root of Machine epsilon*/
#define MATH_SIN_RANGE      (1.e4)
#define MATH_EXP_LO_RANGE   (-744.03460681309024948859587)
#define MATH_EXP_HI_RANGE   (+709.782712893383973096)

typedef union
{
    u16_t   w[ 4 ];
    u64_t   u;
    f64_t   d;
} dw_t;

#define REAL_FFT

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct
{
    f64_t hi;
    f64_t lo;
}
    quad64_t;

static f64_t math_significand( f64_t x )
{
	dw_t result = { 0 };

    dw_t f = { .d = x };

	result.w[ W3 ] = f.w[ W3 ];
    result.w[ W2 ] = f.w[ W2 ];
    result.w[ W1 ] = f.w[ W1 ];
    result.w[ W0 ] = ( f.w[ W0 ] & SMMASK );

	return ( result.d );
}

extern f64_t math_cwsetexp ( f64_t x, i16_t n)
{
    dw_t result;

	i16_t abs_n;

	if( n < 0 )
	{
		abs_n = -n;
	}

	else
	{
		abs_n = +n;
	}

	if ( DMAX <= abs_n )
	{
		if( x < 0.0 )
		{
			result.u = 0;
			result.w[ W0 ] = INF | SMASK;
		}

		else
		{
			result.u = 0;
			result.w[ W0 ] = INF;
		}
	}

	else
	{
		result.d = math_significand( x );

		if( ( n + EMAX ) <= 0 )
		{
			result.u	= result.u | ( 0x8000000000000LLu >> ( (u32_t) ( -( n + EMAX ) ) ) );
			n 			= -EMAX;
		}

		result.w[ W0 ] = result.w[ W0 ] | ( ( n + EMAX ) << DOFF );
	}

    return ( result.d );
}

quad64_t math_veltkamp_split( f64_t x, i16_t s )
{
    quad64_t ret = { 0 };

    if( ( 2 <= s ) && ( s <= ( 53 /* p */ - 2 ) ) )
    {
        const f64_t c = 1.0 + math_cwsetexp( 1.0, s );

        const f64_t gamma   = c * x;
        const f64_t delta   = x - gamma;

        ret.hi              = gamma + delta;
        ret.lo              = x - ret.hi;
    }

    return ( ret );
}

quad64_t math_dekker_mult( f64_t x, f64_t y )
{
    const i16_t s       = 1 + 53/2; /* floor(p/2) */

    const quad64_t qx   = math_veltkamp_split( x, s );
    const quad64_t qy   = math_veltkamp_split( y, s );


    const f64_t r1      = x * y;
    const f64_t t1      = -r1 + ( qx.hi * qy.hi );
    const f64_t t2      = +t1 + ( qx.hi * qy.lo );
    const f64_t t3      = +t2 + ( qx.lo * qy.hi );
    const f64_t r2      = +t3 + ( qx.lo * qy.lo );

    return ( ( quad64_t ) { .hi = r1, .lo = r2 } );
}

quad64_t math_fast_sum( f64_t a, f64_t b )
{
    const f128_t s       = a + b;
    const f128_t a1      = s - b;
    const f128_t b1      = s - a1;
    const f128_t da      = a - a1;
    const f128_t db      = b - b1;
    const f128_t t       = da + db;

    return ( (quad64_t) { .hi = s, .lo = t } );
}

complex nroot_unity( u32_t n, u32_t N )
{
    return ( cos( -2.0 * M_PI * n / N ) + I * sin( -2.0 * M_PI * n / N ) );
}

void fft_radix2_N4( complex * x, u32_t n )
{
    complex x0, x1, x2, x3;

    const complex w0 = 1;
    const complex w1 = nroot_unity( 1, 4 );
    const complex w2 = -w0;
    const complex w3 = -w1;

    x0 = x[ 0 ] + w0 * x[ 2 ];
    x1 = x[ 1 ] + w0 * x[ 3 ];
    x2 = x[ 0 ] + w2 * x[ 2 ];
    x3 = x[ 1 ] + w2 * x[ 3 ];

    x[ 0 ] = x0 + w0 * x1;
    x[ 1 ] = x0 + w2 * x1;
    x[ 2 ] = x2 + w1 * x3;
    x[ 3 ] = x2 + w3 * x3;
}

#ifdef REAL_FFT

typedef struct {
    double re;
    double im;
} complex64_t;

static inline complex64_t cexp_j(double angle) {
    return (complex64_t){ cos(angle), sin(angle) };
}

/*
 * OLD FFT
 */

// Helper function to reverse bits (needed for bit-reversal permutation)
static uint32_t reverse_bits(uint32_t x, int bits) {
    uint32_t result = 0;
    for (int i = 0; i < bits; i++) {
        result = (result << 1) | (x & 1);
        x >>= 1;
    }
    return result;
}

// Radix-2 FFT implementation compatible with your code
bool fft_radix2(complex64_t vec[], uint32_t n, bool inverse)
{
    // Length variables
    int levels = 0;  // Compute levels = floor(log2(n))
    for (uint32_t temp = n; temp > 1U; temp >>= 1)
    {
        levels++;
    }

    if ((uint32_t)1U << levels != n)
    {
        return false;  // n is not a power of 2
    }

    // Trigonometric tables
    complex64_t *exptable = malloc((n / 2) * sizeof(complex64_t));
    if (exptable == NULL)
        return false;

    for (uint32_t i = 0; i < n / 2; i++) {
        double angle = (inverse ? 2 : -2) * M_PI * i / n;
        exptable[i] = cexp_j(angle);
    }

    // Bit-reversed addressing permutation
    for (uint32_t i = 0; i < n; i++) {
        uint32_t j = reverse_bits(i, levels);
        if (j > i) {
            complex64_t temp = vec[i];
            vec[i] = vec[j];
            vec[j] = temp;
        }
    }

    // Cooley-Tukey decimation-in-time radix-2 FFT
    for (uint32_t size = 2; size <= n; size *= 2)
    {
        uint32_t halfsize = size / 2;
        uint32_t tablestep = n / size;

        for (uint32_t i = 0; i < n; i += size)
        {

            uint32_t k = 0;

            for ( uint32_t j = i; j < i + halfsize; j++ )
            {

                uint32_t l = j + halfsize;

                complex64_t temp =
                {
                    .re = vec[l].re * exptable[k].re - vec[l].im * exptable[k].im,
                    .im = vec[l].re * exptable[k].im + vec[l].im * exptable[k].re
                };

                vec[l].re = vec[j].re - temp.re;
                vec[l].im = vec[j].im - temp.im;
                vec[j].re += temp.re;
                vec[j].im += temp.im;

                k += tablestep;
            }
        }
        if (size == n)  // Prevent overflow in 'size *= 2'
            break;
    }

    free(exptable);
    return true;
}

void rfft_to_2N(complex64_t *output, complex64_t *data, u32_t half_N);

void rfft_radix2(double *input, complex64_t *output, u32_t N)
{
    u32_t half_N = N / 2;

    complex64_t *data = malloc(half_N * sizeof(complex64_t));

    for (u32_t i = 0; i < half_N; ++i)
    {
        data[i].re = input[2 * i];
        data[i].im = input[2 * i + 1];
    }

    fft_radix2(data, half_N, false);

    rfft_to_2N(output, data, half_N);

    free(data);
}

void rfft_to_2N(complex64_t *output, complex64_t *data, u32_t half_N)
{
    const u32_t N = 2 * half_N;

    output[0].re = data[0].re + data[0].im;
    output[0].im = 0;

    output[half_N].re = data[0].re - data[0].im;
    output[half_N].im = 0;

    for (u32_t i = 1; i < half_N; ++i)
    {
        const double R_n = data[i].re;
        const double I_n = data[i].im;

        const double R_m = data[half_N - i].re;
        const double I_m = data[half_N - i].im;

        const double rd = 0.5 * (R_n - R_m);
        const double rp = 0.5 * (R_n + R_m);

        const double id = 0.5 * (I_n - I_m);
        const double ip = 0.5 * (I_n + I_m);

        const double c = cos(M_PI * i / half_N);
        const double s = sin(M_PI * i / half_N);

        output[i].re = rp + c * ip - s * rd;
        output[i].im = id - s * ip - c * rd;

        output[N - i].re = output[i].re;
        output[N - i].im = -output[i].im;
    }
}

#endif

int main( void )
{
    complex x [ 4 ] = { 0.0, 0.0, 0.0, 1.0 };

    fft_radix2_N4( x, 4 );

    for( u32_t i = 0; i < 4; i++ )
    {
        printf("%f %f\n", creal(x[i]), cimag(x[i]));
    }

#ifdef REAL_FFT
    const uint32_t N = 1 << 22;

    double *input = malloc(N * sizeof(double));
    for (uint32_t i = 0; i < N; ++i) { input[i] = 0.0; }
    input[N-1]=1.0;

    complex64_t *output = malloc(N * sizeof(complex64_t));

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    printf("FFT starts (Radix-2)\n");
    rfft_radix2(input, output, N);
    printf("FFT ends\n");

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Radix-2 FFT of size %u took %.6f seconds.\n", N, elapsed);

    double mean = 0.0;

    for (uint32_t i = 0; i < N; ++i)
    {
        mean += sqrt(output[i].re*output[i].re + output[i].im*output[i].im);
    }

    printf("MEAN: %.55f\n", mean/N);

    free(input);
    free(output);
    return 0;

#endif

    return ( 0 );
}
