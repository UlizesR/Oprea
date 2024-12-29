#include <math.h> // Include math.h for cos and sin functions

#include "Oprea.h"
#include "CPlotLib.h"

// Parametric function
static double spiral_x(double t) { return t * cos(t); }
static double spiral_y(double t) { return t * sin(t); }

static double spiral_dx(double t) { return cos(t) - t * sin(t); }
static double spiral_dy(double t) { return sin(t) + t * cos(t); }

static double spiral_d2x(double t) { return -2 * sin(t) - t * cos(t); }
static double spiral_d2y(double t) { return 2 * cos(t) - t * sin(t); }

static double circle_x(double t) { return cos(t); }
static double circle_y(double t) { return sin(t); }

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
    Figure* fig = CreateFigure(800,600,"2D Curve Plot");
    if(!fig) return -1;

    double domain[2] = {0.0, 2.0*M_PI};
    FuncVec2 f   = { f_x,  f_y };
    FuncVec2 df  = { df_x, df_y };
    FuncVec2 d2f = { d2f_x,d2f_y};
    FuncVec2 d3f = { d3f_x,d3f_y};

    // FuncVec2 f   = { spiral_x,  spiral_y };
    // FuncVec2 df  = { spiral_dx, spiral_dy };
    // FuncVec2 d2f = { spiral_d2x,spiral_d2y};

    Curve2 curve;
    InitCurve2(&curve,&f,&df,&d2f,&d3f,300,domain);

    // 1) Plot a parametric curve with a color callback
    PlotParam2D(
        fig,      // figure
        f_x,      // x(t)
        f_y,      // y(t)
        0.0,      // tMin
        2.0*M_PI, // tMax
        300,      // numSamples
        COLOR_WHITE,          // fallback line color
        RainbowColor,     // pass in your callback
        (void*)&curve         // pass in your data
    );

    // PlotParam2D(
    //     fig,      // figure
    //     spiral_x, // x(t)
    //     spiral_y, // y(t)
    //     0.0,      // tMin
    //     2.0*M_PI, // tMax
    //     300,      // numSamples
    //     COLOR_WHITE,          // fallback line color
    //     CurvatureColorFn,     // pass in your callback
    //     (void*)&curve         // pass in your data
    // );

    // 2) Plot another param with a color callback
    //    e.g., circle with rainbow
    // PlotParam2D(fig, circle_x, circle_y, 0.0, 2.0 * M_PI, 200, COLOR_GREEN, RainbowColor);

    int count = 5;
    double xTicks[count];
    double yTicks[count];


    for(int i = 0; i < count; i++)
    {
        xTicks[i] = curve.i_minX + i*(curve.i_maxX - curve.i_minX) * (1.0 / (count-1));
        yTicks[i] = curve.i_minY + i*(curve.i_maxY - curve.i_minY) * (1.0 / (count-1));
    }

    // We want 5 ticks each
    SetXaxis(fig,xTicks,5);
    SetYaxis(fig,yTicks,5);

    // 4) Show the figure
    ShowPlot(fig);

    // 5) Clean up
    FreeCurve2(&curve);

    return 0;
}