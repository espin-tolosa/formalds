#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <complex.h>
#include <time.h>

#include "sp_private.h"

/*
 * defintions:
 *
 * t    : discrete-time independent variable, which can takes the values of t = nT
 * T    : sampling period of the the discrete-time signal
 * ts   : total sampling time after taken the total number of ns samples ts = ns T
 *
 * s[t] : discrete-time dependent variable
 * n[z] : gausian noise, random variables can satisfy n[t] != n[t], because noise also changes with position according to: z = t+cx
 *
 */

static double * create_s( float T, float tp, float ts );
static double * create_x( const double *s, int ns, int d, double a, double nlo, double nhi );
static double complex * copy_to_complex( const double *x, int ns, int padd );
static double complex * compute_Z( const double complex *X, const double complex *Y, int n );
static double * compute_z( const double complex *Z, int n );
static double * center_correlation( const double *z, int n );
static int find_argmax( const double * zc, int n );
static void plot_xyz(const double * x, const double * y, const double * z, int ns, int padd );

static int main_start( int delay );

const float T   = 1.0f/256.0f;
const float ts  = 1.0f;
const int   padd= 2;
const double tp = 0.25f;

const double noise_low = -0.125;
const double noise_hig = +0.125;

int main(void)
{
    srand((unsigned)time(NULL));

    int delay = -10;
    int count = 0;

    bool is_err = false;

    while( delay < 10)
    {
        const int arg_max = main_start( delay );
        if( arg_max != delay )
        {
            is_err = true;
            fprintf(stderr,"Compute Delay: %d, Error: %d\n", arg_max, arg_max - delay );
        }

        if( count == 100 )
        {
            count = 0;
            delay += 1;
        }
        count++;
    }

    if( !is_err )
    {
        fprintf(stderr, "Test Pass - All delays are computed exatcly\n");
    }

    return 0;
}

static int main_start( int delay )
{
    const int   ns  = (int) (ts/T);

    /* 0. create the signal s[t], which is a pulse of duration tp bounded by: T << tp << ts */
    const double * s    = create_s( T, tp, ts );

    /* 1. write signal 1: x[t] = a·s[t] + n1[t] */
    const double * x    = create_x( s, ns, 0, 1.0, 0.0, 0.0 );

    /* 2. write signal 2: y[t] = b·s[t-d] + n2[t] */
    const double * y    = create_x( s, ns, delay, 0.8, noise_low, noise_hig );

    /* 3. copy signal 1 to a complex array called X[f] */
    complex double * X = copy_to_complex( x, ns, padd );

    /* 4. copy signal 2 to a complex array called Y[f]*/
    complex double * Y = copy_to_complex( y, ns, padd );

    /* 5. compute the fft of x[t] stored in X[f] */
    if( !sp_fft( X, ns * padd ) ) { fprintf( stderr, "[Error] FFT{x}\n");}

    /* 6. compute the fft of y[t] stored in Y[f] */
    if( !sp_fft( Y, ns * padd ) ) { fprintf( stderr, "[Error] FFT{y}\n");}

    /* 7. compute Z = X·conj(Y) */
    const complex double * Z = compute_Z( X, Y, ns * padd );

    /* 8. compute z = creal( sp_ifft( Z, n ) ) */
    const double * z = compute_z( Z, ns*padd );

    /* 9. center z */
    const double * zc = center_correlation( z, ns*padd );

    /* 10. compute delay from cross-correlation peak */
    const int argmax = find_argmax( zc, ns*padd ) - ns;

    /* 11. print results */
    plot_xyz(x, y, zc, ns, padd );

    /* clean */
    free((void*)s);
    free((void*)x);
    free((void*)y);
    free((void*)X);
    free((void*)Y);
    free((void*)z);
    free((void*)zc);

    return ( argmax );
}

static double * create_s( float T, float tp, float ts )
{
    const float f = 2.0f;

    int ns = (int)( ts / T );
    int np = (int)( tp / T );
    double *s = (double *) malloc( ns * sizeof( double ) );

    if ( s == NULL )
    {
        return NULL;
    }

    for ( int i = 0; i < ns; i++ )
    {
        if ( ( 0 < (i - ns/2 + np/2) ) && ( (i - ns/2 + np/2) < np ) )
        {
            const double evolve = sin((i-ns/2+np/2)*2.0f*M_PI*T*f);

            s[i] = evolve * sin((i-ns/2)*2.0f*M_PI*T*8.0f*f) * evolve;
        }
        else
        {
            s[i] = 0.0;
        }
    }

    return s;
}

static double * create_x( const double *s, int ns, int d, double a, double nlo, double nhi )
{
    double *x = (double *) malloc( ns * sizeof( double ) );

    if ( x == NULL )
    {
        return NULL;
    }

    if( d > 0 )
    {
        for ( int i = 0; i < ns; i++ )
        {
            double u1 = (double) rand() / RAND_MAX;
            double u2 = (double) rand() / RAND_MAX;
            double z = sqrt( -2.0 * log( u1 ) ) * cos( 2.0 * M_PI * u2 ); /* Gaussian (mean=0, std=1) */

            double n = ( ( nhi - nlo ) / 2.0 ) * z; /* scale to desired range approximately */

            if( i>=d )
            {
                x[i-d] = a * s[i] + n;
            }
            else
            {
                x[i] = a * s[i] + n;
            }
        }
    }

    else
    {
        for ( int i = 0; i < ns; i++ )
        {
            double u1 = (double) rand() / RAND_MAX;
            double u2 = (double) rand() / RAND_MAX;
            double z = sqrt( -2.0 * log( u1 ) ) * cos( 2.0 * M_PI * u2 ); /* Gaussian (mean=0, std=1) */

            double n = ( ( nhi - nlo ) / 2.0 ) * z; /* scale to desired range approximately */

            if( (i-d)<ns )
            {
                x[i-d] = a * s[i] + n;
            }
            else
            {
                x[i] = a * s[i] + n;
            }
        }
    }


    return x;
}

double complex * copy_to_complex( const double *x, int ns, int padd )
{
    int n2 = 1;
    while ( n2 < ns ) { n2 <<= 1; }

    n2 = padd * n2; /* padd is usually 2, to double the length of the complex signal */

    double complex *X = (double complex *) malloc( n2 * sizeof( double complex ) );

    if ( X == NULL )
    {
        return NULL;
    }

    for ( int i = 0; i < ns; i++ )
    {
        X[i] = x[i] + 0.0 * I;
    }

    for ( int i = ns; i < n2; i++ )
    {
        X[i] = 0.0 + 0.0 * I;
    }

    return X;
}

static double complex * compute_Z( const double complex *X, const double complex *Y, int n )
{
    double complex *Z = (double complex *) malloc( n * sizeof( double complex ) );

    if ( Z == NULL )
    {
        return NULL;
    }

    for ( int i = 0; i < n; i++ )
    {
        Z[i] = X[i] * conj( Y[i] );
    }

    return Z;
}

static double * compute_z( const double complex *Z, int n )
{
    double complex *Zcopy = (double complex *) malloc( n * sizeof( double complex ) );

    if ( Zcopy == NULL )
    {
        return NULL;
    }

    for ( int i = 0; i < n; i++ )
    {
        Zcopy[i] = Z[i];
    }

    if ( !sp_ifft( Zcopy, n ) )
    {
        free( Zcopy );
        return NULL;
    }

    double *z = (double *) malloc( n * sizeof( double ) );

    if ( z == NULL )
    {
        free( Zcopy );
        return NULL;
    }

    for ( int i = 0; i < n; i++ )
    {
        z[i] = creal( Zcopy[i] );
    }

    free( Zcopy );

    return z;
}

static double * center_correlation( const double *z, int n )
{
    double *zc = (double *) malloc( n * sizeof( double ) );

    if ( zc == NULL )
    {
        return NULL;
    }

    int half = n / 2;

    for ( int i = 0; i < n; i++ )
    {
        int j = ( i + half ) % n;
        zc[i] = z[j];
    }

    return zc;
}

static int find_argmax( const double * zc, int n )
{
    if( zc == NULL )
    {
        return 0;
    }

    double zmax = fabs( zc[0] );
    int argmax  = 0;

    for ( int i = 1; i < n; i++ )
    {
        if ( fabs( zc[i] ) > zmax )
        {
            zmax = fabs( zc[i] );
            argmax = i;
        }
    }

    return ( argmax );
}

static void plot_xyz(const double * x, const double * y, const double * z, int ns, int padd )
{
    for(int i = 0; i < ns*padd; ++i)
    {
        if(i<ns)
        {
            fprintf(stdout,"%f,%f,%f\n",x[i],y[i],z[i]);
        }
        else
        {
            fprintf(stdout,"%f,%f,%f\n",0.0,0.0,z[i]);
        }
    }
}
