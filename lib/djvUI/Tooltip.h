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

#pragma once

#include <djvUI/UI.h>

#include <glm/vec2.hpp>

#include <memory>

namespace djv
{
    namespace Core
    {
        class Context;
    
    } // namespace Core

    namespace UI
    {
        class Widget;
        class Window;

        //! This class provides tooltips.
        class Tooltip : public std::enable_shared_from_this<Tooltip>
        {
            DJV_NON_COPYABLE(Tooltip);

        protected:
            void _init(
                const std::shared_ptr<Window> &,
                const glm::vec2 & pos,
                const std::shared_ptr<Widget> &,
                const std::shared_ptr<Core::Context>&);
            Tooltip();

        public:
            virtual ~Tooltip();

            static std::shared_ptr<Tooltip> create(
                const std::shared_ptr<Window> &,
                const glm::vec2 & pos,
                const std::shared_ptr<Widget> &,
                const std::shared_ptr<Core::Context>&);

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

