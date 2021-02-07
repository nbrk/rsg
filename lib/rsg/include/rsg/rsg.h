/*
 * Copyright 2021 Nikolay Burkov <nbrk@linklevel.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#pragma once

/*
 *
 * RSG (Reactive/retained Scene Graph) library: public API
 *
 */
#include <cglm/struct.h>
#include <stdbool.h>
#include <stdlib.h>

#define rsgMalloc(x) rsgMallocDbg(x, __FILE__, __LINE__)
#define rsgCalloc(x, y) rsgCallocDbg(x, y, __FILE__, __LINE__)
#define rsgRealloc(x, y) rsgReallocDbg(x, y, __FILE__, __LINE__)
#define rsgFree(x) rsgFreeDbg(x, __FILE__, __LINE__)

#define RSG_INIT_FLAG_FULLSCREEN 1
#define RSG_INIT_FLAG_HIDECURSOR 2

/*******************************************************************************
 * DATA.
 */
/**
 * @brief Possible value type
 */
typedef enum {
  RSG_VALUE_INT = 1,
  RSG_VALUE_FLOAT,
  RSG_VALUE_VEC2,
  RSG_VALUE_VEC3,
  RSG_VALUE_VEC4,
  RSG_VALUE_MAT4,
} RsgValueType;

/**
 * @brief Value pass in/pass out container used throughout the library
 */
typedef struct {
  RsgValueType type;
  union {
    int asInt;
    float asFloat;
    vec2s asVec2;
    vec3s asVec3;
    vec4s asVec4;
    mat4s asMat4;
  };
} RsgValue;

/**
 * @brief Function pointers used as adapters to convert Value data on the fly
 */
typedef RsgValue (*RsgValueAdapterFunc)(RsgValue);

/**
 * @brief Opaque base node handle (every node must contain this at offset zero)
 */
typedef struct RsgNode RsgNode;

/**
 * @brief Node that runs a custom code with a cookie pointer arg, when processed
 */
typedef struct RsgCallbackNode RsgCallbackNode;

/**
 * @brief Node that groups other nodes. When processed, it preserves the local
 * context, processes the children from left to right, then restores the local
 * ctx (so no local changes are leaked back up into the tree/group's siblings).
 */
typedef struct RsgGroupNode RsgGroupNode;

/**
 * @brief Node that draws a mesh geometry using values from the local context.
 */
typedef struct RsgMeshNode RsgMeshNode;

/**
 * @brief Node that, when processed, sets a list of uniforms in the local ctx
 */
typedef struct RsgUniformSetterNode RsgUniformSetterNode;

/**
 * @brief Node that sets a shader program in the local context
 */
typedef struct RsgShaderProgramNode RsgShaderProgramNode;

/**
 * @brief Convenience node that sets Model/View matrix uniforms in the local
 * context and exposes them via property system
 */
typedef struct RsgCameraNode RsgCameraNode;

/**
 * @brief Node that changes its "x", "y", etc. properties on mouse motions
 */
typedef struct RsgTrackballManipulatorNode RsgTrackballManipulatorNode;

/**
 * @brief Node that spews key scancodes (via props) on keyboard state changes
 */
typedef struct RsgKeyboardManipulatorNode RsgKeyboardManipulatorNode;

/**
 * @brief Node that strobes the system
 */
typedef struct RsgClockNode RsgClockNode;
/*******************************************************************************
 * FUNCTIONS.
 */
/*
 * Memory helpers
 */
extern void* rsgCallocDbg(size_t number,
                          size_t size,
                          const char* file,
                          int line);
extern void* rsgMallocDbg(size_t size, const char* file, int line);
extern void* rsgReallocDbg(void* mem, size_t size, const char* file, int line);
extern void rsgFreeDbg(void* mem, const char* file, int line);
extern void rsgMallocSetDebug(bool value);
extern void rsgMallocPrintStat(void);

/*
 * Initialization, library global parameters and main loop.
 */
extern void rsgInit(int width, int height, int flags);
/**
 * @brief Enter the main event loop. The tree is traversed in response to the
 * backend events or automatically `traverseFreq' times per second
 * @param rootNode
 * @param traverseFreq if zero, then run in "retained mode" which only traverses
 * the tree when the GUI/inputs is changed. Otherwise, run in "immediate mode"
 * and traverse the tree `traverseFreq' times per second.
 */
extern void rsgMainLoop(RsgNode* rootNode, int traverseFreq);
extern int rsgGetScreenWidth(void);
extern int rsgGetScreenHeight(void);

/*
 * Value construction/adaptation/conversion helpers
 */
extern char* rsgValueToString(RsgValue val);
extern RsgValue rsgValueInt(int val);
extern RsgValue rsgValueFloat(float val);
extern RsgValue rsgValueVec2(vec2s val);
extern RsgValue rsgValueVec3(vec3s val);
extern RsgValue rsgValueVec4(vec4s val);
extern RsgValue rsgValueMat4(mat4s val);
extern RsgValueAdapterFunc rsgValueAdapterVec2ProjectX(void);
extern RsgValueAdapterFunc rsgValueAdapterVec2ProjectY(void);

/*
 * Base node functions
 */
extern const char* rsgNodeGetType(RsgNode* node);
extern RsgValue rsgNodeGetProperty(RsgNode* node, const char* name);
extern void rsgNodeSetProperty(RsgNode* node, const char* name, RsgValue val);
extern void rsgNodeConnectProperty(RsgNode* node,
                                   const char* name,
                                   RsgNode* targetNode,
                                   const char* targetName);
extern void rsgNodeConnectPropertyWithAdapter(RsgNode* node,
                                              const char* name,
                                              RsgNode* targetNode,
                                              const char* targetName,
                                              RsgValue (**funcs)(RsgValue),
                                              size_t nfuncs);

/*
 * Callback node functions
 */
extern RsgCallbackNode* rsgCallbackNodeCreate(void (*func)(void*),
                                              void* cookie);

/*
 * Group node functions
 */
extern RsgGroupNode* rsgGroupNodeCreate(void);
extern void rsgGroupNodeAddChild(RsgGroupNode* node, RsgNode* childNode);

/*
 * Mesh node functions
 */
extern RsgMeshNode* rsgMeshNodeCreateTriangle(void);

/*
 * Shader Uniform setter node functions
 */
extern RsgUniformSetterNode* rsgUniformSetterNodeCreate(const char** names,
                                                        RsgValue* values,
                                                        size_t numValues);
/*
 * Shader node functions
 */
extern RsgShaderProgramNode* rsgShaderProgramNodeCreate(
    const char* vertexText,
    const char* fragmentText);

/*
 * Camera node functions
 */
extern RsgCameraNode* rsgCameraNodeCreatePerspectiveDefault(float aspect);
extern RsgCameraNode* rsgCameraNodeCreateOrthographicDefault(float aspect);
RsgCameraNode* rsgCameraNodeCreate(vec3s position,
                                   float horizAngle,
                                   float vertAngle,
                                   float fov,
                                   float aspect,
                                   float nearPlane,
                                   float farPlane,
                                   bool perspective);

/*
 * Trackball manipulator node functions
 */
extern RsgTrackballManipulatorNode* rsgTrackballManipulatorNodeCreate(void);

/*
 * Keyboard manipulator node functions
 */
extern RsgKeyboardManipulatorNode* rsgKeyboardManipulatorNodeCreate(void);
