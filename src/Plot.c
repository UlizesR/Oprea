#include <stdlib.h>
#include <math.h>
#include "Plot/Plot.h"

// We'll store static info about the curve's VAO, VBO, range, etc.
static GLuint s_curveVAO    = 0;
static GLuint s_curveVBO    = 0;
static GLsizei s_curveCount = 0;

// For axis lines
static GLuint s_axisVAO   = 0;
static GLuint s_axisVBO   = 0;
static GLsizei s_axisCount = 0;

// We store the minX, maxX, minY, maxY for auto-scaling
static float s_minX= -1.f, s_maxX=1.f, s_minY=-1.f, s_maxY=1.f;

static int s_isDataLoaded = 0; // If we've called PlotCurve2D

/**
 * Simple function to map curvature to a color:
 * green -> yellow -> red
 */
static void curvatureToColor(double k, double kMin, double kMax, float *r, float *g, float *b)
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

// Create an orthographic matrix from (left,right,bottom,top).
// We'll store it as a 4x4 in column-major order:
static void makeOrthoMatrix(float left, float right, float bottom, float top, float* out)
{
    // near/far = -1..1 for 2D
    float zNear = -1.f;
    float zFar  =  1.f;
    
    // Column-major
    out[0] = 2.0f / (right - left);
    out[1] = 0; 
    out[2] = 0; 
    out[3] = 0;

    out[4] = 0; 
    out[5] = 2.0f / (top - bottom); 
    out[6] = 0; 
    out[7] = 0;

    out[8] = 0; 
    out[9] = 0; 
    out[10] = -2.0f / (zFar - zNear); 
    out[11] = 0;

    out[12] = -(right + left) / (right - left);
    out[13] = -(top + bottom) / (top - bottom);
    out[14] = -(zFar + zNear) / (zFar - zNear);
    out[15] = 1.0f;
}

// Builds a small line set for axes: 
// X-axis: (minX,0)->(maxX,0)
// Y-axis: (0,minY)->(0,maxY)
static void BuildAxisData(Figure *fig)
{
    if (!fig) return;
    
    if (s_axisVAO == 0)
        glGenVertexArrays(1, &s_axisVAO);
    if (s_axisVBO == 0)
        glGenBuffers(1, &s_axisVBO);

    // We'll store 2 lines => 4 points => (x, y, r,g,b)
    float axisData[4 * 5];
    // X-axis
    axisData[0] = s_minX; axisData[1] = 0.0f; 
    axisData[2] = 0.5f;   axisData[3] = 0.5f; axisData[4] = 0.5f; // gray
    axisData[5] = s_maxX; axisData[6] = 0.0f;
    axisData[7] = 0.5f;   axisData[8] = 0.5f; axisData[9] = 0.5f;

    // Y-axis
    axisData[10] = 0.0f; axisData[11] = s_minY;
    axisData[12] = 0.5f; axisData[13] = 0.5f; axisData[14] = 0.5f;
    axisData[15] = 0.0f; axisData[16] = s_maxY;
    axisData[17] = 0.5f; axisData[18] = 0.5f; axisData[19] = 0.5f;

    s_axisCount = 4; // 2 lines => 4 vertices

    glBindVertexArray(s_axisVAO);
    glBindBuffer(GL_ARRAY_BUFFER, s_axisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisData), axisData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1); // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

static void generateVAOandVBO(GLuint *vao, GLuint *vbo)
{
    if (*vao == 0)
        glGenVertexArrays(1, vao);
    if (*vbo == 0)
        glGenBuffers(1, vbo);
}

static void computeMinMax(const Curve2* curve, double *minX, double *maxX, double *minY, double *maxY)
{
    *minX = 1e30, *maxX = -1e30, *minY = 1e30, *maxY = -1e30;
    for (size_t i = 0; i < curve->num_pts; i++)
    {
        if (curve->P[i].x < *minX) *minX = curve->P[i].x;
        if (curve->P[i].x > *maxX) *maxX = curve->P[i].x;
        if (curve->P[i].y < *minY) *minY = curve->P[i].y;
        if (curve->P[i].y > *maxY) *maxY = curve->P[i].y;
    }
}

static void computeCurvatureRange(const Curve2* curve, double *kMin, double *kMax)
{
    *kMin = 1e30, *kMax = -1e30;
    for (size_t i = 0; i < curve->num_pts; i++)
    {
        double k = curve->kappa[i];
        if (k < *kMin) *kMin = k;
        if (k > *kMax) *kMax = k;
    }
    if (*kMin == *kMax) *kMax = *kMin + 1.0; // avoid division by 0
}

void PlotCurve2D(Figure *fig, const Curve2* curve)
{
    if (!fig || !curve) return;

    // 1) Compute min/max for auto-scale
    double minX, maxX, minY, maxY;
    computeMinMax(curve, &minX, &maxX, &minY, &maxY);

    // small margin
    double marginX = fmax(0.1 * (maxX - minX), 0.1);
    double marginY = fmax(0.1 * (maxY - minY), 0.1);

    s_minX = (float)(minX - marginX);
    s_maxX = (float)(maxX + marginX);
    s_minY = (float)(minY - marginY);
    s_maxY = (float)(maxY + marginY);

    // 2) Build CPU array for (x,y,r,g,b)
    float* data = (float*)malloc(curve->num_pts * 5 * sizeof(float));
    if (!data) return;

    // find min, max curvature
    double kMin, kMax;
    computeCurvatureRange(curve, &kMin, &kMax);

    for (size_t i = 0; i < curve->num_pts; i++)
    {
        float* ptr = &data[i * 5];
        ptr[0] = (float)curve->P[i].x;   // x
        ptr[1] = (float)curve->P[i].y;   // y

        float rcol, gcol, bcol;
        curvatureToColor(curve->kappa[i], kMin, kMax, &rcol, &gcol, &bcol);
        ptr[2] = rcol;
        ptr[3] = gcol;
        ptr[4] = bcol;
    }

    // 3) Create VAO/VBO
    generateVAOandVBO(&s_curveVAO, &s_curveVBO);

    glBindVertexArray(s_curveVAO);
    glBindBuffer(GL_ARRAY_BUFFER, s_curveVBO);
    glBufferData(GL_ARRAY_BUFFER, curve->num_pts * 5 * sizeof(float), data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1); // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    s_curveCount = (GLsizei)curve->num_pts;
    free(data);

    // 4) Build axis lines
    BuildAxisData(fig);

    s_isDataLoaded = 1;
}

void PlotDraw(Figure *fig)
{
    if (!fig || !s_isDataLoaded) return;

    // Use the shared program
    glUseProgram(fig->programID);

    // Create an orthographic projection from s_minX..s_maxX, s_minY..s_maxY
    float proj[16];
    makeOrthoMatrix(s_minX, s_maxX, s_minY, s_maxY, proj);
    glUniformMatrix4fv(fig->loc_u_projection, 1, GL_FALSE, proj);

    // 1) Draw axes
    glBindVertexArray(s_axisVAO);
    glDrawArrays(GL_LINES, 0, s_axisCount);

    // 2) Draw the curve
    glBindVertexArray(s_curveVAO);
    glDrawArrays(GL_LINE_STRIP, 0, s_curveCount);

    glBindVertexArray(0);
    glUseProgram(0);
}

void ShowPlot(Figure *fig)
{
    if (!fig) return;

    // 4) Render loop
    while (FigureIsOpen(fig))
    {
        FigureClear(fig, COLOR_BLACK);
        PlotDraw(fig);
        FigureSwapBuffers(fig);
    }

    // 5) Cleanup
    glDeleteVertexArrays(1, &s_curveVAO);
    glDeleteBuffers(1, &s_curveVBO);
    glDeleteVertexArrays(1, &s_axisVAO);
    glDeleteBuffers(1, &s_axisVBO);
}