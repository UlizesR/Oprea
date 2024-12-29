#ifndef O_FIGURE_H
#define O_FIGURE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Colors.h"

/**
 * Opaque struct that holds the window, GL context, etc.
 */
typedef struct Figure
{
    GLFWwindow* window;

    // The shader program we use for lines
    GLuint programID;
    GLint  loc_u_projection;

    // size of the figure
    int width;
    int height;
} Figure;

/**
 * Creates a window and sets up an OpenGL 3.3 core context.
 */
Figure* CreateFigure(int width, int height, const char* title);

/**
 * Returns true if the window is open and not signaled to close.
 */
int FigureIsOpen(Figure* fig);

/**
 * Clears the screen (background black).
 */
void FigureClear(Figure* fig, Color bg);

/**
 * Swap front/back buffers + poll events.
 */
void FigureSwapBuffers(Figure* fig);

/**
 * Destroys the window, OpenGL context, and frees resources.
 */
void FreeFigure(Figure* fig);

#ifdef __cplusplus
}
#endif

#endif // O_FIGURE_H
