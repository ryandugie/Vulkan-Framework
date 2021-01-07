/****************************************************************************/
/*!
\file
   Simple.frag
\Author
   Ryan Dugie
\brief
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0
*/
/****************************************************************************/
#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl : enable

layout (location = 0) in vec4 normal;
layout (location = 0) out vec4 color;

void main()
{  
  color = normal;
}