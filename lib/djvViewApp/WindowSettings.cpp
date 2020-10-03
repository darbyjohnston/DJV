// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/WindowSettings.h>

#include <djvSystem/Context.h>
#include <djvSystem/FileInfo.h>
#include <djvSystem/ResourceSystem.h>

#include <djvMath/VectorFunc.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        namespace
        {
            //! \todo Should this be configurable?
            const glm::ivec2 windowSizeDefault = glm::ivec2(1280, 720);
        
        } // namespace

        struct WindowSettings::Private
        {
            std::shared_ptr<Observer::ValueSubject<bool> > restorePos;
            std::shared_ptr<Observer::ValueSubject<bool> > restoreSize;
            glm::ivec2 windowPos = glm::ivec2(0, 0);
            glm::ivec2 windowSize = windowSizeDefault;
            std::shared_ptr<Observer::ValueSubject<bool> > fullScreen;
            std::shared_ptr<Observer::ValueSubject<int> > fullScreenMonitor;
            std::shared_ptr<Observer::ValueSubject<bool> > floatOnTop;
            std::shared_ptr<Observer::ValueSubject<bool> > maximize;
            std::shared_ptr<Observer::ValueSubject<std::string> > backgroundImage;
            std::shared_ptr<Observer::ValueSubject<bool> > backgroundImageScale;
            std::shared_ptr<Observer::ValueSubject<bool> > backgroundImageColorize;
        };

        void WindowSettings::_init(const std::shared_ptr<System::Context>& context)
        {
            ISettings::_init("djv::ViewApp::WindowSettings", context);

            DJV_PRIVATE_PTR();
            p.restorePos = Observer::ValueSubject<bool>::create(false);
            p.restoreSize = Observer::ValueSubject<bool>::create(true);
            p.fullScreen = Observer::ValueSubject<bool>::create(false);
            p.fullScreenMonitor = Observer::ValueSubject<int>::create(0);
            p.floatOnTop = Observer::ValueSubject<bool>::create(false);
            p.maximize = Observer::ValueSubject<bool>::create(true);
            auto resourceSystem = context->getSystemT<System::ResourceSystem>();
            const auto& iconsPath = resourceSystem->getPath(System::File::ResourcePath::Icons);
            p.backgroundImage = Observer::ValueSubject<std::string>::create(std::string(
                System::File::Path(iconsPath, "djv-tshirt-v02.png")));
            p.backgroundImageScale = Observer::ValueSubject<bool>::create(false);
            p.backgroundImageColorize = Observer::ValueSubject<bool>::create(true);
            _load();
        }

        WindowSettings::WindowSettings() :
            _p(new Private)
        {}

        WindowSettings::~WindowSettings()
        {}

        std::shared_ptr<WindowSettings> WindowSettings::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<WindowSettings>(new WindowSettings);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Core::Observer::IValueSubject<bool> > WindowSettings::observeRestorePos() const
        {
            return _p->restorePos;
        }

        std::shared_ptr<Core::Observer::IValueSubject<bool> > WindowSettings::observeRestoreSize() const
        {
            return _p->restoreSize;
        }

        const glm::ivec2& WindowSettings::getWindowPos() const
        {
            return _p->windowPos;
        }

        const glm::ivec2& WindowSettings::getWindowSize() const
        {
            return _p->windowSize;
        }

        const glm::ivec2& WindowSettings::getWindowSizeDefault() const
        {
            return windowSizeDefault;
        }

        void WindowSettings::setRestorePos(bool value)
        {
            _p->restorePos->setIfChanged(value);
        }

        void WindowSettings::setRestoreSize(bool value)
        {
            _p->restoreSize->setIfChanged(value);
        }

        void WindowSettings::setWindowPos(const glm::ivec2& value)
        {
            _p->windowPos = value;
        }

        void WindowSettings::setWindowSize(const glm::ivec2& value)
        {
            _p->windowSize = value;
        }

        std::shared_ptr<Observer::IValueSubject<bool> > WindowSettings::observeFullScreen() const
        {
            return _p->fullScreen;
        }

        std::shared_ptr<Observer::IValueSubject<int> > WindowSettings::observeFullScreenMonitor() const
        {
            return _p->fullScreenMonitor;
        }

        void WindowSettings::setFullScreen(bool value)
        {
            _p->fullScreen->setIfChanged(value);
        }

        void WindowSettings::setFullScreenMonitor(int value)
        {
            _p->fullScreenMonitor->setIfChanged(value);
        }

        std::shared_ptr<Observer::IValueSubject<bool> > WindowSettings::observeFloatOnTop() const
        {
            return _p->floatOnTop;
        }

        std::shared_ptr<Observer::IValueSubject<bool> > WindowSettings::observeMaximize() const
        {
            return _p->maximize;
        }

        void WindowSettings::setFloatOnTop(bool value)
        {
            _p->floatOnTop->setIfChanged(value);
        }

        void WindowSettings::setMaximize(bool value)
        {
            _p->maximize->setIfChanged(value);
        }

        std::shared_ptr<Observer::IValueSubject<std::string> > WindowSettings::observeBackgroundImage() const
        {
            return _p->backgroundImage;
        }

        std::shared_ptr<Observer::IValueSubject<bool> > WindowSettings::observeBackgroundImageScale() const
        {
            return _p->backgroundImageScale;
        }

        std::shared_ptr<Observer::IValueSubject<bool> > WindowSettings::observeBackgroundImageColorize() const
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

        void WindowSettings::load(const rapidjson::Value & value)
        {
            if (value.IsObject())
            {
                DJV_PRIVATE_PTR();
                UI::Settings::read("RestorePos", value, p.restorePos);
                UI::Settings::read("RestoreSize", value, p.restoreSize);
                UI::Settings::read("WindowPos", value, p.windowPos);
                UI::Settings::read("WindowSize", value, p.windowSize);
                UI::Settings::read("FullScreen", value, p.fullScreen);
                UI::Settings::read("FullScreenMonitor", value, p.fullScreenMonitor);
                UI::Settings::read("FloatOnTop", value, p.floatOnTop);
                UI::Settings::read("Maximize", value, p.maximize);
                UI::Settings::read("BackgroundImage", value, p.backgroundImage);
                UI::Settings::read("BackgroundImageScale", value, p.backgroundImageScale);
                UI::Settings::read("BackgroundImageColorize", value, p.backgroundImageColorize);
            }
        }

        rapidjson::Value WindowSettings::save(rapidjson::Document::AllocatorType& allocator)
        {
            DJV_PRIVATE_PTR();
            rapidjson::Value out(rapidjson::kObjectType);
            UI::Settings::write("RestorePos", p.restorePos->get(), out, allocator);
            UI::Settings::write("RestoreSize", p.restoreSize->get(), out, allocator);
            UI::Settings::write("WindowPos", p.windowPos, out, allocator);
            UI::Settings::write("WindowSize", p.windowSize, out, allocator);
            UI::Settings::write("FullScreen", p.fullScreen->get(), out, allocator);
            UI::Settings::write("FullScreenMonitor", p.fullScreenMonitor->get(), out, allocator);
            UI::Settings::write("FloatOnTop", p.floatOnTop->get(), out, allocator);
            UI::Settings::write("Maximize", p.maximize->get(), out, allocator);
            UI::Settings::write("BackgroundImage", p.backgroundImage->get(), out, allocator);
            UI::Settings::write("BackgroundImageScale", p.backgroundImageScale->get(), out, allocator);
            UI::Settings::write("BackgroundImageColorize", p.backgroundImageColorize->get(), out, allocator);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

