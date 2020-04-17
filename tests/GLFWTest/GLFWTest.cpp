// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

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
