// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ToolSettings.h>

#include <djvSystem/Context.h>
#include <djvSystem/FileInfo.h>
#include <djvSystem/ResourceSystem.h>

#include <djvMath/Vector.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ToolSettings::Private
        {
            std::shared_ptr<Observer::ValueSubject<int> > currentTool;
        };

        void ToolSettings::_init(const std::shared_ptr<System::Context>& context)
        {
            ISettings::_init("djv::ViewApp::ToolSettings", context);

            DJV_PRIVATE_PTR();
            p.currentTool = Observer::ValueSubject<int>::create(-1);
            _load();
        }

        ToolSettings::ToolSettings() :
            _p(new Private)
        {}

        ToolSettings::~ToolSettings()
        {}

        std::shared_ptr<ToolSettings> ToolSettings::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ToolSettings>(new ToolSettings);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Observer::IValueSubject<int> > ToolSettings::observeCurrentTool() const
        {
            return _p->currentTool;
        }

        void ToolSettings::setCurrentTool(int value)
        {
            _p->currentTool->setIfChanged(value);
        }

        void ToolSettings::load(const rapidjson::Value & value)
        {
            if (value.IsObject())
            {
                DJV_PRIVATE_PTR();
                UI::Settings::read("CurrentTool", value, p.currentTool);
            }
        }

        rapidjson::Value ToolSettings::save(rapidjson::Document::AllocatorType& allocator)
        {
            DJV_PRIVATE_PTR();
            rapidjson::Value out(rapidjson::kObjectType);
            UI::Settings::write("CurrentTool", p.currentTool->get(), out, allocator);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

