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

#define RSG_INIT_FLAG_FULLSCREEN 1
#define RSG_INIT_FLAG_HIDECURSOR 2

/*******************************************************************************
 * DATA.
 */
/**
 * @brief Supported value types
 */
typedef enum {
  RSG_VALUE_POINTER,
  RSG_VALUE_INT,
  RSG_VALUE_FLOAT,
  RSG_VALUE_VEC2,
  RSG_VALUE_VEC3,
  RSG_VALUE_VEC4,
  RSG_VALUE_MAT4,
} RsgValueType;

/**
 * @brief Value container
 */
typedef struct {
  RsgValueType type;
  union {
    void* asPointer;
    int asInt;
    float asFloat;
    vec2s asVec2;
    vec3s asVec3;
    vec4s asVec4;
    mat4s asMat4;
  };
} RsgValue;

/**
 * @brief Opaque node type
 */
typedef struct RsgNode RsgNode;

/*******************************************************************************
 * FUNCTIONS.
 */
/*
 * Memory helpers
 */
extern void rsgMallocSetDebug(bool value);
extern void rsgMallocPrintStat(void);

/*
 * Value container helpers.
 */
extern char* rsgValueToString(RsgValue val);
extern RsgValue rsgValuePointer(void* val);
extern RsgValue rsgValueInt(int val);
extern RsgValue rsgValueFloat(float val);
extern RsgValue rsgValueVec2(vec2s val);
extern RsgValue rsgValueVec3(vec3s val);
extern RsgValue rsgValueVec4(vec4s val);
extern RsgValue rsgValueMat4(mat4s val);

/*
 * Initialization, library global parameters and main loop.
 */
extern void rsgInit(int width, int height, int flags);
extern void rsgMainLoop(RsgNode* root, int traversalFreq);
extern int rsgGetScreenWidth(void);
extern int rsgGetScreenHeight(void);

/*
 * Properties of nodes
 */
extern RsgValue rsgNodeGetProperty(RsgNode* node, const char* name);
extern void rsgNodeSetProperty(RsgNode* node, const char* name, RsgValue value);
extern void rsgNodeBindProperty(RsgNode* node,
                                const char* name,
                                RsgNode* toNode,
                                const char* toName);

/*
 * Callback node
 */
extern RsgNode* rsgCallbackNodeCreate(void (*func)(void*), void* cookie);

/*
 * Group node
 */
extern RsgNode* rsgGroupNodeCreate(void);
extern void rsgGroupNodeAddChild(RsgNode* groupNode, RsgNode* childNode);

/*
 * Screen node
 */
extern RsgNode* rsgScreenNodeCreate(void);

/*
 * Camera node
 */
extern RsgNode* rsgCameraNodeCreatePerspectiveDefault(void);
extern RsgNode* rsgCameraNodeCreateOrthographicDefault(void);
extern RsgNode* rsgCameraNodeCreate(vec3s position,
                                    float horizAngle,
                                    float vertAngle,
                                    float fov,
                                    float aspect,
                                    float nearPlane,
                                    float farPlane,
                                    bool perspective);

/*
 * Shader node
 */
extern RsgNode* rsgShaderNodeCreate(unsigned int program);
extern RsgNode* rsgShaderNodeCreateFromMemory(const char* vertexText,
                                              const char* fragmentText);
extern RsgNode* rsgShaderNodeCreateFromFiles(const char* vertexPath,
                                             const char* fragmentPath);

/*
 * Mouse manipulator node
 */
extern RsgNode* rsgMouseManipulatorNodeCreate(void);

/*
 * Property printer/debugger node
 */
extern RsgNode* rsgPropertyPrinterNodeCreate(void);

/*
 * Mesh draw node.
 */
extern RsgNode* rsgMeshNodeCreateTriangle(void);
