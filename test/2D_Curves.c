#include <math.h> // Include math.h for cos and sin functions
#include <stdio.h>
#include <stdlib.h>

#include "Oprea.h"
#include "CPlotLib.h"

// Parametric function

static double f_x(double t) { return cos(t)*sin(4*t); }
static double f_y(double t) { return sin(t)*sin(4*t); }

static double df_x(double t) 
{
    return 4.0*cos(t)*cos(4*t) - sin(t)*sin(4*t);
}
static double df_y(double t)
{
    return cos(t)*sin(4*t) + 4.0*sin(t)*cos(4*t);
}
static double d2f_x(double t)
{
    return -17.0*cos(t)*sin(4*t) - 8.0*sin(t)*cos(4*t);
}
static double d2f_y(double t)
{
    return -17.0* sin(t)*sin(4*t) + 8.0*cos(t)*cos(4*t);
}

static double d3f_x(double t){ return 0.0; }
static double d3f_y(double t){ return 0.0; }


static Color CurvatureColorFn(double t, void* userData)
{
    // Cast back to our CurvatureData struct
    Curve2* curve = (Curve2*)userData;

    // Convert param t => array index, just like you did before:
    double domainLen = curve->domain[1] - curve->domain[0];
    if (domainLen < 1e-14) 
    {
        // fallback color
        return COLOR_GREEN; 
    }
    // Map t => [0..(num_pts-1)]
    double alpha = (t - curve->domain[0]) / domainLen;
    int i = (int)(alpha * (curve->num_pts - 1));
    if (i < 0) i=0;
    if (i >= (int)curve->num_pts) i = curve->num_pts - 1;


    double k = curve->kappa[i];
    double kMin, kMax;
    computeCurvatureRange(curve, &kMin, &kMax);

    // Some function curvatureToColor(k, kMin, kMax)
    Color c = {0};
    curvatureToColor(k, kMin, kMax, &c.r, &c.g, &c.b);
    c.a = 1.f;
    return c;
}

Color RainbowColor(double t, void* userData)
{
    // simple rainbow: Hue cycles from 0..360 as t goes from 0..2π, for instance
    float hue = (float)(fmod(t, 2.0*M_PI)/(2.0*M_PI)*360.0);
    Color c = {0};
    HSVtoRGB(hue, 1.0f, 1.0f, &c);
    c.a = 1.0f;
    return c;
}

int main(void)
{

    double domain[2] = {0.0, 2.0*M_PI};
    FuncVec2 f   = { f_x,  f_y };
    FuncVec2 df  = { df_x, df_y };
    FuncVec2 d2f = { d2f_x,d2f_y};

    // FuncVec2 f   = { spiral_x,  spiral_y };
    // FuncVec2 df  = { spiral_dx, spiral_dy };
    // FuncVec2 d2f = { spiral_d2x,spiral_d2y};

    FuncVec2 d3f = { d3f_x,d3f_y};

    Curve2 curve;
    InitCurve2(&curve,&f,&df,&d2f,&d3f,300,domain);

    // Create a figure with specified width and height
    CPLFigure* fig = CreateFigure(800, 600);
    if (!fig) {
        fprintf(stderr, "Failed to create figure.\n");
        return EXIT_FAILURE;
    }

    // set fig bg color to dark gray
    Color darkGray = {0.2f, 0.2f, 0.2f, 1.0f};
    fig->bg_color = darkGray;

    // Add a single plot to the figure
    CPLPlot* plot = AddPlot(fig);
    if (!plot) {
        fprintf(stderr, "Failed to add plot to figure.\n");
        FreeFigure(fig); 
        return EXIT_FAILURE;
    }

    // Set X and Y ranges
    double x_range[2] = { -1.0, 1.0 };
    double y_range[2] = { -1.0, 1.0 };
    SetXRange(plot, x_range);  
    SetYRange(plot, y_range);  

    // Prepare data
    size_t num_points = 300;
    double x[300];
    double y[300];

    // Fill data
    for (size_t i = 0; i < num_points; ++i) {
        // map i to [0..2π]
        double t = domain[0] + i * (domain[1] - domain[0]) / (num_points - 1);
        x[i] = f_x(t);
        y[i] = f_y(t);
    }

    Plot(plot, x, y, num_points, COLOR_GREEN, NULL, NULL);

    ShowFigure(fig);

    // 5) Clean up
    FreeCurve2(&curve);

    return 0;
}