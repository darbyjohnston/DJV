// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/SettingsSystem.h>

#include <djvViewApp/FileSettingsWidget.h>
#include <djvViewApp/KeyboardSettingsWidget.h>
#include <djvViewApp/MemorySettingsWidget.h>
#include <djvViewApp/NUXSettingsWidget.h>
#include <djvViewApp/PlaybackSettingsWidget.h>
#include <djvViewApp/SettingsWidget.h>
#include <djvViewApp/ToolTitleBar.h>
#include <djvViewApp/WindowSettingsWidget.h>

#include <djvUIComponents/FileBrowserSettingsWidget.h>
#include <djvUIComponents/GLFWSettingsWidget.h>
#include <djvUIComponents/IOSettingsWidget.h>
#include <djvUIComponents/LanguageSettingsWidget.h>
#include <djvUIComponents/MouseSettingsWidget.h>
#include <djvUIComponents/PPMSettingsWidget.h>
#include <djvUIComponents/Render2DSettingsWidget.h>
#include <djvUIComponents/StyleSettingsWidget.h>
#include <djvUIComponents/TimeSettingsWidget.h>
#include <djvUIComponents/UISettingsWidget.h>
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
#include <djvUI/ShortcutData.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct SettingsSystem::Private
        {
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::map<std::string, std::shared_ptr<Observer::Value<bool> > > actionObservers;
        };

        void SettingsSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            IViewAppSystem::_init("djv::ViewApp::SettingsSystem", context);
            DJV_PRIVATE_PTR();

            p.actions["Settings"] = UI::Action::create();
            p.actions["Settings"]->setIcon("djvIconSettings");
            p.actions["Settings"]->setButtonType(UI::ButtonType::Toggle);

            _addShortcut("shortcut_settings", 0);

            _shortcutsUpdate();
        }

        SettingsSystem::SettingsSystem() :
            _p(new Private)
        {}

        SettingsSystem::~SettingsSystem()
        {}

        std::shared_ptr<SettingsSystem> SettingsSystem::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = context->getSystemT<SettingsSystem>();
            if (!out)
            {
                out = std::shared_ptr<SettingsSystem>(new SettingsSystem);
                out->_init(context);
            }
            return out;
        }
        
        std::map<std::string, std::shared_ptr<UI::Action> > SettingsSystem::getActions() const
        {
            return _p->actions;
        }
        
        std::vector<ActionData> SettingsSystem::getToolActionData() const
        {
            return
            {
                { _p->actions["Settings"], "Z4" }
            };
        }

        ToolWidgetData SettingsSystem::createToolWidget(const std::shared_ptr<UI::Action>& value)
        {
            DJV_PRIVATE_PTR();
            ToolWidgetData out;
            auto contextWeak = getContext();
            if (auto context = contextWeak.lock())
            {
                if (value == _p->actions["Settings"])
                {
                    auto titleBar = ToolTitleBar::create(DJV_TEXT("settings_title"), context);

                    auto widget = SettingsWidget::create(context);

                    out.titleBar = titleBar;
                    out.widget = widget;
                }
            }
            return out;
        }

        std::vector<std::shared_ptr<UIComponents::Settings::IWidget> > SettingsSystem::createSettingsWidgets() const
        {
            std::vector<std::shared_ptr<UIComponents::Settings::IWidget> > out;
            if (auto context = getContext().lock())
            {
                out =
                {
                    UIComponents::Settings::MouseWidget::create(context),
                    UIComponents::Settings::FileBrowserWidget::create(context),
                    UIComponents::Settings::GLFWWidget::create(context),
                    UIComponents::Settings::LanguageWidget::create(context),
                    UIComponents::Settings::StyleWidget::create(context),
                    UIComponents::Settings::Render2DImageWidget::create(context),
                    UIComponents::Settings::Render2DTextWidget::create(context),
                    UIComponents::Settings::TimeWidget::create(context),
                    UIComponents::Settings::TooltipsWidget::create(context),
                    UIComponents::Settings::IOThreadsWidget::create(context),
                    UIComponents::Settings::PPMWidget::create(context),
#if defined(JPEG_FOUND)
                    UIComponents::Settings::JPEGWidget::create(context),
#endif
#if defined(FFmpeg_FOUND)
                    UIComponents::Settings::FFmpegWidget::create(context),
#endif
#if defined(OpenEXR_FOUND)
                    UIComponents::Settings::OpenEXRWidget::create(context),
#endif
#if defined(TIFF_FOUND)
                    UIComponents::Settings::TIFFWidget::create(context),
#endif

                    SequenceSettingsWidget::create(context),
                    RecentFilesSettingsWidget::create(context),

                    WindowGeometrySettingsWidget::create(context),
                    FullscreenMonitorSettingsWidget::create(context),
                    BackgroundImageSettingsWidget::create(context),

                    MemorySettingsWidget::create(context),

                    KeyboardShortcutSettingsWidget::create(context),

                    PlaybackSettingsWidget::create(context),
                    TimelineSettingsWidget::create(context),

                    NUXSettingsWidget::create(context)
                };
            }
            return out;
        }

        void SettingsSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Settings"]->setText(_getText(DJV_TEXT("menu_tools_settings")));
                p.actions["Settings"]->setTooltip(_getText(DJV_TEXT("menu_tools_settings_tooltip")));
            }
        }

        void SettingsSystem::_shortcutsUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Settings"]->setShortcuts(_getShortcuts("shortcut_settings"));
            }
        }
        
    } // namespace ViewApp
} // namespace djv

