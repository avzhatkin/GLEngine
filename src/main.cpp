//
//  main.cpp
//  GLEngine
//
//  Created by Андрей Жаткин on 06.01.15.
//  Copyright (c) 2015 Андрей Жаткин. All rights reserved.
//

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdint>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
using std::string;
//
#include "shaders.h"

class GLShader
{
public:
  GLShader(const GLenum type, const char *source) : m_shader(-1), m_error(0)
  {
    // создадим шейдер
    m_shader = glCreateShader(type);

    // зададим шейдеру исходный код и скомпилируем его
    glShaderSource(m_shader, 1, (const GLchar**)&source, NULL);
    glCompileShader(m_shader);

    // проверим статус компиляции шейдера
    GLint status, length;
    GLchar buffer[1024];

    // получим статус шейдера
    glGetShaderiv(m_shader, GL_COMPILE_STATUS, &status);

    // в случае ошибки запишем ее в лог-файл
    if (status != GL_TRUE)
    {
      glGetShaderInfoLog(m_shader, 1024, &length, buffer);
      std::cout << "Shader error: " << buffer << std::endl;

      // проверим не было ли ошибок OpenGL
      m_error = glGetError();

      // сохраним статус
      m_error = m_error == GL_NO_ERROR ? GL_INVALID_VALUE : m_error;
    }
  }

  GLint Status() {return m_error;}

  operator GLint(){return m_shader;}

  ~GLShader() {if(-1 != m_shader) glDeleteShader(m_shader);}
private:
  GLShader() : m_shader(-1),m_error(0) {}
protected:
  GLint m_shader;
  GLint m_error;
};

class GLShaderProgram
{
public:
  GLShaderProgram() : m_program(-1)
  {
    // создадим программу
    m_program = glCreateProgram();
  }

  void AttachShader(const GLint shader)
  {
    glAttachShader(m_program, shader);
  }

  GLint Link(bool validate = true)
  {
    glLinkProgram(m_program);

    GLint status, length, error = 0;
    GLchar buffer[1024];

    // получим статус шейдерной программы
    glGetProgramiv(m_program, GL_LINK_STATUS, &status);

    // в случае ошибки запишем ее в лог-файл
    if (status != GL_TRUE)
    {
      glGetProgramInfoLog(m_program, 1024, &length, buffer);
      std::cout << "Shader program error: " << buffer << std::endl;

      // проверим не было ли ошибок OpenGL
      error = glGetError();

      // вернем статус
      error = error == GL_NO_ERROR ? GL_INVALID_VALUE : error;
    }

    if(!validate)
      return error;

    glValidateProgram(m_program);

    // получим статус шейдерной программы
    glGetProgramiv(m_program, GL_VALIDATE_STATUS, &status);

    // в случае ошибки запишем ее в лог-файл
    if (status != GL_TRUE)
    {
      glGetProgramInfoLog(m_program, 1024, &length, buffer);
      std::cout << "Shader program error: " << buffer << std::endl;

      // проверим не было ли ошибок OpenGL
      error = glGetError();

      // вернем статус
      error = error == GL_NO_ERROR ? GL_INVALID_VALUE : error;
    }

    return error;
  }

  operator GLint(){return m_program;}

  ~GLShaderProgram() {if(-1 != m_program) glDeleteProgram(m_program);}
protected:
  GLint m_program;
};
//

static void error_callback(int error, const char* description)
{
  fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
}

// проверка статуса param шейдерной программы program
GLint ShaderProgramStatus(GLuint program, GLenum param)
{
  GLint status, length;
  GLchar buffer[1024];

  // получим статус шейдерной программы
  glGetProgramiv(program, param, &status);

  // в случае ошибки запишем ее в лог-файл
  if (status != GL_TRUE)
  {
    glGetProgramInfoLog(program, 1024, &length, buffer);
    std::cout << "Shader program error: " << buffer << std::endl;
  }

  // проверим не было ли ошибок OpenGL
  GLint error = glGetError();

  // вернем статус
  return error == GL_NO_ERROR ? GL_INVALID_VALUE : error;
}

int main(void)
{
  GLFWwindow* window;
  glfwSetErrorCallback(error_callback);
  if (!glfwInit())
    exit(EXIT_FAILURE);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);
  string version((char*)glGetString(GL_VERSION));
  glewExperimental = GL_TRUE;
  glewInit();
  glfwSetWindowTitle(window, version.c_str());
  glfwSetKeyCallback(window, key_callback);
  //
  GLShader vsh(GL_VERTEX_SHADER, vert_color_passthrough);
  if(GL_NO_ERROR != vsh.Status())
  {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  GLShader fsh(GL_FRAGMENT_SHADER, frag_color_passthrough);
  if(GL_NO_ERROR != fsh.Status())
  {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  GLShaderProgram prog;
  prog.AttachShader(vsh);
  prog.AttachShader(fsh);
  if(GL_NO_ERROR != prog.Link(false))
  {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  //
  // количество вершин в нашей геометрии, у нас простой треугольник
  const int vertexCount = 3;

  // размер одной вершины меша в байтах - 6 float на позицию и на цвет вершины
  const int vertexSize = 6 * sizeof(float);

  // подготовим данные для вывода треугольника, всего 3 вершины по 6 float на каждую
  static const float triangleMesh[vertexCount * vertexSize] = {
    /* 1 вершина, позиция: */ -1.0f, -1.0f, 0.0f, /* цвет: */ 1.0f, 0.0f, 0.0f,
    /* 2 вершина, позиция: */ -1.0f,  3.0f, 0.0f, /* цвет: */ 0.0f, 1.0f, 0.0f,
    /* 3 вершина, позиция: */  3.0f, -1.0f, 0.0f, /* цвет: */ 0.0f, 0.0f, 1.0f
  };

  // переменные для хранения индексов VAO и VBO
  GLuint meshVAO, meshVBO;

  // создадим Vertex Array Object (VAO)
  glGenVertexArrays(1, &meshVAO);

  // установим созданный VAO как текущий
  glBindVertexArray(meshVAO);

  // создадим Vertex Buffer Object (VBO)
  glGenBuffers(1, &meshVBO);

  // устанавливаем созданный VBO как текущий
  glBindBuffer(GL_ARRAY_BUFFER, meshVBO);

  // заполним VBO данными треугольника
  glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSize, triangleMesh, GL_STATIC_DRAW);

  // укажем параметры доступа вершинного атрибута к VBO
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, (const GLvoid*)0);

  // разрешим использование атрибута
  glEnableVertexAttribArray(0);

  // укажем параметры доступа вершинного атрибута к VBO
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize, (const GLvoid*) (3 * sizeof(float)));

  // разрешим использование атрибута
  glEnableVertexAttribArray(1);

  //
  glClearColor(1.f, 1.f, 1.f, 0.f);
  while (!glfwWindowShouldClose(window))
  {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //
    // для рендеринга исопльзуем VAO
    glBindVertexArray(meshVAO);

    // делаем шейдерную программу активной
    glUseProgram(prog);

    // рендер треугольника из VBO привязанного к VAO
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    //
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
