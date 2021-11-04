
#ifndef SHARBIGAJAR_EFFECTNO_H
#define SHARBIGAJAR_EFFECTNO_H

// Type of side-effect being passed up the control graph.
typedef enum EffectType EffectType;

enum EffectType {
// Okay status
    AllOK,

// General errors
    StandardError,
    IOError,

// OpenGL errors
    ShaderCreateError,
    ShaderCompileError,
};


extern EffectType effectno;

extern int effectInfo;

#endif

