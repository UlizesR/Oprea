#ifndef O_CURVE_HPP
#define O_CURVE_HPP

#include <stddef.h>
#include "Vector.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A structure to store a 2D curve discretized at several points.
 *
 * f, df, d2f, d3f are pointers to function-valued vectors
 * that give the position, first derivative, second derivative,
 * and third derivative, respectively, at a parameter t.
 *
 * kappa[i] = curvature at i-th point
 */
typedef struct Curve2
{
    const FuncVec2 *f;  
    const FuncVec2 *df; 
    const FuncVec2 *d2f;
    const FuncVec2 *d3f;
    
    size_t num_pts;     

    double domain[2]; 
    double *t;        
    double dt;        

    // Position
    Vec2  *P;         
    // Tangent
    Vec2  *T;         
    // Magnitude of T
    double *Tn;       
    // Second derivative
    Vec2  *d2P;       
    // Third derivative (optional)
    Vec2  *d3P;       

    // Normal vector in 2D (rotated T)
    Vec2  *N;         

    // Curvature
    double *kappa;   

    // Torsion in 2D (normally zero)
    double *tau;     

    // min/max of the curve
    double d_minX, d_maxX, d_minY, d_maxY;
    // int minx, maxx, miny, maxy;
    int i_minX, i_maxX, i_minY, i_maxY;

} Curve2;

/**
 * Initializes the curve structure, allocates memory, 
 * and fills in all geometric data (P, T, N, kappa, etc.).
 */
void InitCurve2(
    Curve2 *curve,
    const FuncVec2 *f,
    const FuncVec2 *df,
    const FuncVec2 *d2f,
    const FuncVec2 *d3f,
    size_t num_pts,
    const double domain[2]
);

/** Frees all allocated arrays in the curve. */
void FreeCurve2(Curve2 *curve);

void computeMinMax(const Curve2* curve, double* minX, double* maxX, double* minY, double* maxY);

void curvatureToColor(double k, double kMin, double kMax, float *r, float *g, float *b);

void computeCurvatureRange(const Curve2* curve, double* kMin, double* kMax);

#ifdef __cplusplus
}
#endif

#endif // O_CURVE_HPP
