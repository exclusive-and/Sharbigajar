
#ifndef SHARBIGAJAR_BACKEND_RENDERER_H
#define SHARBIGAJAR_BACKEND_RENDERER_H

#include <GL/glew.h>



// State of the graphics state machine.
//
typedef struct GraphicsState GraphicsState;

struct GraphicsState {
    GLuint program;
    GLuint vertexArray;

    GLuint vertexBuffer;
    GLuint indexBuffer;
};

GraphicsState newGraphicsState (const GLuint);

void freeGraphicsState (GraphicsState);

void bindGraphicsState (GraphicsState);


// Mesh object to draw to the screen.
//
typedef struct Mesh Mesh;

struct Mesh {
    Vector3 *vertices;
    unsigned int *indices;
};

GraphicsState renderMeshWith (GraphicsState, Mesh);

#endif
