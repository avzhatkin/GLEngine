//
//  shaders.h
//  CppTest
//
//  Created by Андрей Жаткин on 08.01.15.
//  Copyright (c) 2015 Андрей Жаткин. All rights reserved.
//

#ifndef CppTest_shaders_h
#define CppTest_shaders_h

static const char vert_color_passthrough[] =
"#version 120                                                   \n\
                                                                \n\
attribute vec3 position;                                        \n\
attribute vec3 color;                                           \n\
                                                                \n\
varying vec3 fragmentColor;                                     \n\
                                                                \n\
void main(void)                                                 \n\
{                                                               \n\
  // перевод вершинных координат в однородные                   \n\
  gl_Position = vec4(position, 1.0);                            \n\
  // передаем цвет вершины в фрагментный шейдер                 \n\
  fragmentColor = color;                                        \n\
}                                                               \n\
";

static const char frag_color_passthrough[] =
"#version 120                                                   \n\
                                                                \n\
varying vec3 fragmentColor;                                     \n\
                                                                \n\
void main(void)                                                 \n\
{                                                               \n\
  // зададим цвет пикселя                                       \n\
  gl_FragColor = vec4(fragmentColor, 1.0);                      \n\
}                                                               \n\
";

#endif
