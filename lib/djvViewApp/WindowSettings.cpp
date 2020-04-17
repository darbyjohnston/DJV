// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/WindowSettings.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/Vector.h>

// These need to be included last on OSX.
#include <djvCore/PicoJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct WindowSettings::Private
        {
            glm::ivec2 windowSize = glm::ivec2(1280, 720);
            std::shared_ptr<ValueSubject<int> > fullscreenMonitor;
            std::shared_ptr<ValueSubject<bool> > maximize;
            std::shared_ptr<ValueSubject<bool> > autoHide;
            std::shared_ptr<ValueSubject<std::string> > backgroundImage;
            std::shared_ptr<ValueSubject<bool> > backgroundImageScale;
            std::shared_ptr<ValueSubject<bool> > backgroundImageColorize;
        };

        void WindowSettings::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISettings::_init("djv::ViewApp::WindowSettings", context);

            DJV_PRIVATE_PTR();
            p.fullscreenMonitor = ValueSubject<int>::create(0);
            p.maximize = ValueSubject<bool>::create(true);
            p.autoHide = ValueSubject<bool>::create(true);
            auto resourceSystem = context->getSystemT<Core::ResourceSystem>();
            const auto& iconsPath = resourceSystem->getPath(FileSystem::ResourcePath::Icons);
            p.backgroundImage = ValueSubject<std::string>::create(std::string(
                FileSystem::Path(iconsPath, "djv-tshirt-v02.png")));
            p.backgroundImageScale = ValueSubject<bool>::create(false);
            p.backgroundImageColorize = ValueSubject<bool>::create(true);
            _load();
        }

        WindowSettings::WindowSettings() :
            _p(new Private)
        {}

        WindowSettings::~WindowSettings()
        {}

        std::shared_ptr<WindowSettings> WindowSettings::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<WindowSettings>(new WindowSettings);
            out->_init(context);
            return out;
        }

        const glm::ivec2& WindowSettings::getWindowSize() const
        {
            return _p->windowSize;
        }

        void WindowSettings::setWindowSize(const glm::ivec2& value)
        {
            _p->windowSize = value;
        }

        std::shared_ptr<IValueSubject<int> > WindowSettings::observeFullscreenMonitor() const
        {
            return _p->fullscreenMonitor;
        }

        void WindowSettings::setFullscreenMonitor(int value)
        {
            _p->fullscreenMonitor->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<bool> > WindowSettings::observeMaximize() const
        {
            return _p->maximize;
        }

        void WindowSettings::setMaximize(bool value)
        {
            _p->maximize->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<bool> > WindowSettings::observeAutoHide() const
        {
            return _p->autoHide;
        }

        void WindowSettings::setAutoHide(bool value)
        {
            _p->autoHide->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<std::string> > WindowSettings::observeBackgroundImage() const
        {
            return _p->backgroundImage;
        }

        std::shared_ptr<IValueSubject<bool> > WindowSettings::observeBackgroundImageScale() const
        {
            return _p->backgroundImageScale;
        }

        std::shared_ptr<IValueSubject<bool> > WindowSettings::observeBackgroundImageColorize() const
        {
            return _p->backgroundImageColorize;
        }

        void WindowSettings::setBackgroundImageScale(bool value)
        {
            _p->backgroundImageScale->setIfChanged(value);
        }

        void WindowSettings::setBackgroundImageColorize(bool value)
        {
            _p->backgroundImageColorize->setIfChanged(value);
        }

        void WindowSettings::setBackgroundImage(const std::string& value)
        {
            _p->backgroundImage->setIfChanged(value);
        }

        void WindowSettings::load(const picojson::value & value)
        {
            if (value.is<picojson::object>())
            {
                DJV_PRIVATE_PTR();
                const auto & object = value.get<picojson::object>();
                UI::Settings::read("WindowSize", object, p.windowSize);
                UI::Settings::read("FullscreenMonitor", object, p.fullscreenMonitor);
                UI::Settings::read("Maximize", object, p.maximize);
                UI::Settings::read("AutoHide", object, p.autoHide);
                UI::Settings::read("BackgroundImage", object, p.backgroundImage);
                UI::Settings::read("BackgroundImageScale", object, p.backgroundImageScale);
                UI::Settings::read("BackgroundImageColorize", object, p.backgroundImageColorize);
            }
        }

        picojson::value WindowSettings::save()
        {
            DJV_PRIVATE_PTR();
            picojson::value out(picojson::object_type, true);
            auto & object = out.get<picojson::object>();
            UI::Settings::write("WindowSize", p.windowSize, object);
            UI::Settings::write("FullscreenMonitor", p.fullscreenMonitor->get(), object);
            UI::Settings::write("Maximize", p.maximize->get(), object);
            UI::Settings::write("AutoHide", p.autoHide->get(), object);
            UI::Settings::write("BackgroundImage", p.backgroundImage->get(), object);
            UI::Settings::write("BackgroundImageScale", p.backgroundImageScale->get(), object);
            UI::Settings::write("BackgroundImageColorize", p.backgroundImageColorize->get(), object);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

