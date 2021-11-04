
#ifndef SHARBIGAJAR_BACKEND_SHADERS_H
#define SHARBIGAJAR_BACKEND_SHADERS_H

#include <GL/glew.h>



// General shader information.
//
typedef struct ShaderInfo ShaderInfo;

struct ShaderInfo {
    Text filename;
    GLenum shaderType;

    Text description;
};

ShaderInfo newShaderInfo (GLenum, const char [], const char []);


const GLuint compileShaderProgram (unsigned int, const ShaderInfo []);


// Shader program attribute bindings information.
//
typedef struct AttribBinding AttribBinding;

struct AttribBinding {
    const char *name;
    const GLuint bindPoint;
};

const GLuint bindAttribs (unsigned int, const AttribBinding [], const GLuint);

#endif
