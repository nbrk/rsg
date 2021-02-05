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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rsg_internal.h"

static size_t file_get_size(FILE* fp) {
  size_t curpos = ftell(fp);
  fseek(fp, 0, SEEK_END);
  size_t endpos = ftell(fp);
  fseek(fp, curpos, SEEK_SET);
  return endpos;
}

static char* file_get_contents(FILE* fp) {
  size_t sourcelen = file_get_size(fp);
  char* source = rsgMalloc(sourcelen + 1);
  size_t nobjs = fread(source, sourcelen, 1, fp);

  if (nobjs != 1) {
    rsgFree(source);
    return NULL;
  } else
    return source;
}

static int program_add_shader(GLuint program,
                              GLenum shader_type,
                              const char* shader_src) {
  GLuint shader = glCreateShader(shader_type);
  const GLchar* source[1] = {shader_src};
  const GLint length[1] = {(GLint)strlen(shader_src)};

  glShaderSource(shader, 1, source, length);
  glCompileShader(shader);

  GLint param_val;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &param_val);
  if (param_val != GL_TRUE) {
    // fetch and print the info log
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &param_val);
    GLchar* buffer = rsgMalloc((size_t)param_val);
    glGetShaderInfoLog(shader, param_val, NULL, buffer);
    glDeleteShader(shader);
    printf("Shader type %d compile error: %s\n", shader_type, buffer);
    rsgFree(buffer);
    return -1;
  }
  glAttachShader(program, shader);
  glDeleteShader(shader);  // marks the shader for deletion upon deletion of the
                           // program object
  return 0;
}

static GLuint program_create(const char* vertex_src, const char* fragment_src) {
  GLuint program;
  program = glCreateProgram();

  if (program_add_shader(program, GL_VERTEX_SHADER, vertex_src) != 0) {
    glDeleteProgram(program);
    return 0;
  }
  if (program_add_shader(program, GL_FRAGMENT_SHADER, fragment_src) != 0) {
    glDeleteProgram(program);
    return 0;
  }
  GLint param_val;
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &param_val);
  if (param_val != GL_TRUE) {
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &param_val);
    GLchar* buffer = rsgMalloc((size_t)param_val);
    glGetProgramInfoLog(program, param_val, NULL, buffer);
    glDeleteProgram(program);
    printf("Program link error: %s\n", buffer);
    rsgFree(buffer);
    return 0;
  }
  glValidateProgram(program);
  glGetProgramiv(program, GL_VALIDATE_STATUS, &param_val);
  if (param_val != GL_TRUE) {
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &param_val);
    GLchar* buffer = rsgMalloc((size_t)param_val);
    glGetProgramInfoLog(program, param_val, NULL, buffer);
    glDeleteProgram(program);
    printf("Program validation error: %s\n", buffer);
    rsgFree(buffer);
    return 0;
  }

  return program;
}

GLuint rsgShaderProgramAssembleFromStrings(const char* vertex_str,
                                           const char* fragment_str) {
  assert(vertex_str != NULL);
  assert(fragment_str != NULL);
  GLuint program = program_create(vertex_str, fragment_str);

  return program;
}

GLuint rsg_shader_program_assemble_from_files(const char* vertex_path,
                                              const char* fragment_path) {
  FILE* fp;

  fp = fopen(vertex_path, "r");
  assert(fp != NULL);
  char* vertex_str = file_get_contents(fp);
  fclose(fp);

  fp = fopen(fragment_path, "r");
  assert(fp != NULL);
  char* fragment_str = file_get_contents(fp);
  fclose(fp);

  GLuint program =
      rsgShaderProgramAssembleFromStrings(vertex_str, fragment_str);

  rsgFree(vertex_str);
  rsgFree(fragment_str);

  return program;
}
