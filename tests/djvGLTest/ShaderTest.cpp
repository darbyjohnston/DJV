// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvGLTest/ShaderTest.h>

#include <djvGL/Shader.h>

#include <djvSystem/Context.h>
#include <djvSystem/Path.h>
#include <djvSystem/ResourceSystem.h>

#include <djvImage/Color.h>

#include <djvCore/Error.h>

using namespace djv::Core;
using namespace djv::GL;

namespace djv
{
    namespace GLTest
    {
        ShaderTest::ShaderTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::GLTest::ShaderTest", tempPath, context)
        {}
        
        void ShaderTest::run()
        {
            try
            {
                auto shader = Shader::create(
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
                const GLuint program = shader->getProgram();
                DJV_ASSERT(program);
                shader->bind();
                
                GLint location = glGetUniformLocation(program, "_int");
                shader->setUniform(location, 1);
                shader->setUniform("_int", 1);
                
                location = glGetUniformLocation(program, "_float");
                shader->setUniform(location, 1.F);
                shader->setUniform("_float", 1.F);
                
                location = glGetUniformLocation(program, "_vec2");
                shader->setUniform(location, glm::vec2(1.F, 2.F));
                shader->setUniform("_vec2", glm::vec2(1.F, 2.F));
                
                location = glGetUniformLocation(program, "_vec3");
                shader->setUniform(location, glm::vec3(1.F, 2.F, 3.F));
                shader->setUniform("_vec3", glm::vec3(1.F, 2.F, 3.F));
                
                location = glGetUniformLocation(program, "_vec4");
                shader->setUniform(location, glm::vec4(1.F, 2.F, 3.F, 4.F));
                shader->setUniform("_vec4", glm::vec4(1.F, 2.F, 3.F, 4.F));
                
                location = glGetUniformLocation(program, "_mat3");
                shader->setUniform(location, glm::mat3x3(1.F));
                shader->setUniform("_mat3", glm::mat3(1.F));
                
                location = glGetUniformLocation(program, "_mat4");
                shader->setUniform(location, glm::mat4x4(1.F));
                shader->setUniform("_mat4", glm::mat4x4(1.F));
                
                location = glGetUniformLocation(program, "_color");
                shader->setUniform(location, glm::vec4(.1F, .2F, .3F, 1.F));
                shader->setUniform("_color", glm::vec4(.1F, .2F, .3F, 1.F));
                
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
                    shader->setUniform(location, Image::Color(i));
                    shader->setUniform("_color", Image::Color(i));
                }
                
                for (const auto i : {
                    Image::Type::L_F32,
                    Image::Type::LA_F32,
                    Image::Type::RGB_F32,
                    Image::Type::RGBA_F32 })
                {
                    location = glGetUniformLocation(program, "_color");
                    shader->setUniformF(location, Image::Color(i));
                    shader->setUniformF("_color", Image::Color(i));
                }

                location = glGetUniformLocation(program, "_color4");
                float color4[4] = { .1F, .2F, .3F, 1.F };
                shader->setUniform(location, color4);
                shader->setUniform("_color", color4);

                location = glGetUniformLocation(program, "_intArray");
                std::vector<int> intArray({ 1, 2, 3, 4 });
                shader->setUniform(location, intArray);
                shader->setUniform("_intArray", intArray);

                location = glGetUniformLocation(program, "_floatArray");
                std::vector<float> floatArray({ 1.F, 2.F, 3.F, 4.F });
                shader->setUniform(location, floatArray);
                shader->setUniform("_floatArray", floatArray);

                location = glGetUniformLocation(program, "_vec3Array");
                std::vector<glm::vec3> vec3Array({
                    glm::vec3(1.F, 2.F, 3.F),
                    glm::vec3(4.F, 5.F, 6.F),
                    glm::vec3(7.F, 8.F, 9.F),
                    glm::vec3(10.F, 11.F, 12.F) });
                shader->setUniform(location, vec3Array);
                shader->setUniform("_vec3Array", vec3Array);

                location = glGetUniformLocation(program, "_vec4Array");
                std::vector<glm::vec4> vec4Array({
                    glm::vec4(1.F, 2.F, 3.F, 4.F),
                    glm::vec4(5.F, 6.F, 7.F, 8.F),
                    glm::vec4(9.F, 10.F, 11.F, 12.F),
                    glm::vec4(13.F, 14.F, 15.F, 16.F) });
                shader->setUniform(location, vec4Array);
                shader->setUniform("_vec4Array", vec4Array);
            }
            catch (const std::exception& e)
            {
                _print(Error::format(e.what()));
            }

            if (auto context = getContext().lock())
            {
                auto resourceSystem = context->getSystemT<System::ResourceSystem>();
                const System::File::Path shaderPath = resourceSystem->getPath(System::File::ResourcePath::Shaders);
                const System::File::Path vertexPath = System::File::Path(shaderPath, "djvImageConvertVertex.glsl");
                const System::File::Path fragmentPath = System::File::Path(shaderPath, "djvImageConvertFragment.glsl");
                auto shader = Shader::create(vertexPath, fragmentPath);
                DJV_ASSERT(vertexPath.get() == shader->getVertexName());
                DJV_ASSERT(fragmentPath.get() == shader->getFragmentName());
                _print(shader->getVertexSource());
                _print(shader->getFragmentSource());
            }
            
            try
            {
                auto shader = Shader::create(System::File::Path(), System::File::Path());
                DJV_ASSERT(false);
            }
            catch (const std::exception& e)
            {
                _print(Error::format(e.what()));
            }
                
            try
            {
                auto shader = Shader::create(
                    "xxx",
                    "xxx");
            }
            catch (const std::exception& e)
            {
                _print(Error::format(e.what()));
            }

            try
            {
                auto shader = Shader::create(
                    "#version 100\n"
                    "\n"
                    "void main()\n"
                    "{\n"
                    "    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);\n"
                    "}\n",
                    "xxx");
            }
            catch (const std::exception& e)
            {
                _print(Error::format(e.what()));
            }
        }

    } // namespace GLTest
} // namespace djv

