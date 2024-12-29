#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Plot/Figure.h"
#include "Plot/utils.h"

#define SHADER_VERTEX_PATH "shaders/vertex.glsl"
#define SHADER_FRAGMENT_PATH "shaders/fragment.glsl"

Figure* CreateFigure(int width, int height, const char* title)
{
    if(!glfwInit())
    {
        fprintf(stderr,"Failed to init GLFW.\n");
        return NULL;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    Figure* fig = (Figure*)calloc(1, sizeof(Figure));
    if(!fig)
    {
        fprintf(stderr,"Failed to allocate Figure.\n");
        glfwTerminate();
        return NULL;
    }

    fig->window = glfwCreateWindow(width, height, title, NULL, NULL);
    if(!fig->window)
    {
        fprintf(stderr,"Failed to create GLFW window.\n");
        free(fig);
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(fig->window);

    // GLEW
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK)
    {
        fprintf(stderr,"Failed to init GLEW.\n");
        glfwDestroyWindow(fig->window);
        free(fig);
        glfwTerminate();
        return NULL;
    }
    glViewport(0,0,width,height);

    // Compile our default line-drawing shaders 
    char* vsrc = ReadFile(SHADER_VERTEX_PATH);
    char* fsrc = ReadFile(SHADER_FRAGMENT_PATH);
    if(!vsrc || !fsrc)
    {
        fprintf(stderr,"Failed to load shader files.\n");
        if(vsrc) free(vsrc);
        if(fsrc) free(fsrc);
        glfwDestroyWindow(fig->window);
        free(fig);
        glfwTerminate();
        return NULL;
    }
    GLuint vs = CompileShader(vsrc, GL_VERTEX_SHADER, 0);
    GLuint fs = CompileShader(fsrc, GL_FRAGMENT_SHADER, 0);
    free(vsrc);
    free(fsrc);

    fig->programID = LinkProgram(vs, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    // Get the uniform location for the projection matrix
    fig->loc_u_projection = glGetUniformLocation(fig->programID, "u_projection");
    if(fig->loc_u_projection < 0)
    {
        fprintf(stderr, "Warning: uniform 'u_projection' not found.\n");
    }

    // Enable alpha blending if needed
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    fig->width = width;
    fig->height = height;

    return fig;
}

int FigureIsOpen(Figure* fig)
{
    return (fig && !glfwWindowShouldClose(fig->window));
}

void FigureClear(Figure* fig, Color bg)
{
    if(!fig) return;

    // Get the *actual* framebuffer size (may differ from window size on HiDPI)
    int width, height;
    glfwGetFramebufferSize(fig->window, &width, &height);

    // Make sure OpenGL knows to render to the full area
    glViewport(0, 0, width, height);

    // Now clear the background (black)
    glClearColor(bg.r, bg.g, bg.b, bg.a);
    glClear(GL_COLOR_BUFFER_BIT);
}


void FigureSwapBuffers(Figure* fig)
{
    if(!fig) return;
    glfwSwapBuffers(fig->window);
    glfwPollEvents();
}

void FreeFigure(Figure* fig)
{
    if(!fig) return;
    if(fig->programID) glDeleteProgram(fig->programID);
    if(fig->window)    glfwDestroyWindow(fig->window);
    glfwTerminate();
    free(fig);
}