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

#include <djvUI/ISettings.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            //! This class provides color space settings.
            class ColorSpace : public ISettings
            {
                DJV_NON_COPYABLE(ColorSpace);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                ColorSpace();

            public:
                virtual ~ColorSpace();

                static std::shared_ptr<ColorSpace> create(const std::shared_ptr<Core::Context>&);

                std::shared_ptr<Core::IValueSubject<std::string> > observeInputColorSpace() const;
                std::shared_ptr<Core::IValueSubject<std::string> > observeDisplay() const;
                std::shared_ptr<Core::IValueSubject<std::string> > observeView() const;
                std::shared_ptr<Core::IValueSubject<std::string> > observeOutputColorSpace() const;
                void setInputColorSpace(const std::string&);
                void setDisplay(const std::string&);
                void setView(const std::string&);

                void load(const picojson::value &) override;
                picojson::value save() override;

            private:
                std::string _getOutputColorSpace() const;

                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UI
} // namespace djv
