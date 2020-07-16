// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Annotate.h>

#include <djvUI/ISettings.h>

#include <djvCore/BBox.h>
#include <djvCore/ListObserver.h>
#include <djvCore/ValueObserver.h>

#include <map>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            class Color;

        } // namespace Image
    } // namespace AV

    namespace ViewApp
    {
        //! This class provides the annotation settings.
        class AnnotateSettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(AnnotateSettings);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);

            AnnotateSettings();

        public:
            ~AnnotateSettings() override;

            static std::shared_ptr<AnnotateSettings> create(const std::shared_ptr<Core::Context>&);

            std::shared_ptr<Core::IValueSubject<AnnotateTool> > observeTool() const;
            void setTool(AnnotateTool);

            std::shared_ptr<Core::IValueSubject<AnnotateLineSize> > observeLineSize() const;
            void setLineSize(AnnotateLineSize);

            std::shared_ptr<Core::IListSubject<AV::Image::Color> > observeColors() const;
            void setColors(const std::vector<AV::Image::Color>&);

            std::shared_ptr<Core::IValueSubject<int> > observeCurrentColor() const;
            void setCurrentColor(int);

            const std::map<std::string, Core::BBox2f>& getWidgetGeom() const;
            void setWidgetGeom(const std::map<std::string, Core::BBox2f>&);

            void load(const rapidjson::Value &) override;
            rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

