/****************************************************************************/
/*!
\file
   Simple.vert
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
layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec4 aNormal;

layout(binding = 0) uniform MatrixBuffer 
{
    mat4 world;
    mat4 view;
    mat4 proj;
} ubo;

layout (location = 0) out vec4 normal;

void main()
{
    normal = normalize(aNormal);    
    gl_Position = ubo.proj * ubo.view * ubo.world * vec4(aPosition.x, -aPosition.y, aPosition.z, 1);
}