//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvViewApp/AudioSystem.h>

#include <djvViewApp/FileSystem.h>
#include <djvViewApp/Media.h>

#include <djvUI/Action.h>
#include <djvUI/Menu.h>
#include <djvUI/Shortcut.h>
#include <djvUI/RowLayout.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct AudioSystem::Private
        {
            std::shared_ptr<Media> currentMedia;
            AV::IO::Info info;
            float volume = 1.F;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::Menu> menu;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > actionObservers;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<ValueObserver<AV::IO::Info> > infoObserver;
            std::shared_ptr<ValueObserver<float> > volumeObserver;
            std::shared_ptr<ValueObserver<bool> > muteObserver;
        };

        void AudioSystem::_init(const std::shared_ptr<Core::Context>& context)
        {
            IViewSystem::_init("djv::ViewApp::AudioSystem", context);

            DJV_PRIVATE_PTR();

            p.actions["IncreaseVolume"] = UI::Action::create();
            p.actions["DecreaseVolume"] = UI::Action::create();
            p.actions["Mute"] = UI::Action::create();
            p.actions["Mute"]->setButtonType(UI::ButtonType::Toggle);

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["IncreaseVolume"]);
            p.actions["IncreaseVolume"]->setShortcut(GLFW_KEY_8, UI::Shortcut::getSystemModifier());
            p.menu->addAction(p.actions["DecreaseVolume"]);
            p.actions["DecreaseVolume"]->setShortcut(GLFW_KEY_7, UI::Shortcut::getSystemModifier());
            p.menu->addAction(p.actions["Mute"]);
            p.actions["Mute"]->setShortcut(GLFW_KEY_9, UI::Shortcut::getSystemModifier());

            _textUpdate();

            auto weak = std::weak_ptr<AudioSystem>(std::dynamic_pointer_cast<AudioSystem>(shared_from_this()));
            p.actionObservers["IncreaseVolume"] = ValueObserver<bool>::create(
                p.actions["IncreaseVolume"]->observeClicked(),
                [weak](bool value)
            {
                if (value)
                {
                    if (auto system = weak.lock())
                    {
                        if (auto media = system->_p->currentMedia)
                        {
                            media->setVolume(system->_p->volume + .1F);
                        }
                    }
                }
            });

            p.actionObservers["DecreaseVolume"] = ValueObserver<bool>::create(
                p.actions["DecreaseVolume"]->observeClicked(),
                [weak](bool value)
            {
                if (value)
                {
                    if (auto system = weak.lock())
                    {
                        if (auto media = system->_p->currentMedia)
                        {
                            media->setVolume(system->_p->volume - .1F);
                        }
                    }
                }
            });

            p.actionObservers["Mute"] = ValueObserver<bool>::create(
                p.actions["Mute"]->observeChecked(),
                [weak](bool value)
            {
                if (auto system = weak.lock())
                {
                    if (auto media = system->_p->currentMedia)
                    {
                        media->setMute(value);
                    }
                }
            });

            if (auto fileSystem = context->getSystemT<FileSystem>())
            {
                p.currentMediaObserver = ValueObserver<std::shared_ptr<Media> >::create(
                    fileSystem->observeCurrentMedia(),
                    [weak](const std::shared_ptr<Media> & value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->currentMedia = value;
                        system->_p->actions["IncreaseVolume"]->setEnabled(value ? (system->_p->volume < 1.F) : false);
                        system->_p->actions["DecreaseVolume"]->setEnabled(value ? (system->_p->volume > 0.F) : false);
                        system->_p->actions["Mute"]->setEnabled(value ? true : false);
                        if (value)
                        {
                            system->_p->infoObserver = ValueObserver<AV::IO::Info>::create(
                                value->observeInfo(),
                                [weak](const AV::IO::Info& value)
                                {
                                    if (auto system = weak.lock())
                                    {
                                        system->_p->info = value;
                                        system->_actionsUpdate();
                                    }
                                });
                            system->_p->volumeObserver = ValueObserver<float>::create(
                                value->observeVolume(),
                                [weak](float value)
                            {
                                if (auto system = weak.lock())
                                {
                                    system->_p->volume = value;
                                    system->_actionsUpdate();
                                }
                            });
                            system->_p->muteObserver = ValueObserver<bool>::create(
                                value->observeMute(),
                                [weak](bool value)
                            {
                                if (auto system = weak.lock())
                                {
                                    system->_p->actions["Mute"]->setChecked(value);
                                }
                            });
                        }
                        else
                        {
                            system->_p->infoObserver.reset();
                            system->_p->volumeObserver.reset();
                            system->_p->muteObserver.reset();
                            system->_actionsUpdate();
                        }
                    }
                });
            }
        }

        AudioSystem::AudioSystem() :
            _p(new Private)
        {}

        AudioSystem::~AudioSystem()
        {}

        std::shared_ptr<AudioSystem> AudioSystem::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<AudioSystem>(new AudioSystem);
            out->_init(context);
            return out;
        }

        std::map<std::string, std::shared_ptr<UI::Action> > AudioSystem::getActions() const
        {
            return _p->actions;
        }

        MenuData AudioSystem::getMenu() const
        {
            return
            {
                _p->menu,
                "F"
            };
        }

        void AudioSystem::_actionsUpdate()
        {
            DJV_PRIVATE_PTR();
            const bool hasAudio = p.info.audio.size();
            p.actions["IncreaseVolume"]->setEnabled(hasAudio && p.volume < 1.F);
            p.actions["DecreaseVolume"]->setEnabled(hasAudio && p.volume > 0.F);
            p.actions["Mute"]->setEnabled(hasAudio);
        }

        void AudioSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["IncreaseVolume"]->setText(_getText(DJV_TEXT("menu_audio_increase_volume")));
                p.actions["IncreaseVolume"]->setTooltip(_getText(DJV_TEXT("menu_audio_increase_volume_tooltip")));
                p.actions["DecreaseVolume"]->setText(_getText(DJV_TEXT("menu_audio_decrease_volume")));
                p.actions["DecreaseVolume"]->setTooltip(_getText(DJV_TEXT("menu_audio_decrease_volume_tooltip")));
                p.actions["Mute"]->setText(_getText(DJV_TEXT("menu_audio_mute")));
                p.actions["Mute"]->setTooltip(_getText(DJV_TEXT("menu_audio_mute_tooltip")));

                p.menu->setText(_getText(DJV_TEXT("menu_audio")));
            }
        }

    } // namespace ViewApp
} // namespace djv

