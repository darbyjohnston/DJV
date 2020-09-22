// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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
                void _init(const std::shared_ptr<System::Context>&);

                Style();

            public:
                ~Style() override;

                static std::shared_ptr<Style> create(const std::shared_ptr<System::Context>&);

                std::shared_ptr<Core::IMapSubject<std::string, UI::Style::Palette> > observePalettes() const;
                std::shared_ptr<Core::IValueSubject<UI::Style::Palette> > observeCurrentPalette() const;
                std::shared_ptr<Core::IValueSubject<std::string> > observeCurrentPaletteName() const;
                std::shared_ptr<Core::IValueSubject<float> > observeBrightness() const;
                std::shared_ptr<Core::IValueSubject<float> > observeContrast() const;

                void setCurrentPalette(const std::string&);
                void setBrightness(float);
                void setContrast(float);

                std::shared_ptr<Core::IMapSubject<std::string, UI::Style::Metrics> > observeMetrics() const;
                std::shared_ptr<Core::IValueSubject<UI::Style::Metrics> > observeCurrentMetrics() const;
                std::shared_ptr<Core::IValueSubject<std::string> > observeCurrentMetricsName() const;
                
                void setCurrentMetrics(const std::string&);

                std::shared_ptr<Core::IValueSubject<std::string> > observeCurrentFont() const;

                void load(const rapidjson::Value&) override;
                rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

            private:
                void _currentFontUpdate();

                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UI
} // namespace djv
