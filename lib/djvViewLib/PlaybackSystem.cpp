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

#include <djvViewLib/PlaybackSystem.h>

#include <djvViewLib/Application.h>
#include <djvViewLib/FileSystem.h>
#include <djvViewLib/Media.h>
#include <djvViewLib/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct PlaybackSystem::Private
        {
            std::shared_ptr<Media> currentMedia;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ActionGroup> playbackActionGroup;
            std::shared_ptr<UI::Menu> menu;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
            std::shared_ptr<ValueObserver<Playback> > playbackObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<ValueObserver<std::string> > localeObserver;
        };

        void PlaybackSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewLib::PlaybackSystem", context);

            DJV_PRIVATE_PTR();
            p.actions["Forward"] = UI::Action::create();
            p.actions["Forward"]->setIcon("djvIconPlaybackForward");
            p.actions["Forward"]->addShortcut(GLFW_KEY_SPACE);
            p.actions["Forward"]->addShortcut(GLFW_KEY_UP);
            p.actions["Reverse"] = UI::Action::create();
            p.actions["Reverse"]->setIcon("djvIconPlaybackReverse");
            p.actions["Reverse"]->setShortcut(GLFW_KEY_DOWN);
            p.playbackActionGroup = UI::ActionGroup::create(UI::ButtonType::Exclusive);
            p.playbackActionGroup->addAction(p.actions["Forward"]);
            p.playbackActionGroup->addAction(p.actions["Reverse"]);

            //! \todo Implement me!
            p.actions["PlayEveryFrame"] = UI::Action::create();
            p.actions["PlayEveryFrame"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["PlayEveryFrame"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["InPoint"] = UI::Action::create();
            p.actions["InPoint"]->setIcon("djvIconFrameStart");
            p.actions["InPoint"]->setShortcut(GLFW_KEY_HOME);
            p.actions["InPoint"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["OutPoint"] = UI::Action::create();
            p.actions["OutPoint"]->setIcon("djvIconFrameEnd");
            p.actions["OutPoint"]->setShortcut(GLFW_KEY_END);
            p.actions["OutPoint"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["StartFrame"] = UI::Action::create();
            p.actions["StartFrame"]->setShortcut(GLFW_KEY_HOME, GLFW_MOD_SHIFT);
            p.actions["StartFrame"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["EndFrame"] = UI::Action::create();
            p.actions["EndFrame"]->setShortcut(GLFW_KEY_END, GLFW_MOD_SHIFT);
            p.actions["EndFrame"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["NextFrame"] = UI::Action::create();
            p.actions["NextFrame"]->setIcon("djvIconFrameNext");
            p.actions["NextFrame"]->addShortcut(GLFW_KEY_RIGHT);
            p.actions["NextFrame"]->addShortcut(GLFW_KEY_RIGHT_BRACKET);
            p.actions["NextFrame"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["NextFrame10"] = UI::Action::create();
            p.actions["NextFrame10"]->addShortcut(GLFW_KEY_RIGHT, GLFW_MOD_SHIFT);
            p.actions["NextFrame10"]->addShortcut(GLFW_KEY_RIGHT_BRACKET, GLFW_MOD_SHIFT);
            p.actions["NextFrame10"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["NextFrame100"] = UI::Action::create();
            p.actions["NextFrame100"]->addShortcut(GLFW_KEY_RIGHT, GLFW_MOD_CONTROL);
            p.actions["NextFrame100"]->addShortcut(GLFW_KEY_RIGHT_BRACKET, GLFW_MOD_CONTROL);
            p.actions["NextFrame100"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["PrevFrame"] = UI::Action::create();
            p.actions["PrevFrame"]->setIcon("djvIconFramePrev");
            p.actions["PrevFrame"]->addShortcut(GLFW_KEY_LEFT);
            p.actions["PrevFrame"]->addShortcut(GLFW_KEY_LEFT_BRACKET);
            p.actions["PrevFrame"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["PrevFrame10"] = UI::Action::create();
            p.actions["PrevFrame10"]->addShortcut(GLFW_KEY_LEFT, GLFW_MOD_SHIFT);
            p.actions["PrevFrame10"]->addShortcut(GLFW_KEY_LEFT_BRACKET, GLFW_MOD_SHIFT);
            p.actions["PrevFrame10"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["PrevFrame100"] = UI::Action::create();
            p.actions["PrevFrame100"]->addShortcut(GLFW_KEY_LEFT, GLFW_MOD_CONTROL);
            p.actions["PrevFrame100"]->addShortcut(GLFW_KEY_LEFT_BRACKET, GLFW_MOD_CONTROL);
            p.actions["PrevFrame100"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["InOutPoints"] = UI::Action::create();
            p.actions["InOutPoints"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["InOutPoints"]->setShortcut(GLFW_KEY_P);
            p.actions["InOutPoints"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["SetInPoint"] = UI::Action::create();
            p.actions["SetInPoint"]->setShortcut(GLFW_KEY_I);
            p.actions["SetInPoint"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["SetOutPoint"] = UI::Action::create();
            p.actions["SetOutPoint"]->setShortcut(GLFW_KEY_O);
            p.actions["SetOutPoint"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["ResetInPoint"] = UI::Action::create();
            p.actions["ResetInPoint"]->setShortcut(GLFW_KEY_I, GLFW_MOD_SHIFT);
            p.actions["ResetInPoint"]->setEnabled(false);
            //! \todo Implement me!
            p.actions["ResetOutPoint"] = UI::Action::create();
            p.actions["ResetOutPoint"]->setShortcut(GLFW_KEY_O, GLFW_MOD_SHIFT);
            p.actions["ResetOutPoint"]->setEnabled(false);

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["Forward"]);
            p.menu->addAction(p.actions["Reverse"]);
            p.menu->addAction(p.actions["PlayEveryFrame"]);
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
                        case 0: playback = Playback::Forward; break;
                        case 1: playback = Playback::Reverse; break;
                        }
                        media->setPlayback(playback);
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
                        system->_p->actions["Forward"]->setEnabled(value ? true : false);
                        system->_p->actions["Reverse"]->setEnabled(value ? true : false);
                        if (value)
                        {
                            system->_p->playbackObserver = ValueObserver<Playback>::create(
                                value->observePlayback(),
                                [weak](Playback value)
                            {
                                if (auto system = weak.lock())
                                {
                                    system->_p->playbackActionGroup->setChecked(0, Playback::Forward == value);
                                    system->_p->playbackActionGroup->setChecked(1, Playback::Reverse == value);
                                }
                            });
                        }
                        else
                        {
                            system->_p->playbackActionGroup->setChecked(0, false);
                            system->_p->playbackActionGroup->setChecked(1, false);
                            system->_p->playbackObserver.reset();
                        }
                    }
                });
            }

            p.localeObserver = ValueObserver<std::string>::create(
                context->getSystemT<TextSystem>()->observeCurrentLocale(),
                [weak](const std::string & value)
            {
                if (auto system = weak.lock())
                {
                    system->_textUpdate();
                }
            });
        }

        PlaybackSystem::PlaybackSystem() :
            _p(new Private)
        {}

        PlaybackSystem::~PlaybackSystem()
        {}

        std::shared_ptr<PlaybackSystem> PlaybackSystem::create(Context * context)
        {
            auto out = std::shared_ptr<PlaybackSystem>(new PlaybackSystem);
            out->_init(context);
            return out;
        }

        std::map<std::string, std::shared_ptr<UI::Action> > PlaybackSystem::getActions()
        {
            return _p->actions;
        }

        MenuData PlaybackSystem::getMenu()
        {
            return
            {
                _p->menu,
                "E"
            };
        }

        void PlaybackSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            auto context = getContext();
            p.actions["Forward"]->setTitle(_getText(DJV_TEXT("Forward")));
            p.actions["Forward"]->setTooltip(_getText(DJV_TEXT("Forward tooltip")));
            p.actions["Reverse"]->setTitle(_getText(DJV_TEXT("Reverse")));
            p.actions["Reverse"]->setTooltip(_getText(DJV_TEXT("Reverse tooltip")));
            p.actions["PlayEveryFrame"]->setTitle(_getText(DJV_TEXT("Play Every Frame")));
            p.actions["PlayEveryFrame"]->setTooltip(_getText(DJV_TEXT("Play every frame tooltip")));
            p.actions["InPoint"]->setTitle(_getText(DJV_TEXT("Go to In Point")));
            p.actions["InPoint"]->setTooltip(_getText(DJV_TEXT("Go to in point tooltip")));
            p.actions["OutPoint"]->setTitle(_getText(DJV_TEXT("Go to Out Point")));
            p.actions["OutPoint"]->setTooltip(_getText(DJV_TEXT("Go to out point tooltip")));
            p.actions["StartFrame"]->setTitle(_getText(DJV_TEXT("Go to Start Frame")));
            p.actions["StartFrame"]->setTooltip(_getText(DJV_TEXT("Go to start frame tooltip")));
            p.actions["EndFrame"]->setTitle(_getText(DJV_TEXT("Go to End Frame")));
            p.actions["EndFrame"]->setTooltip(_getText(DJV_TEXT("Go to end frame tooltip")));
            p.actions["NextFrame"]->setTitle(_getText(DJV_TEXT("Next Frame")));
            p.actions["NextFrame"]->setTooltip(_getText(DJV_TEXT("Next frame tooltip")));
            p.actions["NextFrame10"]->setTitle(_getText(DJV_TEXT("Next Frame X10")));
            p.actions["NextFrame10"]->setTooltip(_getText(DJV_TEXT("Next frame X10 tooltip")));
            p.actions["NextFrame100"]->setTitle(_getText(DJV_TEXT("Next Frame X100")));
            p.actions["NextFrame100"]->setTooltip(_getText(DJV_TEXT("Next frame X100 tooltip")));
            p.actions["PrevFrame"]->setTitle(_getText(DJV_TEXT("Previous Frame")));
            p.actions["PrevFrame"]->setTooltip(_getText(DJV_TEXT("Previous frame tooltip")));
            p.actions["PrevFrame10"]->setTitle(_getText(DJV_TEXT("Previous Frame X10")));
            p.actions["PrevFrame10"]->setTooltip(_getText(DJV_TEXT("Previous frame X10 tooltip")));
            p.actions["PrevFrame100"]->setTitle(_getText(DJV_TEXT("Previous Frame X100")));
            p.actions["PrevFrame100"]->setTooltip(_getText(DJV_TEXT("Previous frame X100 tooltip")));
            p.actions["InOutPoints"]->setTitle(_getText(DJV_TEXT("Enable In/Out Points")));
            p.actions["InOutPoints"]->setTooltip(_getText(DJV_TEXT("Enable in/out points tooltip")));
            p.actions["SetInPoint"]->setTitle(_getText(DJV_TEXT("Set In Point")));
            p.actions["SetInPoint"]->setTooltip(_getText(DJV_TEXT("Set in point tooltip")));
            p.actions["SetOutPoint"]->setTitle(_getText(DJV_TEXT("Set Out Point")));
            p.actions["SetOutPoint"]->setTooltip(_getText(DJV_TEXT("Set out point tooltip")));
            p.actions["ResetInPoint"]->setTitle(_getText(DJV_TEXT("Reset In Point")));
            p.actions["ResetInPoint"]->setTooltip(_getText(DJV_TEXT("Reset in point tooltip")));
            p.actions["ResetOutPoint"]->setTitle(_getText(DJV_TEXT("Reset Out Point")));
            p.actions["ResetOutPoint"]->setTooltip(_getText(DJV_TEXT("Reset out point tooltip")));

            p.menu->setText(_getText(DJV_TEXT("Playback")));
        }

    } // namespace ViewLib
} // namespace djv
