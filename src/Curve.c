#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Curve.h"

/**
 * Helper to free allocated arrays in the curve struct
 * and set pointers to NULL.
 */
static void FreeCurve2Arrays(Curve2 *curve) 
{
    free(curve->t);
    free(curve->P);
    free(curve->T);
    free(curve->Tn);
    free(curve->d2P);
    free(curve->d3P);
    free(curve->N);
    free(curve->kappa);
    free(curve->tau);

    curve->t = NULL;
    curve->P = NULL;
    curve->T = NULL;
    curve->Tn = NULL;
    curve->d2P = NULL;
    curve->d3P = NULL;
    curve->N = NULL;
    curve->kappa = NULL;
    curve->tau = NULL;
}

static void AllocateCurve2Arrays(Curve2 *curve, size_t num_pts) 
{
    curve->t = (double *)malloc(num_pts * sizeof(double));
    curve->P = (Vec2 *)malloc(num_pts * sizeof(Vec2));
    curve->T = (Vec2 *)malloc(num_pts * sizeof(Vec2));
    curve->Tn = (double *)malloc(num_pts * sizeof(double));
    curve->d2P = (Vec2 *)malloc(num_pts * sizeof(Vec2));
    curve->d3P = (Vec2 *)malloc(num_pts * sizeof(Vec2));
    curve->N = (Vec2 *)malloc(num_pts * sizeof(Vec2));
    curve->kappa = (double *)malloc(num_pts * sizeof(double));
    curve->tau = (double *)malloc(num_pts * sizeof(double));

    if (!curve->t || !curve->P || !curve->T || !curve->Tn || !curve->d2P || !curve->d3P || !curve->N || !curve->kappa || !curve->tau) 
    {
        fprintf(stderr, "Memory allocation failed in InitCurve2.\n");
        FreeCurve2Arrays(curve);
        exit(EXIT_FAILURE);
    }
}

static void ComputeCurve2Properties(Curve2 *curve, const FuncVec2 *f, const FuncVec2 *df, const FuncVec2 *d2f,                const FuncVec2 *d3f) 
{
    for (size_t i = 0; i < curve->num_pts; i++) 
    {
        double t_val = curve->domain[0] + i * curve->dt;
        curve->t[i] = t_val;

        // Evaluate position
        Vec2 Ptmp = {f->x(t_val), f->y(t_val)};
        curve->P[i] = Ptmp;

        // Evaluate first derivative
        Vec2 Ttmp = {df->x(t_val), df->y(t_val)};
        curve->T[i] = Ttmp;
        double Tnorm = sqrt(Ttmp.x * Ttmp.x + Ttmp.y * Ttmp.y);
        curve->Tn[i] = Tnorm;

        // Evaluate second derivative
        Vec2 d2Ptmp = {d2f->x(t_val), d2f->y(t_val)};
        curve->d2P[i] = d2Ptmp;

        // Evaluate third derivative if provided
        Vec2 d3Ptmp = {0.0, 0.0};
        if (d3f) 
        {
          d3Ptmp.x = d3f->x(t_val);
          d3Ptmp.y = d3f->y(t_val);
        }
        curve->d3P[i] = d3Ptmp;

        // Compute curvature kappa = |T x d2P| / |T|^3  in 2D
        double cross_val = (Ttmp.x * d2Ptmp.y - Ttmp.y * d2Ptmp.x);
        double denom = Tnorm * Tnorm * Tnorm;
        curve->kappa[i] = (denom < 1e-14) ? 0.0 : fabs(cross_val) / denom;

        // Torsion = 0 in 2D
        curve->tau[i] = 0.0;

        // Compute normal by rotating T by +90 deg and normalizing
        Vec2 Ntmp = {-Ttmp.y, Ttmp.x};
        double Nnorm = sqrt(Ntmp.x * Ntmp.x + Ntmp.y * Ntmp.y);
        if (Nnorm > 1e-14) 
        {
          Ntmp.x /= Nnorm;
          Ntmp.y /= Nnorm;
        }
        curve->N[i] = Ntmp;
    }
}

void InitCurve2(
    Curve2 *curve, 
    const FuncVec2 *f, 
    const FuncVec2 *df,
    const FuncVec2 *d2f, 
    const FuncVec2 *d3f, 
    size_t num_pts,
    const double domain[2]
) 
{
    curve->f = f;
    curve->df = df;
    curve->d2f = d2f;
    curve->d3f = d3f;

    curve->num_pts = num_pts;
    memcpy(curve->domain, domain, 2 * sizeof(double));

    AllocateCurve2Arrays(curve, num_pts);

    // Step size
    curve->dt = (domain[1] - domain[0]) / ((num_pts > 1) ? (num_pts - 1) : 1);

    // Fill in data
    ComputeCurve2Properties(curve, f, df, d2f, d3f);

    // Compute min/max
    computeMinMax(curve, &curve->d_minX, &curve->d_maxX, &curve->d_minY, &curve->d_maxY);

    // Find the closest integer to the max and min
    curve->i_minX = (int)floor(curve->d_minX);
    curve->i_maxX = (int)ceil(curve->d_maxX);
    curve->i_minY = (int)floor(curve->d_minY);
    curve->i_maxY = (int)ceil(curve->d_maxY);
}

void FreeCurve2(Curve2 *curve) 
{
    if (!curve)
        return;
    FreeCurve2Arrays(curve);
}

void computeMinMax(const Curve2 *curve, double *minX, double *maxX, double *minY, double *maxY) 
{
    *minX = 1e30;
    *maxX = -1e30;
    *minY = 1e30;
    *maxY = -1e30;

    for (size_t i = 0; i < curve->num_pts; i++) 
    {
        double x = curve->P[i].x;
        double y = curve->P[i].y;
        if (x < *minX)
          *minX = x;
        if (x > *maxX)
          *maxX = x;
        if (y < *minY)
          *minY = y;
        if (y > *maxY)
          *maxY = y;
    }
}

void curvatureToColor(double k, double kMin, double kMax, float *r, float *g, float *b)
{
    // clamp k to [kMin, kMax]
    double t = (k - kMin) / (kMax - kMin);
    t = fmax(0.0, fmin(1.0, t));

    // 0 => green (0,1,0), 0.5 => yellow (1,1,0), 1 => red (1,0,0)
    if (t < 0.5)
    {
        float u = (float)(t / 0.5);
        *r = u;       // 0->1
        *g = 1.0f;    // 1
        *b = 0.0f;
    }
    else
    {
        float u = (float)((t - 0.5) / 0.5);
        *r = 1.0f;
        *g = 1.0f - u; 
        *b = 0.0f;
    }
}

void computeCurvatureRange(const Curve2 *curve, double *kMin, double *kMax) 
{
    if (!curve || !kMin || !kMax)
        return;
    *kMin = 1e30;
    *kMax = -1e30;
    for (size_t i = 0; i < curve->num_pts; i++) 
    {
        double val = curve->kappa[i];
        if (val < *kMin)
          *kMin = val;
        if (val > *kMax)
          *kMax = val;
    }
    if (fabs(*kMax - *kMin) < 1e-14)
        *kMax = *kMin + 1.0;
}