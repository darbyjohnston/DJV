// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUI/Render2DSettings.h>

#include <djvRender2D/DataFunc.h>
#include <djvRender2D/EnumFunc.h>
#include <djvRender2D/Render2DSystem.h>

#include <djvImage/ImageDataFunc.h>

#include <djvSystem/Context.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            struct Render2D::Private
            {
                std::shared_ptr<djv::Render2D::Render2DSystem> render2DSystem;
            };

            void Render2D::_init(const std::shared_ptr<System::Context>& context)
            {
                ISettings::_init("djv::UI::Settings::Render2D", context);
                DJV_PRIVATE_PTR();
                p.render2DSystem = context->getSystemT<djv::Render2D::Render2DSystem>();
                _load();
            }

            Render2D::Render2D() :
                _p(new Private)
            {}

            Render2D::~Render2D()
            {}

            std::shared_ptr<Render2D> Render2D::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<Render2D>(new Render2D);
                out->_init(context);
                return out;
            }

            void Render2D::load(const rapidjson::Value& value)
            {
                DJV_PRIVATE_PTR();
                if (value.IsObject())
                {
                    djv::Render2D::ImageFilterOptions imageFilterOptions;
                    bool textLCDRendering = true;
                    read("ImageFilterOptions", value, imageFilterOptions);
                    read("TextLCDRendering", value, textLCDRendering);

                    p.render2DSystem->setImageFilterOptions(imageFilterOptions);
                    p.render2DSystem->setTextLCDRendering(textLCDRendering);
                }
            }

            rapidjson::Value Render2D::save(rapidjson::Document::AllocatorType& allocator)
            {
                DJV_PRIVATE_PTR();
                rapidjson::Value out(rapidjson::kObjectType);
                write("ImageFilterOptions", p.render2DSystem->observeImageFilterOptions()->get(), out, allocator);
                write("TextLCDRendering", p.render2DSystem->observeTextLCDRendering()->get(), out, allocator);
                return out;
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv

