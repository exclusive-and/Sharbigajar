
// Sharbigajar.Shaders

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

#include "Effectno.h"
#include "Text.h"
#include "Shaders.h"



// type ShaderInfo

// Create a new shader info by wrapping strings in text objects.
//
ShaderInfo newShaderInfo
    (GLenum shaderType, const char filename[], const char description[])
{
    return (ShaderInfo) {
        .filename       = textFromString (filename),
        .shaderType     = shaderType,
        .description    = textFromString (description)
    };
}


// Load a GL shader from a file and compile it.
//
static const GLuint loadGLShader (ShaderInfo info)
{
    Text shaderSrc = textFromFile (info.filename);
    if (effectno == IOError) {
        printf
            ( "error: loadGLShader: failed to open %s\n", info.filename.array );
        return 0;
    }

    GLuint shaderHandle = glCreateShader (info.shaderType);
    if (shaderHandle == 0) {
        effectno = ShaderCreateError;
        printf ("error: loadGLShader: failed to create shader\n");
        return 0;
    }

    // Arrange and compile the shader's source text.
    // I wish OpenGL didn't do this weird split-arrays thing.
    const GLchar *shaderSourceStrings[] = {
        shaderSrc.array
    };

    const GLint shaderSourceLengths[] = {
        shaderSrc.length
    };

    glShaderSource (shaderHandle, 1, shaderSourceStrings, shaderSourceLengths);
    glCompileShader (shaderHandle);

    // Report any shader compilation errors.
    int success;
    glGetShaderiv (shaderHandle, GL_COMPILE_STATUS, &success);
    if (!success) {
        effectno = ShaderCompileError;

        printf
            ( "error: loadGLShader: shader compilation error\n"
              "       in shader file: %s\n"
              "       GL reported:\n", info.filename.array );

        GLint logLen = 0;
        glGetShaderiv (shaderHandle, GL_INFO_LOG_LENGTH, &logLen);

        GLchar *log = (GLchar *) malloc (logLen);
        glGetShaderInfoLog (shaderHandle, logLen, &logLen, log);
        printf ("%s\n", log);

        free (log);

        glDeleteShader (shaderHandle);
    }

    freeText (shaderSrc);

    return shaderHandle;
}


// Compile a list of shaders into a GL program.
//
const GLuint compileShaderProgram
    (unsigned int numShaders, ShaderInfo infos[])
{
    GLuint program = glCreateProgram ();
    GLuint *shaders = (GLuint *) malloc (numShaders * sizeof (GLuint));

    for (unsigned int shaderIx = 0; shaderIx < numShaders; shaderIx++) {
        const GLuint shader = loadGLShader (infos[shaderIx]);
        // ^ TODO: Handle side-effects
        glAttachShader (program, shader);
        shaders[shaderIx] = shader;
    }

    glLinkProgram (program);

    for (unsigned int shaderIx = 0; shaderIx < numShaders; shaderIx++)
        glDetachShader (program, shaders[shaderIx]);

    free (shaders);

    glValidateProgram (program);
    return program;
}


// type AttribBinding

// Create attribute bindings within a GL program.
//
const GLuint bindAttribs
    (unsigned int numAttribs, AttribBinding bindings[], const GLuint program)
{
    glUseProgram (program);

    for (unsigned int attrIx = 0; attrIx < numAttribs; attrIx++)
        glBindAttribLocation
            (program, bindings[attrIx].bindPoint, bindings[attrIx].name);

    glUseProgram (0);

    return program;
}


