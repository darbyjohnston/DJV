// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/PlaybackSystem.h>

#include <djvViewApp/Application.h>
#include <djvViewApp/FileSystem.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/PlaybackSettings.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Menu.h>
#include <djvUI/MouseSettings.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ShortcutDataFunc.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/Style.h>
#include <djvUI/UISystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/TextSystem.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct PlaybackSystem::Private
        {
            std::shared_ptr<PlaybackSettings> settings;
            std::shared_ptr<Media> currentMedia;
            Math::IntRational speed;
            Math::Frame::Sequence sequence;
            bool playEveryFrame = false;
            Playback playback = Playback::Forward;
            AV::IO::InOutPoints inOutPoints;
            glm::vec2 hoverPos = glm::vec2(0.F, 0.F);
            glm::vec2 dragStart = glm::vec2(0.F, 0.F);
            Math::Frame::Index dragStartFrame = Math::Frame::invalidIndex;
            Playback dragStartPlayback = Playback::Stop;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ActionGroup> playbackActionGroup;
            std::shared_ptr<UI::ActionGroup> playbackModeActionGroup;
            std::shared_ptr<UI::Menu> menu;
            std::shared_ptr<Observer::Value<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<Observer::Value<Math::IntRational> > speedObserver;
            std::shared_ptr<Observer::Value<bool> > playEveryFrameObserver;
            std::shared_ptr<Observer::Value<Math::Frame::Sequence> > sequenceObserver;
            std::shared_ptr<Observer::Value<Playback> > playbackObserver;
            std::shared_ptr<Observer::Value<PlaybackMode> > playbackModeObserver;
            std::shared_ptr<Observer::Value<AV::IO::InOutPoints> > inOutPointsObserver;
            std::shared_ptr<Observer::Value<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
            std::shared_ptr<Observer::Value<PointerData> > hoverObserver;
            std::shared_ptr<Observer::Value<PointerData> > dragObserver;
            std::shared_ptr<Observer::Value<ScrollData> > scrollObserver;

            void drag(const PointerData&, const std::weak_ptr<System::Context>&);
            void scroll(const ScrollData&, const std::weak_ptr<System::Context>&);
        };

        void PlaybackSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            IViewAppSystem::_init("djv::ViewApp::PlaybackSystem", context);

            DJV_PRIVATE_PTR();
            p.settings = PlaybackSettings::create(context);

            p.actions["Forward"] = UI::Action::create();
            p.actions["Forward"]->setIcon("djvIconPlaybackForward");
            p.actions["Forward"]->setCheckedIcon("djvIconPlaybackStop");
            p.actions["Reverse"] = UI::Action::create();
            p.actions["Reverse"]->setIcon("djvIconPlaybackReverse");
            p.actions["Reverse"]->setCheckedIcon("djvIconPlaybackStop");
            p.playbackActionGroup = UI::ActionGroup::create(UI::ButtonType::Exclusive);
            p.playbackActionGroup->setActions({
                p.actions["Forward"],
                p.actions["Reverse"] });
            p.actions["Playback"] = UI::Action::create();

            p.actions["PlayOnce"] = UI::Action::create();
            p.actions["PlayOnce"]->setEnabled(false);
            p.actions["PlayLoop"] = UI::Action::create();
            p.actions["PlayLoop"]->setEnabled(false);
            p.actions["PlayPingPong"] = UI::Action::create();
            p.actions["PlayPingPong"]->setEnabled(false);
            p.playbackModeActionGroup = UI::ActionGroup::create(UI::ButtonType::Radio);
            p.playbackModeActionGroup->setActions({
                p.actions["PlayOnce"],
                p.actions["PlayLoop"],
                p.actions["PlayPingPong"] });

            p.actions["PlayEveryFrame"] = UI::Action::create();
            p.actions["PlayEveryFrame"]->setButtonType(UI::ButtonType::Toggle);

            p.actions["InPoint"] = UI::Action::create();
            p.actions["InPoint"]->setIcon("djvIconFrameStart");
            p.actions["OutPoint"] = UI::Action::create();
            p.actions["OutPoint"]->setIcon("djvIconFrameEnd");
            p.actions["StartFrame"] = UI::Action::create();
            p.actions["EndFrame"] = UI::Action::create();
            p.actions["NextFrame"] = UI::Action::create();
            p.actions["NextFrame"]->setIcon("djvIconFrameNext");
            p.actions["NextFrame"]->setAutoRepeat(true);
            p.actions["NextFrame10"] = UI::Action::create();
            p.actions["NextFrame10"]->setAutoRepeat(true);
            p.actions["NextFrame100"] = UI::Action::create();
            p.actions["NextFrame100"]->setAutoRepeat(true);
            p.actions["PrevFrame"] = UI::Action::create();
            p.actions["PrevFrame"]->setIcon("djvIconFramePrev");
            p.actions["PrevFrame"]->setAutoRepeat(true);
            p.actions["PrevFrame10"] = UI::Action::create();
            p.actions["PrevFrame10"]->setAutoRepeat(true);
            p.actions["PrevFrame100"] = UI::Action::create();
            p.actions["PrevFrame100"]->setAutoRepeat(true);

            p.actions["InOutPoints"] = UI::Action::create();
            p.actions["InOutPoints"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["SetInPoint"] = UI::Action::create();
            p.actions["SetInPoint"]->setIcon("djvIconFrameSetStart");
            p.actions["SetOutPoint"] = UI::Action::create();
            p.actions["SetOutPoint"]->setIcon("djvIconFrameSetEnd");
            p.actions["ResetInPoint"] = UI::Action::create();
            p.actions["ResetInPoint"]->setIcon("djvIconClear");
            p.actions["ResetOutPoint"] = UI::Action::create();
            p.actions["ResetOutPoint"]->setIcon("djvIconClear");

            _addShortcut(DJV_TEXT("shortcut_playback_forward"), {
                UI::ShortcutData(GLFW_KEY_UP),
                UI::ShortcutData(GLFW_KEY_L) });
            _addShortcut(DJV_TEXT("shortcut_playback_reverse"), {
                UI::ShortcutData(GLFW_KEY_DOWN),
                UI::ShortcutData(GLFW_KEY_J) });
            _addShortcut(DJV_TEXT("shortcut_playback_playback"), {
                UI::ShortcutData(GLFW_KEY_SPACE),
                UI::ShortcutData(GLFW_KEY_K) });
            _addShortcut(DJV_TEXT("shortcut_playback_in_point"), GLFW_KEY_HOME);
            _addShortcut(DJV_TEXT("shortcut_playback_out_point"), GLFW_KEY_END);
            _addShortcut(DJV_TEXT("shortcut_playback_start_frame"), GLFW_KEY_HOME, GLFW_MOD_SHIFT);
            _addShortcut(DJV_TEXT("shortcut_playback_end_frame"), GLFW_KEY_END, GLFW_MOD_SHIFT);
            _addShortcut(DJV_TEXT("shortcut_playback_next_frame"), {
                UI::ShortcutData(GLFW_KEY_RIGHT),
                UI::ShortcutData(GLFW_KEY_RIGHT_BRACKET) });
            _addShortcut(DJV_TEXT("shortcut_playback_next_frame_10"), {
                UI::ShortcutData(GLFW_KEY_RIGHT, GLFW_MOD_SHIFT),
                UI::ShortcutData(GLFW_KEY_RIGHT_BRACKET, GLFW_MOD_SHIFT) });
            _addShortcut(DJV_TEXT("shortcut_playback_next_frame_100"), {
                UI::ShortcutData(GLFW_KEY_RIGHT, UI::getSystemModifier()),
                UI::ShortcutData(GLFW_KEY_RIGHT_BRACKET, UI::getSystemModifier()) });
            _addShortcut(DJV_TEXT("shortcut_playback_prev_frame"), {
                UI::ShortcutData(GLFW_KEY_LEFT),
                UI::ShortcutData(GLFW_KEY_LEFT_BRACKET) });
            _addShortcut(DJV_TEXT("shortcut_playback_prev_frame_10"), {
                UI::ShortcutData(GLFW_KEY_LEFT, GLFW_MOD_SHIFT),
                UI::ShortcutData(GLFW_KEY_LEFT_BRACKET, GLFW_MOD_SHIFT) });
            _addShortcut(DJV_TEXT("shortcut_playback_prev_frame_100"), {
                UI::ShortcutData(GLFW_KEY_LEFT, UI::getSystemModifier()),
                UI::ShortcutData(GLFW_KEY_LEFT_BRACKET, UI::getSystemModifier()) });
            _addShortcut(DJV_TEXT("shortcut_playback_in_out_points"), GLFW_KEY_P);
            _addShortcut(DJV_TEXT("shortcut_playback_set_in_point"), GLFW_KEY_I);
            _addShortcut(DJV_TEXT("shortcut_playback_set_out_point"), GLFW_KEY_O);
            _addShortcut(DJV_TEXT("shortcut_playback_reset_in_point"), GLFW_KEY_I, GLFW_MOD_SHIFT);
            _addShortcut(DJV_TEXT("shortcut_playback_reset_out_point"), GLFW_KEY_O, GLFW_MOD_SHIFT);

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["Forward"]);
            p.menu->addAction(p.actions["Reverse"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["InPoint"]);
            p.menu->addAction(p.actions["OutPoint"]);
            p.menu->addAction(p.actions["StartFrame"]);
            p.menu->addAction(p.actions["EndFrame"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["NextFrame"]);
            p.menu->addAction(p.actions["NextFrame10"]);
            p.menu->addAction(p.actions["NextFrame100"]);
            p.menu->addAction(p.actions["PrevFrame"]);
            p.menu->addAction(p.actions["PrevFrame10"]);
            p.menu->addAction(p.actions["PrevFrame100"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["InOutPoints"]);
            p.menu->addAction(p.actions["SetInPoint"]);
            p.menu->addAction(p.actions["SetOutPoint"]);
            p.menu->addAction(p.actions["ResetInPoint"]);
            p.menu->addAction(p.actions["ResetOutPoint"]);

            _actionsUpdate();
            _textUpdate();
            _shortcutsUpdate();

            auto weak = std::weak_ptr<PlaybackSystem>(std::dynamic_pointer_cast<PlaybackSystem>(shared_from_this()));
            p.playbackActionGroup->setExclusiveCallback(
                [weak](int index)
            {
                if (auto system = weak.lock())
                {
                    if (auto media = system->_p->currentMedia)
                    {
                        Playback playback = Playback::Stop;
                        switch (index)
                        {
                        case 0: playback = system->_p->playback = Playback::Forward; break;
                        case 1: playback = system->_p->playback = Playback::Reverse; break;
                        }
                        media->setPlayback(playback);
                    }
                }
            });

            p.playbackModeActionGroup->setRadioCallback(
                [weak](int index)
            {
                if (auto system = weak.lock())
                {
                    if (auto media = system->_p->currentMedia)
                    {
                        media->setPlaybackMode(static_cast<PlaybackMode>(index));
                    }
                }
            });

            p.actions["Playback"]->setClickedCallback(
                [weak]
            {
                if (auto system = weak.lock())
                {
                    if (auto media = system->_p->currentMedia)
                    {
                        media->setPlayback(
                            Playback::Stop == media->observePlayback()->get() ?
                            (Playback::Stop == system->_p->playback ? Playback::Forward : system->_p->playback) :
                            Playback::Stop);
                    }
                }
            });

            p.actions["PlayEveryFrame"]->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        if (auto media = system->_p->currentMedia)
                        {
                            media->setPlayEveryFrame(value);
                        }
                    }
                });

            p.actions["InPoint"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        if (auto media = system->_p->currentMedia)
                        {
                            media->inPoint();
                        }
                    }
                });

            p.actions["OutPoint"]->setClickedCallback(
                [weak]
            {
                if (auto system = weak.lock())
                {
                    if (auto media = system->_p->currentMedia)
                    {
                        media->outPoint();
                    }
                }
            });

            p.actions["StartFrame"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        if (auto media = system->_p->currentMedia)
                        {
                            media->start();
                        }
                    }
                });

            p.actions["EndFrame"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        if (auto media = system->_p->currentMedia)
                        {
                            media->end();
                        }
                    }
                });

            p.actions["NextFrame"]->setClickedCallback(
                [weak]
            {
                if (auto system = weak.lock())
                {
                    if (auto media = system->_p->currentMedia)
                    {
                        media->nextFrame();
                    }
                }
            });

            p.actions["NextFrame10"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        if (auto media = system->_p->currentMedia)
                        {
                            media->nextFrame(10);
                        }
                    }
                });

            p.actions["NextFrame100"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        if (auto media = system->_p->currentMedia)
                        {
                            media->nextFrame(100);
                        }
                    }
                });

            p.actions["PrevFrame"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        if (auto media = system->_p->currentMedia)
                        {
                            media->prevFrame();
                        }
                    }
                });

            p.actions["PrevFrame10"]->setClickedCallback(
                [weak]
            {
                if (auto system = weak.lock())
                {
                    if (auto media = system->_p->currentMedia)
                    {
                        media->prevFrame(10);
                    }
                }
            });

            p.actions["PrevFrame100"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        if (auto media = system->_p->currentMedia)
                        {
                            media->prevFrame(100);
                        }
                    }
                });

            p.actions["InOutPoints"]->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        if (auto media = system->_p->currentMedia)
                        {
                            const auto& inOutPoints = media->observeInOutPoints()->get();
                            media->setInOutPoints(AV::IO::InOutPoints(value, inOutPoints.getIn(), inOutPoints.getOut()));
                        }
                    }
                });

            p.actions["SetInPoint"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        if (auto media = system->_p->currentMedia)
                        {
                            media->setInPoint();
                        }
                    }
                });

            p.actions["SetOutPoint"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        if (auto media = system->_p->currentMedia)
                        {
                            media->setOutPoint();
                        }
                    }
                });

            p.actions["ResetInPoint"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        if (auto media = system->_p->currentMedia)
                        {
                            media->resetInPoint();
                        }
                    }
                });

            p.actions["ResetOutPoint"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        if (auto media = system->_p->currentMedia)
                        {
                            media->resetOutPoint();
                        }
                    }
                });

            if (auto fileSystem = context->getSystemT<FileSystem>())
            {
                p.currentMediaObserver = Observer::Value<std::shared_ptr<Media> >::create(
                    fileSystem->observeCurrentMedia(),
                    [weak](const std::shared_ptr<Media> & value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->currentMedia = value;
                        system->_actionsUpdate();
                        if (value)
                        {
                            system->_p->speedObserver = Observer::Value<Math::IntRational>::create(
                                value->observeSpeed(),
                                [weak](const Math::IntRational& value)
                                {
                                    if (auto system = weak.lock())
                                    {
                                        system->_p->speed = value;
                                        system->_actionsUpdate();
                                    }
                                });

                            system->_p->playEveryFrameObserver = Observer::Value<bool>::create(
                                value->observePlayEveryFrame(),
                                [weak](bool value)
                                {
                                    if (auto system = weak.lock())
                                    {
                                        system->_p->playEveryFrame = value;
                                        system->_actionsUpdate();
                                    }
                                });

                            system->_p->sequenceObserver = Observer::Value<Math::Frame::Sequence>::create(
                                value->observeSequence(),
                                [weak](const Math::Frame::Sequence& value)
                                {
                                    if (auto system = weak.lock())
                                    {
                                        system->_p->sequence = value;
                                        system->_actionsUpdate();
                                    }
                                });

                            system->_p->playbackObserver = Observer::Value<Playback>::create(
                                value->observePlayback(),
                                [weak](Playback value)
                            {
                                if (auto system = weak.lock())
                                {
                                    int index = -1;
                                    switch (value)
                                    {
                                    case Playback::Forward:
                                        index = 0;
                                        system->_p->playback = value;
                                        break;
                                    case Playback::Reverse:
                                        index = 1;
                                        system->_p->playback = value;
                                        break;
                                    default: break;
                                    }
                                    system->_p->playbackActionGroup->setChecked(index);
                                }
                            });

                            system->_p->playbackModeObserver = Observer::Value<PlaybackMode>::create(
                                value->observePlaybackMode(),
                                [weak](PlaybackMode value)
                            {
                                if (auto system = weak.lock())
                                {
                                    system->_p->playbackModeActionGroup->setChecked(static_cast<int>(value), true);
                                }
                            });

                            system->_p->inOutPointsObserver = Observer::Value<AV::IO::InOutPoints>::create(
                                value->observeInOutPoints(),
                                [weak](const AV::IO::InOutPoints& value)
                                {
                                    if (auto system = weak.lock())
                                    {
                                        system->_p->inOutPoints = value;
                                        system->_actionsUpdate();
                                    }
                                });
                        }
                        else
                        {
                            system->_p->speed = Math::IntRational();
                            system->_p->sequence = Math::Frame::Sequence();
                            system->_p->playEveryFrame = false;
                            system->_p->playback = Playback::Stop;
                            system->_p->inOutPoints = AV::IO::InOutPoints();

                            system->_p->playbackActionGroup->setChecked(0, false);
                            system->_p->playbackActionGroup->setChecked(1, false);

                            system->_p->speedObserver.reset();
                            system->_p->sequenceObserver.reset();
                            system->_p->playEveryFrameObserver.reset();
                            system->_p->playbackObserver.reset();
                            system->_p->playbackModeObserver.reset();
                            system->_p->inOutPointsObserver.reset();

                            system->_actionsUpdate();
                        }
                    }
                });
            }

            if (auto windowSystem = context->getSystemT<WindowSystem>())
            {
                p.activeWidgetObserver = Observer::Value<std::shared_ptr<MediaWidget> >::create(
                    windowSystem->observeActiveWidget(),
                    [weak](const std::shared_ptr<MediaWidget>& value)
                    {
                        if (auto system = weak.lock())
                        {
                            if (value)
                            {
                                system->_p->hoverObserver = Observer::Value<PointerData>::create(
                                    value->observeHover(),
                                    [weak](const PointerData& value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->hoverPos = value.pos;
                                        }
                                    });
                                system->_p->dragObserver = Observer::Value<PointerData>::create(
                                    value->observeDrag(),
                                    [weak](const PointerData& value)
                                {
                                    if (auto system = weak.lock())
                                    {
                                        system->_p->drag(value, system->getContext());
                                    }
                                });
                                system->_p->scrollObserver = Observer::Value<ScrollData>::create(
                                    value->observeScroll(),
                                    [weak](const ScrollData& value)
                                {
                                    if (auto system = weak.lock())
                                    {
                                        system->_p->scroll(value, system->getContext());
                                    }
                                });
                            }
                            else
                            {
                                system->_p->hoverObserver.reset();
                                system->_p->dragObserver.reset();
                            }
                        }
                    });
            }

            _logInitTime();
        }

        PlaybackSystem::PlaybackSystem() :
            _p(new Private)
        {}

        PlaybackSystem::~PlaybackSystem()
        {}

        std::shared_ptr<PlaybackSystem> PlaybackSystem::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = context->getSystemT<PlaybackSystem>();
            if (!out)
            {
                out = std::shared_ptr<PlaybackSystem>(new PlaybackSystem);
                out->_init(context);
            }
            return out;
        }

        int PlaybackSystem::getSortKey() const
        {
            return 6;
        }

        std::map<std::string, std::shared_ptr<UI::Action>> PlaybackSystem::getActions() const
        {
            return _p->actions;
        }

        std::vector<std::shared_ptr<UI::Menu> > PlaybackSystem::getMenus() const
        {
            return { _p->menu };
        }

        void PlaybackSystem::_actionsUpdate()
        {
            DJV_PRIVATE_PTR();
            const bool playable =
                (p.currentMedia ? true : false) &&
                (p.sequence.getFrameCount() > 1);
            p.actions["Forward"]->setEnabled(playable);
            p.actions["Reverse"]->setEnabled(playable);
            p.actions["PlayOnce"]->setEnabled(playable);
            p.actions["PlayLoop"]->setEnabled(playable);
            p.actions["PlayPingPong"]->setEnabled(playable);
            p.actions["PlayEveryFrame"]->setEnabled(playable);
            p.actions["PlayEveryFrame"]->setChecked(p.playEveryFrame);
            p.actions["InPoint"]->setEnabled(playable);
            p.actions["OutPoint"]->setEnabled(playable);
            p.actions["StartFrame"]->setEnabled(playable);
            p.actions["EndFrame"]->setEnabled(playable);
            p.actions["NextFrame"]->setEnabled(playable);
            p.actions["NextFrame10"]->setEnabled(playable);
            p.actions["NextFrame100"]->setEnabled(playable);
            p.actions["PrevFrame"]->setEnabled(playable);
            p.actions["PrevFrame10"]->setEnabled(playable);
            p.actions["PrevFrame100"]->setEnabled(playable);
            p.actions["InOutPoints"]->setEnabled(playable);
            p.actions["InOutPoints"]->setChecked(p.inOutPoints.isEnabled());
            p.actions["SetInPoint"]->setEnabled(playable);
            p.actions["SetOutPoint"]->setEnabled(playable);
            p.actions["ResetInPoint"]->setEnabled(playable && p.inOutPoints.getIn() != 0);
            p.actions["ResetOutPoint"]->setEnabled(playable && p.inOutPoints.getOut() != p.sequence.getLastIndex());
        }

        void PlaybackSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Forward"]->setText(_getText(DJV_TEXT("menu_playback_forward")));
                p.actions["Forward"]->setTooltip(_getText(DJV_TEXT("menu_playback_forward_tooltip")));
                p.actions["Reverse"]->setText(_getText(DJV_TEXT("menu_playback_reverse")));
                p.actions["Reverse"]->setTooltip(_getText(DJV_TEXT("menu_playback_reverse_tooltip")));
                p.actions["PlayOnce"]->setText(_getText(DJV_TEXT("menu_playback_play_once")));
                p.actions["PlayOnce"]->setTooltip(_getText(DJV_TEXT("menu_playback_play_once_tooltip")));
                p.actions["PlayLoop"]->setText(_getText(DJV_TEXT("loop")));
                p.actions["PlayLoop"]->setTooltip(_getText(DJV_TEXT("menu_playback_loop_tooltip")));
                p.actions["PlayPingPong"]->setText(_getText(DJV_TEXT("menu_playback_ping_pong")));
                p.actions["PlayPingPong"]->setTooltip(_getText(DJV_TEXT("menu_playback_ping_pong_tooltip")));
                p.actions["PlayEveryFrame"]->setText(_getText(DJV_TEXT("menu_playback_play_every_frame")));
                p.actions["PlayEveryFrame"]->setTooltip(_getText(DJV_TEXT("menu_playback_play_every_frame_tooltip")));
                p.actions["InPoint"]->setText(_getText(DJV_TEXT("menu_playback_go_to_in_point")));
                p.actions["InPoint"]->setTooltip(_getText(DJV_TEXT("menu_playback_go_to_in_point_tooltip")));
                p.actions["OutPoint"]->setText(_getText(DJV_TEXT("menu_playback_go_to_out_point")));
                p.actions["OutPoint"]->setTooltip(_getText(DJV_TEXT("menu_playback_go_to_out_point_tooltip")));
                p.actions["StartFrame"]->setText(_getText(DJV_TEXT("menu_playback_go_to_start_frame")));
                p.actions["StartFrame"]->setTooltip(_getText(DJV_TEXT("menu_playback_go_to_start_frame_tooltip")));
                p.actions["EndFrame"]->setText(_getText(DJV_TEXT("menu_playback_go_to_end_frame")));
                p.actions["EndFrame"]->setTooltip(_getText(DJV_TEXT("menu_playback_go_to_end_frame_tooltip")));
                p.actions["NextFrame"]->setText(_getText(DJV_TEXT("menu_playback_next_frame")));
                p.actions["NextFrame"]->setTooltip(_getText(DJV_TEXT("menu_playback_next_frame_tooltip")));
                p.actions["NextFrame10"]->setText(_getText(DJV_TEXT("menu_playback_next_frame_x10")));
                p.actions["NextFrame10"]->setTooltip(_getText(DJV_TEXT("menu_playback_next_frame_x10_tooltip")));
                p.actions["NextFrame100"]->setText(_getText(DJV_TEXT("menu_playback_next_frame_x100")));
                p.actions["NextFrame100"]->setTooltip(_getText(DJV_TEXT("menu_playback_next_frame_x100_tooltip")));
                p.actions["PrevFrame"]->setText(_getText(DJV_TEXT("menu_playback_previous_frame")));
                p.actions["PrevFrame"]->setTooltip(_getText(DJV_TEXT("menu_playback_previous_frame_tooltip")));
                p.actions["PrevFrame10"]->setText(_getText(DJV_TEXT("menu_playback_previous_frame_x10")));
                p.actions["PrevFrame10"]->setTooltip(_getText(DJV_TEXT("menu_playback_previous_frame_x10_tooltip")));
                p.actions["PrevFrame100"]->setText(_getText(DJV_TEXT("menu_playback_previous_frame_x100")));
                p.actions["PrevFrame100"]->setTooltip(_getText(DJV_TEXT("menu_playback_previous_frame_x100_tooltip")));
                p.actions["InOutPoints"]->setText(_getText(DJV_TEXT("menu_playback_enable_in_out_points")));
                p.actions["InOutPoints"]->setTooltip(_getText(DJV_TEXT("menu_playback_enable_in_out_points_tooltip")));
                p.actions["SetInPoint"]->setText(_getText(DJV_TEXT("menu_playback_set_in_point")));
                p.actions["SetInPoint"]->setTooltip(_getText(DJV_TEXT("menu_playback_set_in_point_tooltip")));
                p.actions["SetOutPoint"]->setText(_getText(DJV_TEXT("menu_playback_set_out_point")));
                p.actions["SetOutPoint"]->setTooltip(_getText(DJV_TEXT("menu_playback_set_out_point_tooltip")));
                p.actions["ResetInPoint"]->setText(_getText(DJV_TEXT("menu_playback_reset_in_point")));
                p.actions["ResetInPoint"]->setTooltip(_getText(DJV_TEXT("menu_playback_reset_in_point_tooltip")));
                p.actions["ResetOutPoint"]->setText(_getText(DJV_TEXT("menu_playback_reset_out_point")));
                p.actions["ResetOutPoint"]->setTooltip(_getText(DJV_TEXT("menu_playback_reset_out_point_tooltip")));

                p.menu->setText(_getText(DJV_TEXT("menu_playback")));
            }
        }

        void PlaybackSystem::_shortcutsUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Forward"]->setShortcuts(_getShortcuts("shortcut_playback_forward"));
                p.actions["Reverse"]->setShortcuts(_getShortcuts("shortcut_playback_reverse"));
                p.actions["Playback"]->setShortcuts(_getShortcuts("shortcut_playback_playback"));
                p.actions["InPoint"]->setShortcuts(_getShortcuts("shortcut_playback_in_point"));
                p.actions["OutPoint"]->setShortcuts(_getShortcuts("shortcut_playback_out_point"));
                p.actions["StartFrame"]->setShortcuts(_getShortcuts("shortcut_playback_start_frame"));
                p.actions["EndFrame"]->setShortcuts(_getShortcuts("shortcut_playback_end_frame"));
                p.actions["NextFrame"]->setShortcuts(_getShortcuts("shortcut_playback_next_frame"));
                p.actions["NextFrame10"]->setShortcuts(_getShortcuts("shortcut_playback_next_frame_10"));
                p.actions["NextFrame100"]->setShortcuts(_getShortcuts("shortcut_playback_next_frame_100"));
                p.actions["PrevFrame"]->setShortcuts(_getShortcuts("shortcut_playback_prev_frame"));
                p.actions["PrevFrame10"]->setShortcuts(_getShortcuts("shortcut_playback_prev_frame_10"));
                p.actions["PrevFrame100"]->setShortcuts(_getShortcuts("shortcut_playback_prev_frame_100"));
                p.actions["InOutPoints"]->setShortcuts(_getShortcuts("shortcut_playback_in_out_points"));
                p.actions["SetInPoint"]->setShortcuts(_getShortcuts("shortcut_playback_set_in_point"));
                p.actions["SetOutPoint"]->setShortcuts(_getShortcuts("shortcut_playback_set_out_point"));
                p.actions["ResetInPoint"]->setShortcuts(_getShortcuts("shortcut_playback_reset_in_point"));
                p.actions["ResetOutPoint"]->setShortcuts(_getShortcuts("shortcut_playback_reset_out_point"));
            }
        }

        float PlaybackSystem::_getScrollWheelSpeed(UI::ScrollWheelSpeed value)
        {
            const float values[] =
            {
                1.F,
                2.F,
                5.F
            };
            return values[static_cast<size_t>(value)];
        }

        void PlaybackSystem::Private::drag(const PointerData& value, const std::weak_ptr<System::Context>& contextWeak)
        {
            if (auto media = currentMedia)
            {
                bool scrub = false;
                auto i = value.buttons.find(1);
                scrub |=
                    1 == value.buttons.size() &&
                    i != value.buttons.end() &&
                    (GLFW_KEY_LEFT_CONTROL == value.key || GLFW_KEY_RIGHT_CONTROL == value.key ) &&
                    value.keyModifiers & GLFW_MOD_CONTROL;
                i = value.buttons.find(3);
                scrub |=
                    1 == value.buttons.size() &&
                    i != value.buttons.end() &&
                    0 == value.key &&
                    0 == value.keyModifiers;
                if (scrub)
                {
                    switch (value.state)
                    {
                    case PointerState::Start:
                        dragStart = value.pos;
                        dragStartFrame = media->observeCurrentFrame()->get();
                        dragStartPlayback = media->observePlayback()->get();
                        break;
                    case PointerState::Move:
                    {
                        if (dragStartFrame != Math::Frame::invalidIndex)
                        {
                            if (auto context = contextWeak.lock())
                            {
                                auto uiSystem = context->getSystemT<UI::UISystem>();
                                auto style = uiSystem->getStyle();
                                const Math::Frame::Index offset =
                                    (value.pos.x - dragStart.x) / style->getMetric(UI::MetricsRole::Scrub);
                                media->setCurrentFrame(dragStartFrame + offset);
                            }
                        }
                        break;
                    }
                    case PointerState::End:
                        media->setPlayback(dragStartPlayback);
                        break;
                    default: break;
                    }
                }
            }
        }

        void PlaybackSystem::Private::scroll(const ScrollData& value, const std::weak_ptr<System::Context>& contextWeak)
        {
            if (auto media = currentMedia)
            {
                bool scrub =
                    (GLFW_KEY_LEFT_CONTROL == value.key || GLFW_KEY_RIGHT_CONTROL == value.key) &&
                    value.keyModifiers & GLFW_MOD_CONTROL;
                if (scrub)
                {
                    if (auto context = contextWeak.lock())
                    {
                        Math::Frame::Index frame = media->observeCurrentFrame()->get();
                        auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                        auto mouseSettings = settingsSystem->getSettingsT<UI::Settings::Mouse>();
                        const float speed = _getScrollWheelSpeed(mouseSettings->observeScrollWheelSpeed()->get());
                        media->setCurrentFrame(frame + value.delta.y * speed);
                    }
                }
            }
        }

    } // namespace ViewApp
} // namespace djv
