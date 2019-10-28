//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvViewApp/SettingsSystem.h>

#include <djvViewApp/FileSettingsWidget.h>
#include <djvViewApp/NUXSettingsWidget.h>
#include <djvViewApp/PlaybackSettingsWidget.h>
#include <djvViewApp/SettingsDialog.h>
#include <djvViewApp/UISettingsWidget.h>
#include <djvViewApp/WindowSettingsWidget.h>

#include <djvUIComponents/IOSettingsWidget.h>
#include <djvUIComponents/LanguageSettingsWidget.h>
#include <djvUIComponents/PPMSettingsWidget.h>
#include <djvUIComponents/PaletteSettingsWidget.h>
#include <djvUIComponents/Render2DSettingsWidget.h>
#include <djvUIComponents/SizeSettingsWidget.h>
#include <djvUIComponents/TimeSettingsWidget.h>
#include <djvUIComponents/TooltipsSettingsWidget.h>
#if defined(JPEG_FOUND)
#include <djvUIComponents/JPEGSettingsWidget.h>
#endif
#if defined(FFmpeg_FOUND)
#include <djvUIComponents/FFmpegSettingsWidget.h>
#endif
#if defined(OPENEXR_FOUND)
#include <djvUIComponents/OpenEXRSettingsWidget.h>
#endif
#if defined(TIFF_FOUND)
#include <djvUIComponents/TIFFSettingsWidget.h>
#endif

#include <djvUI/Action.h>
#include <djvUI/ButtonGroup.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SoloLayout.h>
#include <djvUI/ScrollWidget.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct SettingsSystem::Private
        {
            int currentTab = 0;
            std::shared_ptr<SettingsDialog> settingsDialog;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > actionObservers;
        };

        void SettingsSystem::_init(const std::shared_ptr<Core::Context>& context)
        {
            IViewSystem::_init("djv::ViewApp::SettingsSystem", context);
            DJV_PRIVATE_PTR();
        }

        SettingsSystem::SettingsSystem() :
            _p(new Private)
        {}

        SettingsSystem::~SettingsSystem()
        {
            DJV_PRIVATE_PTR();
            if (p.settingsDialog)
            {
                p.settingsDialog->close();
            }
        }

        std::shared_ptr<SettingsSystem> SettingsSystem::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<SettingsSystem>(new SettingsSystem);
            out->_init(context);
            return out;
        }

        void SettingsSystem::showSettingsDialog()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                if (p.settingsDialog)
                {
                    p.settingsDialog->close();
                }
                p.settingsDialog = SettingsDialog::create(context);
                p.settingsDialog->setCurrentTab(p.currentTab);
                auto weak = std::weak_ptr<SettingsSystem>(std::dynamic_pointer_cast<SettingsSystem>(shared_from_this()));
                p.settingsDialog->setCloseCallback(
                    [weak]
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->currentTab = widget->_p->settingsDialog->getCurrentTab();
                            widget->_p->settingsDialog->close();
                            widget->_p->settingsDialog.reset();
                        }
                    });
                p.settingsDialog->show();
            }
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
                    UI::LanguageSettingsWidget::create(context),
                    UI::SizeSettingsWidget::create(context),
                    UI::PaletteSettingsWidget::create(context),
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
#if defined(OPENEXR_FOUND)
                    UI::OpenEXRSettingsWidget::create(context),
#endif
#if defined(TIFF_FOUND)
                    UI::TIFFSettingsWidget::create(context),
#endif

                    FullscreenMonitorSettingsWidget::create(context),
                    BackgroundImageSettingsWidget::create(context),
                    NUXSettingsWidget::create(context),
                    PlaybackSettingsWidget::create(context),
                    SequenceSettingsWidget::create(context),
                    TimelineSettingsWidget::create(context),
                    UISettingsWidget::create(context)
                };
            }
            return out;
        }
        
    } // namespace ViewApp
} // namespace djv

