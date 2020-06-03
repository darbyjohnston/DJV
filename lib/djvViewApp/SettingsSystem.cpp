// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/SettingsSystem.h>

#include <djvViewApp/FileSettingsWidget.h>
#include <djvViewApp/InputSettingsWidget.h>
#include <djvViewApp/NUXSettingsWidget.h>
#include <djvViewApp/PlaybackSettingsWidget.h>
#include <djvViewApp/WindowSettingsWidget.h>

#include <djvUIComponents/GLFWSettingsWidget.h>
#include <djvUIComponents/IOSettingsWidget.h>
#include <djvUIComponents/LanguageSettingsWidget.h>
#include <djvUIComponents/PPMSettingsWidget.h>
#include <djvUIComponents/Render2DSettingsWidget.h>
#include <djvUIComponents/StyleSettingsWidget.h>
#include <djvUIComponents/TimeSettingsWidget.h>
#include <djvUIComponents/TooltipsSettingsWidget.h>
#if defined(JPEG_FOUND)
#include <djvUIComponents/JPEGSettingsWidget.h>
#endif
#if defined(FFmpeg_FOUND)
#include <djvUIComponents/FFmpegSettingsWidget.h>
#endif
#if defined(OpenEXR_FOUND)
#include <djvUIComponents/OpenEXRSettingsWidget.h>
#endif
#if defined(TIFF_FOUND)
#include <djvUIComponents/TIFFSettingsWidget.h>
#endif

#include <djvUI/Action.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct SettingsSystem::Private
        {
            int currentTab = 0;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > actionObservers;
        };

        void SettingsSystem::_init(const std::shared_ptr<Core::Context>& context)
        {
            IViewSystem::_init("djv::ViewApp::SettingsSystem", context);
        }

        SettingsSystem::SettingsSystem() :
            _p(new Private)
        {}

        SettingsSystem::~SettingsSystem()
        {}

        std::shared_ptr<SettingsSystem> SettingsSystem::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<SettingsSystem>(new SettingsSystem);
            out->_init(context);
            return out;
        }
        
        std::map<std::string, std::shared_ptr<UI::Action> > SettingsSystem::getActions() const
        {
            return _p->actions;
        }

        std::vector<std::shared_ptr<UI::ISettingsWidget> > SettingsSystem::createSettingsWidgets() const
        {
            std::vector<std::shared_ptr<UI::ISettingsWidget> > out;
            if (auto context = getContext().lock())
            {
                out =
                {
                    // UI settings widgets.
                    UI::GLFWSettingsWidget::create(context),
                    UI::LanguageSettingsWidget::create(context),
                    UI::StyleSettingsWidget::create(context),
                    UI::Render2DImageSettingsWidget::create(context),
                    UI::Render2DTextSettingsWidget::create(context),
                    UI::TimeSettingsWidget::create(context),
                    UI::TooltipsSettingsWidget::create(context),
                    UI::IOThreadsSettingsWidget::create(context),
                    UI::PPMSettingsWidget::create(context),
                    UI::PPMSettingsWidget::create(context),
#if defined(JPEG_FOUND)
                    UI::JPEGSettingsWidget::create(context),
#endif
#if defined(FFmpeg_FOUND)
                    UI::FFmpegSettingsWidget::create(context),
#endif
#if defined(OpenEXR_FOUND)
                    UI::OpenEXRSettingsWidget::create(context),
#endif
#if defined(TIFF_FOUND)
                    UI::TIFFSettingsWidget::create(context),
#endif

                    // ViewApp settings widgets.
                    WindowGeometrySettingsWidget::create(context),
                    FullscreenMonitorSettingsWidget::create(context),
                    AutoHideSettingsWidget::create(context),
                    BackgroundImageSettingsWidget::create(context),
                    InputSettingsWidget::create(context),
                    NUXSettingsWidget::create(context),
                    PlaybackSettingsWidget::create(context),
                    SequenceSettingsWidget::create(context),
                    TimelineSettingsWidget::create(context)
                };
            }
            return out;
        }
        
    } // namespace ViewApp
} // namespace djv

