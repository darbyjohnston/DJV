//------------------------------------------------------------------------------
// Copyright (c) 2020 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <glm/vec2.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <chrono>
#include <iostream>
#include <stdexcept>

namespace
{
    const glm::ivec2 windowSize(1280, 720);
    GLFWwindow* glfwWindow = nullptr;
    const size_t frameRate = 24;

    void glfwErrorCallback(int error, const char* description)
    {
        std::cout << "GLFW error " << error << ": " << description << std::endl;
    }

} // namespace

int main(int argc, char ** argv)
{
    int r = 0;
    try
    {
        glfwSetErrorCallback(glfwErrorCallback);
        if (!glfwInit())
        {
            throw std::runtime_error("Cannot initialize GLFW.");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 1);
        glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
        glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
        glfwWindow = glfwCreateWindow(
            windowSize.x,
            windowSize.y,
            "GLFWTest", NULL, NULL);
        if (!glfwWindow)
        {
            throw std::runtime_error("Cannot create window.");
        }
        glfwMakeContextCurrent(glfwWindow);
        glfwSwapInterval(1);
        glfwShowWindow(glfwWindow);

        bool run = true;
        auto start = std::chrono::steady_clock::now();
        auto delta = std::chrono::microseconds(0);
        auto frameTime = std::chrono::microseconds(1000000 / frameRate);
        while (run && glfwWindow && !glfwWindowShouldClose(glfwWindow))
        {
            glfwPollEvents();
            glfwSwapBuffers(glfwWindow);

            auto end = std::chrono::steady_clock::now();
            delta = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            while (delta < frameTime)
            {
                end = std::chrono::steady_clock::now();
                delta = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            }
            std::cout << "frame: " << delta.count() << "/" << (1000000 / frameRate) << std::endl;
            start = end;
        }

        r = 1;
    }
    catch (const std::exception & e)
    {
        std::cout << e.what();
    }
    return r;
}
