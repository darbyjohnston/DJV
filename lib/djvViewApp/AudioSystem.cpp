// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/AudioSystem.h>

#include <djvViewApp/FileSystem.h>
#include <djvViewApp/Media.h>

#include <djvUI/Action.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ShortcutData.h>

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

            _addShortcut("shortcut_audio_decrease_volume", GLFW_KEY_8, UI::ShortcutData::getSystemModifier());
            _addShortcut("shortcut_audio_increase_volume", GLFW_KEY_7, UI::ShortcutData::getSystemModifier());
            _addShortcut("shortcut_audio_mute", GLFW_KEY_9, UI::ShortcutData::getSystemModifier());

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["IncreaseVolume"]);
            p.menu->addAction(p.actions["DecreaseVolume"]);
            p.menu->addAction(p.actions["Mute"]);

            _actionsUpdate();
            _textUpdate();
            _shortcutsUpdate();

            auto weak = std::weak_ptr<AudioSystem>(std::dynamic_pointer_cast<AudioSystem>(shared_from_this()));
            p.actions["IncreaseVolume"]->setClickedCallback(
                [weak]
            {
                if (auto system = weak.lock())
                {
                    if (auto media = system->_p->currentMedia)
                    {
                        media->setVolume(system->_p->volume + .1F);
                    }
                }
            });

            p.actions["DecreaseVolume"]->setClickedCallback(
                [weak]
            {
                if (auto system = weak.lock())
                {
                    if (auto media = system->_p->currentMedia)
                    {
                        media->setVolume(system->_p->volume - .1F);
                    }
                }
            });

            p.actions["Mute"]->setCheckedCallback(
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
                "G"
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

        void AudioSystem::_shortcutsUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["IncreaseVolume"]->setShortcuts(_getShortcuts("shortcut_audio_decrease_volume"));
                p.actions["DecreaseVolume"]->setShortcuts(_getShortcuts("shortcut_audio_increase_volume"));
                p.actions["Mute"]->setShortcuts(_getShortcuts("shortcut_audio_mute"));
            }
        }

    } // namespace ViewApp
} // namespace djv

