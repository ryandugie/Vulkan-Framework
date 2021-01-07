/****************************************************************************/
/*!
\file
   Engine.hpp
\Author
   Ryan Dugie
\brief
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0

    Parent class of the entire application
*/
/****************************************************************************/
#ifndef ENGINE_HPP
#define ENGINE_HPP
#pragma once

#include "Renderer.hpp"

namespace VK
{
    class Engine
    {
    public:

        Engine();
        void Init();
        void Run();
        void ShutDown();

    private:
        float UpdateDT();

        VK::Renderer mRenderer;
        WindowPtr mWindow = nullptr;

        double pDeltaTime;
        float pFPS;

        double pPreviousTime;
        float pFPSCalcInterval;
        float pGameLoopIterations;
        float pStartTime;
    };
}


#endif