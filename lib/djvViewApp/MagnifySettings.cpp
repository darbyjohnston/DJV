// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MagnifySettings.h>

// These need to be included last on OSX.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct MagnifySettings::Private
        {
            size_t magnify = 2;
            glm::vec2 magnifyPos = glm::vec2(0.F, 0.F);
            std::map<std::string, BBox2f> widgetGeom;
        };

        void MagnifySettings::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISettings::_init("djv::ViewApp::MagnifySettings", context);
            _load();
        }

        MagnifySettings::MagnifySettings() :
            _p(new Private)
        {}

        MagnifySettings::~MagnifySettings()
        {}

        std::shared_ptr<MagnifySettings> MagnifySettings::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<MagnifySettings>(new MagnifySettings);
            out->_init(context);
            return out;
        }

        size_t MagnifySettings::getMagnify() const
        {
            return _p->magnify;
        }

        void MagnifySettings::setMagnify(size_t value)
        {
            _p->magnify = value;
        }

        const glm::vec2& MagnifySettings::getMagnifyPos() const
        {
            return _p->magnifyPos;
        }

        void MagnifySettings::setMagnifyPos(const glm::vec2& value)
        {
            _p->magnifyPos = value;
        }

        const std::map<std::string, BBox2f>& MagnifySettings::getWidgetGeom() const
        {
            return _p->widgetGeom;
        }

        void MagnifySettings::setWidgetGeom(const std::map<std::string, BBox2f>& value)
        {
            _p->widgetGeom = value;
        }

        void MagnifySettings::load(const rapidjson::Value & value)
        {
            if (value.IsObject())
            {
                DJV_PRIVATE_PTR();
                UI::Settings::read("Magnify", value, p.magnify);
                UI::Settings::read("MagnifyPos", value, p.magnifyPos);
                UI::Settings::read("WidgetGeom", value, p.widgetGeom);
            }
        }

        rapidjson::Value MagnifySettings::save(rapidjson::Document::AllocatorType& allocator)
        {
            DJV_PRIVATE_PTR();
            rapidjson::Value out(rapidjson::kObjectType);
            UI::Settings::write("Magnify", p.magnify, out, allocator);
            UI::Settings::write("MagnifyPos", p.magnifyPos, out, allocator);
            UI::Settings::write("WidgetGeom", p.widgetGeom, out, allocator);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

