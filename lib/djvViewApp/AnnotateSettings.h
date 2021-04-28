// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Annotate.h>

#include <djvUI/ISettings.h>

#include <djvMath/BBox.h>
#include <djvCore/ListObserver.h>
#include <djvCore/ValueObserver.h>

#include <map>

namespace djv
{
    namespace Image
    {
        class Color;

    } // namespace Image

    namespace ViewApp
    {
        //! Annotation settings.
        class AnnotateSettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(AnnotateSettings);

        protected:
            void _init(const std::shared_ptr<System::Context>&);

            AnnotateSettings();

        public:
            ~AnnotateSettings() override;

            static std::shared_ptr<AnnotateSettings> create(const std::shared_ptr<System::Context>&);

            //! \name Tool
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<AnnotateTool> > observeTool() const;

            void setTool(AnnotateTool);

            ///@}

            //! \name Options
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<AnnotateLineSize> > observeLineSize() const;
            std::shared_ptr<Core::Observer::IValueSubject<Image::Color> > observeColor() const;

            void setLineSize(AnnotateLineSize);
            void setColor(const Image::Color&);

            /// @}

            void load(const rapidjson::Value &) override;
            rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

