#ifndef O_PLOT_HPP
#define O_PLOT_HPP

#include "Figure.h"
#include "../Curve.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Call once: prepare the data for the given curve (positions + color),
 * create VBO/VAO, do an auto-scale for the projection, 
 * and store references in the Renderer to draw later.
 */
void PlotCurve2D(Figure *fig, const Curve2* curve);

/**
 * Renders the 2D axes and curve (already passed to PlotCurve2D).
 * Must be called each frame, after RendererClear(), and before RendererSwapBuffers().
 */
void PlotDraw(Figure *fig);

void ShowPlot(Figure *fig);

#ifdef __cplusplus
}
#endif

#endif // O_PLOT_HPP
