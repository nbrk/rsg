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
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <glib-object.h>
#include <rsg/rsg.h>

#define rsgMalloc(x) rsgMallocDbg(x, __FILE__, __LINE__)
#define rsgCalloc(x, y) rsgCallocDbg(x, y, __FILE__, __LINE__)
#define rsgRealloc(x, y) rsgReallocDbg(x, y, __FILE__, __LINE__)
#define rsgFree(x) rsgFreeDbg(x, __FILE__, __LINE__)

/*******************************************************************************
 * DATA.
 */
typedef struct {
  GLuint program;
  mat4s u_view;
  mat4s u_projection;
} RsgLocalContext;

typedef struct {
  GLFWwindow* window;
  size_t totalTraversals;
} RsgGlobalContext;

typedef struct {
  RsgGlobalContext* global;
  RsgLocalContext* local;
} RsgContext;

struct RsgClosure {
  GClosure* gclosure;
  void* data;
};

G_DECLARE_DERIVABLE_TYPE(RsgAbstractNode,
                         rsg_abstract_node,
                         RSG,
                         ABSTRACT_NODE,
                         GObject)
#define RSG_TYPE_ABSTRACT_NODE rsg_abstract_node_get_type()
struct _RsgAbstractNodeClass {
  GObjectClass parent_class;

  /* Class virtual function fields. */
  void (*processFunc)(RsgAbstractNode* node, RsgContext* ctx);
  //  void (*setPropertyFunc)(RsgAbstractNode* node, const char* name,
  //                          RsgValue value);
  //  RsgValue (*getPropertyFunc)(RsgAbstractNode* node, const char* name);

  /* Padding to allow adding up to 12 new virtual functions without
   * breaking ABI. */
  gpointer padding[12];
};

/*******************************************************************************
 * FUNCTIONS.
 */
extern void* rsgCallocDbg(size_t number,
                          size_t size,
                          const char* file,
                          int line);
extern void* rsgMallocDbg(size_t size, const char* file, int line);
extern void* rsgReallocDbg(void* mem, size_t size, const char* file, int line);
extern void rsgFreeDbg(void* mem, const char* file, int line);

extern RsgGlobalContext* rsgGetGlobalContext(void);
extern void rsgSetGlobalContext(RsgGlobalContext* gctx);

extern void rsgLocalContextReset(RsgLocalContext* lctx);

extern GValue rsgValueToGValue(RsgValue value);
extern RsgValue rsgGValueToValue(GValue value);

extern GType vec2s_get_type(void);
extern GType vec3s_get_type(void);
extern GType vec4s_get_type(void);
extern GType mat4s_get_type(void);

GLuint rsgShaderProgramAssembleFromStrings(const char* vertexString,
                                           const char* fragmentString);
GLuint rsgShaderProgramAssembleFromFiles(const char* vertexPath,
                                         const char* fragmentPath);
