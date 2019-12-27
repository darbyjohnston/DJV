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

#include <djvAV/Enum.h>

#include <djvCore/ISystem.h>
#include <djvCore/Time.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace AV
    {
        namespace Render
        {
            struct ImageFilterOptions;

        } // namespace Render

        //! This class provides an AV system.
        class AVSystem : public Core::ISystem
        {
            DJV_NON_COPYABLE(AVSystem);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            AVSystem();

        public:
            ~AVSystem() override;

            static std::shared_ptr<AVSystem> create(const std::shared_ptr<Core::Context>&);

            std::shared_ptr<Core::IValueSubject<TimeUnits> > observeTimeUnits() const;
            void setTimeUnits(TimeUnits);

            std::shared_ptr<Core::IValueSubject<AlphaBlend> > observeAlphaBlend() const;
            void setAlphaBlend(AlphaBlend);

            std::shared_ptr<Core::IValueSubject<Core::Time::FPS> > observeDefaultSpeed() const;
            void setDefaultSpeed(Core::Time::FPS);

            std::shared_ptr<Core::IValueSubject<Render::ImageFilterOptions> > observeImageFilterOptions() const;
            void setImageFilterOptions(const Render::ImageFilterOptions&);

            std::shared_ptr<Core::IValueSubject<bool> > observeLCDText() const;
            void setLCDText(bool);

            std::string getLabel(Core::Frame::Number, const Core::Time::Speed&) const;

        private:
            DJV_PRIVATE();
        };

    } // namespace AV
} // namespace djv
