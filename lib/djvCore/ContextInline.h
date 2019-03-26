//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

namespace djv
{
    namespace Core
    {
        inline Context::Context()
        {}

        inline const std::vector<std::string> & Context::getArgs() const
        {
            return _args;
        }
           
        inline const std::string & Context::getName() const
        {
            return _name;
        }

        inline float Context::getFpsAverage() const
        {
            return _fpsAverage;
        }

        inline const std::vector<std::shared_ptr<ISystemBase> > & Context::getSystems() const
        {
            return _systems;
        }

        template<typename T>
        inline std::vector<std::shared_ptr<T> > Context::getSystemsT() const
        {
            std::vector<std::shared_ptr<T> > out;
            for (const auto & i : _systems)
            {
                if (auto system = std::dynamic_pointer_cast<T>(i))
                {
                    out.push_back(system);
                }
            }
            return out;
        }

        template<typename T>
        inline std::shared_ptr<T> Context::getSystemT() const
        {
            for (const auto & i : _systems)
            {
                if (auto system = std::dynamic_pointer_cast<T>(i))
                {
                    return system;
                }
            }
            return nullptr;
        }

    } // namespace Core
} // namespace djv

