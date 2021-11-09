
// Sharbigajar.Backend.Renderer

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include "Effectno.h"



// type GraphicsState

// Create a new graphics state machine. Generate all the necessary buffers.
//
GraphicsState newGraphicsState (const GLuint program) {
    GLuint vertexArray;
    glGenVertexArrays (1, &vertexArray);

    GLuint vertexBuffer, indexBuffer;
    glGenBuffers (1, &vertexBuffer);
    glGenBuffers (1, &indexBuffer);

    return (GraphicsState) {
        .program        = program,
        .vertexArray    = vertexArray,
        .vertexBuffer   = vertexBuffer,
        .indexBuffer    = indexBuffer
    };
}

// Free all the resources used by a graphics state machine.
//
void freeGraphicsState (GraphicsState gfxstate) {
    glDeleteBuffers (1, &gfxstate.vertexBuffer);
    glDeleteBuffers (1, &gfxstate.indexBuffer);

    glDeleteVertexArrays (1, &gfxstate.vertexArray);
}

// Bind all the buffers of this graphics state to be used in subsequent
// GL calls.
//
void bindGraphicsState (GraphicsState gfxstate) {
    glUseProgram (gfxstate.program);

    glBindVertexArray (gfxstate.vertexArray);

    glBindBuffer
        ( GL_ARRAY_BUFFER
        , gfxstate.vertexBuffer );
    glBindBuffer
        ( GL_ELEMENT_ARRAY_BUFFER
        , gfxstate.indexBuffer );
}


// type Mesh

// Render the mesh using a particular graphics state.
// This just copies to the buffers and doesn't run draw calls.
//
GraphicsState renderMeshWith (GraphicsState gfxstate, Mesh mesh) {
    bindGraphicsState (gfxstate);

    glBufferData
        ( GL_ARRAY_BUFFER
        , sizeof (Vec3Float) * numVertices
        , mesh.vertices
        , GL_DYNAMIC_DRAW);

    glBufferData
        ( GL_ELEMENT_ARRAY_BUFFER
        , sizeof (unsigned int) * mesh.numIndices
        , mesh.indices
        , GL_DYNAMIC_DRAW );

    return gfxstate;
}

