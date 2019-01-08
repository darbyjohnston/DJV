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

#include <djvUI/ISettings.h>
#include <djvUI/Style.h>

#include <djvCore/MapObserver.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            //! This class provides the style settings.
            class Style : public ISettings
            {
                DJV_NON_COPYABLE(Style);

            protected:
                void _init(Core::Context * context);

                Style();

            public:
                virtual ~Style();

                static std::shared_ptr<Style> create(Core::Context *);

                std::shared_ptr<Core::IMapSubject<std::string, UI::Style::Palette> > observePalettes() const;
                std::shared_ptr<Core::IValueSubject<UI::Style::Palette> > observeCurrentPalette() const;
                std::shared_ptr<Core::IValueSubject<std::string> > observeCurrentPaletteName() const;
                void setCurrentPalette(const std::string&);

                std::shared_ptr<Core::IValueSubject<int> > observeDPI() const;
                std::shared_ptr<Core::IMapSubject<std::string, UI::Style::Metrics> > observeMetrics() const;
                std::shared_ptr<Core::IValueSubject<UI::Style::Metrics> > observeCurrentMetrics() const;
                std::shared_ptr<Core::IValueSubject<std::string> > observeCurrentMetricsName() const;
                void setDPI(int);
                void setCurrentMetrics(const std::string&);

                void load(const picojson::value&) override;
                picojson::value save() override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UI
} // namespace djv
