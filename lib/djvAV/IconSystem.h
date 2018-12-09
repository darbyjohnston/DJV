//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvCore/ISystem.h>

#include <QOpenGLDebugLogger>
#include <QThread>

#include <future>

namespace djv
{
    namespace Core
    {
        class Path;
    
    } // namespace Core

    namespace AV
    {
        namespace Pixel
        {
            class Convert;
            class Info;

        } // namespace Pixel

        class Image;

        class IconSystem : public QThread, public Core::ISystem
        {
            DJV_NON_COPYABLE(IconSystem);

        protected:
            void _init(const std::shared_ptr<Core::Context>& context);
            IconSystem();

        public:
            virtual ~IconSystem();

            static std::shared_ptr<IconSystem> create(const std::shared_ptr<Core::Context>& context);

            std::future<Pixel::Info> getInfo(const Core::Path&);
            std::future<std::shared_ptr<Image> > getImage(const Core::Path&);
            std::future<std::shared_ptr<Image> > getImage(const Core::Path&, const Pixel::Info&);

        protected:
            void run() override;
            void _exit() override;

        private Q_SLOTS:
            void _debugLogMessage(const QOpenGLDebugMessage &);

        private:
            void _handleInfoRequests();
            void _handleImageRequests(const std::shared_ptr<Pixel::Convert> &);

            DJV_PRIVATE();
        };

    } // namespace AV
} // namespace djv
