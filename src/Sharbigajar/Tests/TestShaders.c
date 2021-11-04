
// Shabigajar.Tests.TestShaders

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>

#include "Effectno.h"
#include "Text.h"
#include "Backend/Shaders.h"



typedef struct Window Window;

struct Window {
    SDL_Window *me;
    SDL_GLContext glcontext;
};

Window sdlStartup (void) {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_Window *window = SDL_CreateWindow
        ( "Hello, world"
        , SDL_WINDOWPOS_CENTERED
        , SDL_WINDOWPOS_CENTERED
        , 720
        , 480
        , SDL_WINDOW_OPENGL );

    SDL_GLContext glcontext = SDL_GL_CreateContext(window);

    return (Window) {window, glcontext};
}

GLenum glStartup (SDL_GLContext glcontext) {
    GLenum status = glewInit ();

    glDisable (GL_CULL_FACE);
    glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);

    return status;
}

int main (void) {
    Window win = sdlStartup ();
    GLenum glStatus = glStartup (win.glcontext);

    // Compile shader program.
    const ShaderInfo progInfo[] = {
        newShaderInfo
            ( GL_VERTEX_SHADER
            , "TestVertexShader.glsl"
            , "A test vertex shader" ),
        newShaderInfo
            ( GL_FRAGMENT_SHADER
            , "TestFragmentShader.glsl"
            , "A test fragment shader" )
    };

    const GLuint program = compileShaderProgram (2, progInfo);

    GLuint vertexArray;
    glGenVertexArrays (1, &vertexArray);
    glBindVertexArray (vertexArray);

    GLuint vertexBuffer, indexBuffer;
    glGenBuffers (1, &vertexBuffer);
    glGenBuffers (1, &indexBuffer);
    glBindBuffer (GL_ARRAY_BUFFER           , vertexBuffer  );
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER   , indexBuffer   );


    // Bind attributes and uniforms.
    const AttribBinding bindings[] = {
        {"position", 0}
    };

    bindAttribs (1, bindings, program);

    GLuint uColour = glGetUniformLocation (program, "colour");


    // Enter main loop.
    while (1) {
        glClearColor (0.129f, 0.102f, 0.141f, 1.0f);
        glClear (GL_COLOR_BUFFER_BIT);

        float colour[4] = { 1.f, 1.f, 1.f, 1.f };

        glUseProgram (program);
        glUniform4fv (uColour, 1, colour);

        float vertices[6] = {
            -0.5, -0.5,
             0  ,  0.5,
             0.5, -0.5
        };

        unsigned int indices[3] = {0, 1, 2};

        glEnableVertexAttribArray (0);
        glVertexAttribPointer (0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glBufferData
            ( GL_ARRAY_BUFFER
            , 6 * sizeof (float)
            , vertices
            , GL_DYNAMIC_DRAW );
        glBufferData
            ( GL_ELEMENT_ARRAY_BUFFER
            , 3 * sizeof (unsigned int)
            , indices
            , GL_DYNAMIC_DRAW );
        glDrawElements (GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        SDL_GL_SwapWindow (win.me);
    }
}
