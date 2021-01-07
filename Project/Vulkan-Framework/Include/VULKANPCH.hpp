/****************************************************************************/
/*!
\file
   VULKANPCH.hpp
\Author
   Ryan Dugie
\brief
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0

    Precompiled Header, all common headers / functionality
*/
/****************************************************************************/

#ifndef VULKANPCH_PCH
#define VULKANPCH_PCH
#pragma once
#pragma warning(push, 0)

// Vulkan & GLFW
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include "glfw3.h"
#include "glfw3native.h"

// GLM
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

// STL Includes
#include <algorithm>
#include <memory>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cassert>
#include <cstdint>
#include <stdexcept>

// CMath
#define _USE_MATH_DEFINES
#include <cmath>
#ifndef PI
#define PI 3.14159265359f
#endif
#undef _USE_MATH_DEFINES

#pragma warning(pop) // warning(push, 0)

//! SILENCE WARNING (expression, warning)
#define SILENCE_WARNING(x, w)         \
        __pragma(warning(push))       \
        __pragma(warning(disable : w))\
                x                     \
        __pragma(warning(pop))        \

#define UNUSED(x) (void)(x)

// DEBUG
#include "Log.hpp"

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#endif