#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Oprea.h"

//------------------------------------------------------------------------------
// Example parametric curve: let's pick a "heart-like" shape or something else
// We just show a quick example with "FuncVec2" callbacks. Adapt as you wish.
//------------------------------------------------------------------------------
static double f_x(double t) { return cos(t)*sin(4*t); }
static double f_y(double t) { return sin(t)*sin(4*t); }

static double df_x(double t) 
{
    // derivative of cos(t)*sin(4*t)
    return 4.0*cos(t)*cos(4*t) - sin(t)*sin(4*t);
}

static double df_y(double t)
{
    // derivative of sin(t)*sin(4*t)
    return cos(t)*sin(4*t) + 4.0*sin(t)*cos(4*t);
}

static double d2f_x(double t)
{
    // second derivative of cos(t)*sin(4*t)
    return -17.0 * cos(t) * sin(4*t) - 8.0 * sin(t) * cos(4*t);
}

static double d2f_y(double t)
{
    // second derivative of sin(t)*sin(4*t)
    return -17.0 * sin(t) * sin(4*t) + 8.0 * cos(t) * cos(4*t);
}

// We'll skip d3f or also do a numeric approximation if needed:
static double d3f_x(double t) { return 0.0; }
static double d3f_y(double t) { return 0.0; }

int main(void)
{
    // 1) Create the Figure (window + GL)
    Figure* fig = CreateFigure(800, 600, "2D Curve Plot");
    printf("Figure created\n");

    // 2) Build a curve from t in [0..2π]
    double domain[2] = {0.0, 2.0*M_PI};
    FuncVec2 f   = { f_x,   f_y   };
    FuncVec2 df  = { df_x,  df_y  };
    FuncVec2 d2f = { d2f_x, d2f_y };
    FuncVec2 d3f = { d3f_x, d3f_y };
    Curve2 curve;
    printf("Initializing curve...\n");
    InitCurve2(&curve, &f, &df, &d2f, &d3f, 300, domain);
    printf("Curve initialized\n");

    // 3) Plot the curve
    PlotCurve2D(fig, &curve);
    printf("Curve plotted\n");

    // 4) Show the plot
    ShowPlot(fig);

    // 5) Cleanup
    FreeCurve2(&curve);
    FreeFigure(fig);

    printf("Done\n");

    return 0;
}