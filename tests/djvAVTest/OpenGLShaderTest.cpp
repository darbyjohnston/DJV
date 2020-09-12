// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/OpenGLShaderTest.h>

#include <djvAV/Color.h>
#include <djvAV/OpenGLShader.h>
#include <djvAV/Shader.h>

#include <djvCore/Error.h>

using namespace djv::Core;
using namespace djv::AV;
using namespace djv::AV::OpenGL;

namespace djv
{
    namespace AVTest
    {
        OpenGLShaderTest::OpenGLShaderTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::AVTest::OpenGLShaderTest", tempPath, context)
        {}
        
        void OpenGLShaderTest::run()
        {
            try
            {
                auto shader = Render::Shader::create(
                    "#version 100\n"
                    "\n"
                    "void main()\n"
                    "{\n"
                    "    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);\n"
                    "}\n",
                    "#version 100\n"
                    "\n"
                    "precision mediump float;\n"
                    "\n"
                    "uniform int _int;\n"
                    "uniform float _float;\n"
                    "uniform vec2 _vec2;\n"
                    "uniform vec3 _vec3;\n"
                    "uniform vec4 _vec4;\n"
                    "uniform mat3 _mat3;\n"
                    "uniform mat4 _mat4;\n"
                    "uniform vec4 _color;\n"
                    "uniform float _color4[4];\n"
                    "uniform int _intArray[4];\n"
                    "uniform int _floatArray[4];\n"
                    "uniform int _vec3Array[4];\n"
                    "uniform int _vec4Array[4];\n"
                    "\n"
                    "void main()\n"
                    "{\n"
                    "    gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);\n"
                    "}\n");
                auto glShader = Shader::create(shader);
                const GLuint program = glShader->getProgram();
                DJV_ASSERT(program);
                glShader->bind();
                
                GLint location = glGetUniformLocation(program, "_int");
                glShader->setUniform(location, 1);
                glShader->setUniform("_int", 1);
                
                location = glGetUniformLocation(program, "_float");
                glShader->setUniform(location, 1.F);
                glShader->setUniform("_float", 1.F);
                
                location = glGetUniformLocation(program, "_vec2");
                glShader->setUniform(location, glm::vec2(1.F, 2.F));
                glShader->setUniform("_vec2", glm::vec2(1.F, 2.F));
                
                location = glGetUniformLocation(program, "_vec3");
                glShader->setUniform(location, glm::vec3(1.F, 2.F, 3.F));
                glShader->setUniform("_vec3", glm::vec3(1.F, 2.F, 3.F));
                
                location = glGetUniformLocation(program, "_vec4");
                glShader->setUniform(location, glm::vec4(1.F, 2.F, 3.F, 4.F));
                glShader->setUniform("_vec4", glm::vec4(1.F, 2.F, 3.F, 4.F));
                
                location = glGetUniformLocation(program, "_mat3");
                glShader->setUniform(location, glm::mat3x3(1.F));
                glShader->setUniform("_mat3", glm::mat3(1.F));
                
                location = glGetUniformLocation(program, "_mat4");
                glShader->setUniform(location, glm::mat4x4(1.F));
                glShader->setUniform("_mat4", glm::mat4x4(1.F));
                
                location = glGetUniformLocation(program, "_color");
                glShader->setUniform(location, glm::vec4(.1F, .2F, .3F, 1.F));
                glShader->setUniform("_color", glm::vec4(.1F, .2F, .3F, 1.F));
                
                for (const auto i : {
                    Image::Type::L_U8,
                    Image::Type::LA_U8,
                    Image::Type::RGB_U8,
                    Image::Type::RGBA_U8,
                    Image::Type::L_F32,
                    Image::Type::LA_F32,
                    Image::Type::RGB_F32,
                    Image::Type::RGBA_F32 ,
                    Image::Type::RGBA_F16 })
                {
                    location = glGetUniformLocation(program, "_color");
                    glShader->setUniform(location, Image::Color(i));
                    glShader->setUniform("_color", Image::Color(i));
                }
                
                for (const auto i : {
                    Image::Type::L_F32,
                    Image::Type::LA_F32,
                    Image::Type::RGB_F32,
                    Image::Type::RGBA_F32 })
                {
                    location = glGetUniformLocation(program, "_color");
                    glShader->setUniformF(location, Image::Color(i));
                    glShader->setUniformF("_color", Image::Color(i));
                }

                location = glGetUniformLocation(program, "_color4");
                float color4[4] = { .1F, .2F, .3F, 1.F };
                glShader->setUniform(location, color4);
                glShader->setUniform("_color", color4);

                location = glGetUniformLocation(program, "_intArray");
                std::vector<int> intArray({ 1, 2, 3, 4 });
                glShader->setUniform(location, intArray);
                glShader->setUniform("_intArray", intArray);

                location = glGetUniformLocation(program, "_floatArray");
                std::vector<float> floatArray({ 1.F, 2.F, 3.F, 4.F });
                glShader->setUniform(location, floatArray);
                glShader->setUniform("_floatArray", floatArray);

                location = glGetUniformLocation(program, "_vec3Array");
                std::vector<glm::vec3> vec3Array({
                    glm::vec3(1.F, 2.F, 3.F),
                    glm::vec3(4.F, 5.F, 6.F),
                    glm::vec3(7.F, 8.F, 9.F),
                    glm::vec3(10.F, 11.F, 12.F) });
                glShader->setUniform(location, vec3Array);
                glShader->setUniform("_vec3Array", vec3Array);

                location = glGetUniformLocation(program, "_vec4Array");
                std::vector<glm::vec4> vec4Array({
                    glm::vec4(1.F, 2.F, 3.F, 4.F),
                    glm::vec4(5.F, 6.F, 7.F, 8.F),
                    glm::vec4(9.F, 10.F, 11.F, 12.F),
                    glm::vec4(13.F, 14.F, 15.F, 16.F) });
                glShader->setUniform(location, vec4Array);
                glShader->setUniform("_vec4Array", vec4Array);
            }
            catch (const std::exception& e)
            {
                _print(Error::format(e.what()));
            }

            try
            {
                auto shader = Render::Shader::create(
                    "xxx",
                    "xxx");
                auto glShader = Shader::create(shader);
            }
            catch (const std::exception& e)
            {
                _print(Error::format(e.what()));
            }

            try
            {
                auto shader = Render::Shader::create(
                    "#version 100\n"
                    "\n"
                    "void main()\n"
                    "{\n"
                    "    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);\n"
                    "}\n",
                    "xxx");
                auto glShader = Shader::create(shader);
            }
            catch (const std::exception& e)
            {
                _print(Error::format(e.what()));
            }
        }

    } // namespace AVTest
} // namespace djv

