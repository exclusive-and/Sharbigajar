
// Shabigajar.TestShaders

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>

#include "Effectno.h"
#include "Text.h"
#include "Shaders.h"



EffectType effectno = AllOK;
int effectInfo;

int main (void) {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow
        ( "Hello, world"
        , SDL_WINDOWPOS_CENTERED
        , SDL_WINDOWPOS_CENTERED
        , 720
        , 480
        , SDL_WINDOW_OPENGL );
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);

    GLenum status = glewInit ();

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


    const AttribBinding bindings[] = {
        {"position", 0}
    };

    bindAttribs (1, bindings, program);
}
