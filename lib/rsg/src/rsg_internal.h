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
 * @brief Uniform value based on our Value machinery
 */
typedef struct RsgUniform RsgUniform;
struct RsgUniform {
  const char* name;
  RsgValue value;
  SLIST_ENTRY(RsgUniform) entries;
};

/**
 * @brief Context local to a subtree
 */
typedef struct {
  GLuint program;
  SLIST_HEAD(, RsgUniform) uniforms;
} RsgLocalContext;

/**
 * @brief Connection from this node's property to another node's property
 */
typedef struct RsgPropertyConnection RsgPropertyConnection;
struct RsgPropertyConnection {
  const char* name;
  RsgNode* targetNode;
  const char* targetName;
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
extern void rsgNodeInit(RsgNode* node);

void rsgLocalContextInitDefaults(RsgLocalContext* lctx);

/**
 * @brief Set/insert/overwrite a uniform value in the local context
 * @param lctx
 * @param u
 */
extern void rsgLocalContextSetUniform(RsgLocalContext* lctx,
                                      RsgUniform* uniform);

/**
 * @brief Make a deep copy (backup) of the context, its internal lists, etc.
 * @param lctx
 * @return
 */
extern RsgLocalContext* rsgLocalContextDeepCopy(RsgLocalContext* origlctx);
