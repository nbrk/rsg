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

#include <rsg/rsg.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <sys/queue.h>

#define RSG_LOCAL_CONTEXT_MAX_UNIFORMS 16

/*******************************************************************************
 * DATA.
 */
/**
 * @brief Library global context
 */
typedef struct {
  GLFWwindow* window;
  size_t totalTraversals;
} RsgGlobalContext;

/**
 * @brief Context local to a subtree
 */
typedef struct {
  /*
   * OpenGL state machine related settings used in various nodes
   */
  GLuint program;
  size_t numUniforms;

  /*
   * Uniforms passed to shaders in drawing nodes
   */
  const char* uniformNames[RSG_LOCAL_CONTEXT_MAX_UNIFORMS];
  RsgValue uniformValues[RSG_LOCAL_CONTEXT_MAX_UNIFORMS];
} RsgLocalContext;

/**
 * @brief Connection from this node's property to another node's property
 */
typedef struct RsgPropertyConnection RsgPropertyConnection;
struct RsgPropertyConnection {
  const char* name;
  RsgNode* targetNode;
  const char* targetName;
  RsgValue (**adapterFuncs)(RsgValue val);
  size_t numAdapterFuncs;
  STAILQ_ENTRY(RsgPropertyConnection) entries;
};

struct RsgNode {
  const char* (*getTypeFunc)(void);
  void (*destroyFunc)(RsgNode* node);
  RsgValue (*getPropertyFunc)(RsgNode* node, const char* name);
  void (*setPropertyFunc)(RsgNode* node, const char* name, RsgValue val);
  void (*processFunc)(RsgNode* node,
                      RsgLocalContext* lctx,
                      RsgGlobalContext* gctx);
  STAILQ_HEAD(, RsgPropertyConnection) propertyConnections;
};

/*******************************************************************************
 * FUNCTIONS.
 */
/**
 * @brief Initialize a base node structure with default values
 * @param node
 */
extern void rsgNodeSetDefaults(RsgNode* node);

/**
 * @brief Init the local context with default values
 * @param lctx
 */
void rsgLocalContextSetDefaults(RsgLocalContext* lctx);

/**
 * @brief Set/insert/overwrite a uniform value in the local context
 * @param lctx
 * @param name
 * @param value
 */
extern void rsgLocalContextSetUniform(RsgLocalContext* lctx,
                                      const char* name,
                                      RsgValue value);

/**
 * @brief Make a deep copy (backup) of the context, its internal lists, etc.
 * @param lctx
 * @return
 */
extern RsgLocalContext* rsgLocalContextCreateCopy(RsgLocalContext* origlctx);

/**
 * @brief Destroy local context's variables, lists, etc. and free the memory
 * @param lctx
 */
extern void rsgLocalContextDestroy(RsgGlobalContext* lctx);
