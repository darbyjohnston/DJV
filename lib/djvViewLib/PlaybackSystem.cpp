//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Menu.h>

#include <djvCore/Context.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct PlaybackSystem::Private
        {
            std::shared_ptr<Media> media;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::map<std::string, std::shared_ptr<UI::Menu> > menus;
            std::shared_ptr<UI::ActionGroup> playbackActionGroup;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
            std::shared_ptr<ValueObserver<Playback> > playbackObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
        };

        void PlaybackSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewLib::PlaybackSystem", context);

            DJV_PRIVATE_PTR();
            p.actions["Stop"] = UI::Action::create();
            p.actions["Stop"]->setIcon("djvIconPlaybackStop");
            p.actions["Stop"]->setShortcut(GLFW_KEY_K);
            p.actions["Stop"]->setEnabled(false);

            p.actions["Forward"] = UI::Action::create();
            p.actions["Forward"]->setIcon("djvIconPlaybackForward");
            p.actions["Forward"]->setShortcut(GLFW_KEY_L);
            p.actions["Forward"]->setEnabled(false);

            p.actions["Reverse"] = UI::Action::create();
            p.actions["Reverse"]->setIcon("djvIconPlaybackReverse");
            p.actions["Reverse"]->setShortcut(GLFW_KEY_J);
            p.actions["Reverse"]->setEnabled(false);

            p.playbackActionGroup = UI::ActionGroup::create(UI::ButtonType::Radio);
            p.playbackActionGroup->addAction(p.actions["Stop"]);
            p.playbackActionGroup->addAction(p.actions["Forward"]);
            p.playbackActionGroup->addAction(p.actions["Reverse"]);

            //! \todo Implement me!
            p.actions["TogglePlayback"] = UI::Action::create();
            p.actions["TogglePlayback"]->addShortcut(GLFW_KEY_SPACE);
            p.actions["TogglePlayback"]->addShortcut(GLFW_KEY_UP);
            p.actions["TogglePlayback"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["ToggleReversePlayback"] = UI::Action::create();
            p.actions["ToggleReversePlayback"]->setShortcut(GLFW_KEY_DOWN);
            p.actions["ToggleReversePlayback"]->setEnabled(false);

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

            p.menus["Playback"] = UI::Menu::create(context);
            p.menus["Playback"]->addAction(p.actions["Stop"]);
            p.menus["Playback"]->addAction(p.actions["Forward"]);
            p.menus["Playback"]->addAction(p.actions["Reverse"]);
            //! \todo Implement me!
            p.menus["Loop"] = UI::Menu::create(context);
            p.menus["Playback"]->addMenu(p.menus["Loop"]);
            p.menus["Playback"]->addAction(p.actions["TogglePlayback"]);
            p.menus["Playback"]->addAction(p.actions["ToggleReversePlayback"]);
            p.menus["Playback"]->addAction(p.actions["PlayEveryFrame"]);
            p.menus["Playback"]->addSeparator();
            p.menus["Playback"]->addAction(p.actions["InPoint"]);
            p.menus["Playback"]->addAction(p.actions["OutPoint"]);
            p.menus["Playback"]->addAction(p.actions["StartFrame"]);
            p.menus["Playback"]->addAction(p.actions["EndFrame"]);
            p.menus["Playback"]->addAction(p.actions["NextFrame"]);
            p.menus["Playback"]->addAction(p.actions["NextFrame10"]);
            p.menus["Playback"]->addAction(p.actions["NextFrame100"]);
            p.menus["Playback"]->addAction(p.actions["PrevFrame"]);
            p.menus["Playback"]->addAction(p.actions["PrevFrame10"]);
            p.menus["Playback"]->addAction(p.actions["PrevFrame100"]);
            p.menus["Playback"]->addSeparator();
            p.menus["Playback"]->addAction(p.actions["InOutPoints"]);
            p.menus["Playback"]->addAction(p.actions["SetInPoint"]);
            p.menus["Playback"]->addAction(p.actions["SetOutPoint"]);
            p.menus["Playback"]->addAction(p.actions["ResetInPoint"]);
            p.menus["Playback"]->addAction(p.actions["ResetOutPoint"]);
            //! \todo Implement me!
            p.menus["Layout"] = UI::Menu::create(context);
            p.menus["Playback"]->addMenu(p.menus["Layout"]);

            auto weak = std::weak_ptr<PlaybackSystem>(std::dynamic_pointer_cast<PlaybackSystem>(shared_from_this()));
            p.playbackActionGroup->setRadioCallback(
                [weak](int value)
            {
                if (auto system = weak.lock())
                {
                    if (auto media = system->_p->media)
                    {
                        media->setPlayback(static_cast<Playback>(value));
                    }
                }
            });
            
            if (auto fileSystem = context->getSystemT<FileSystem>().lock())
            {
                p.currentMediaObserver = ValueObserver<std::shared_ptr<Media> >::create(
                    fileSystem->observeCurrentMedia(),
                    [weak](const std::shared_ptr<Media> & value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->media = value;
                        system->_p->actions["Stop"]->setEnabled(value ? true : false);
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
                                    system->_p->playbackActionGroup->setChecked(static_cast<int>(value));
                                }
                            });
                        }
                        else
                        {
                            system->_p->playbackActionGroup->setChecked(0);
                            system->_p->playbackObserver.reset();
                        }
                    }
                });
            }
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

        NewMenu PlaybackSystem::getMenu()
        {
            DJV_PRIVATE_PTR();
            return { p.menus["Playback"], "E" };
        }

        void PlaybackSystem::_localeEvent(Event::Locale &)
        {
            DJV_PRIVATE_PTR();
            p.actions["Stop"]->setText(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Stop")));
            p.actions["Stop"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Stop Tooltip")));
            p.actions["Forward"]->setText(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Forward")));
            p.actions["Forward"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Forward Tooltip")));
            p.actions["Reverse"]->setText(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Reverse")));
            p.actions["Reverse"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Reverse Tooltip")));
            p.actions["TogglePlayback"]->setText(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Toggle Playback")));
            p.actions["TogglePlayback"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Toggle Playback Tooltip")));
            p.actions["ToggleReversePlayback"]->setText(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Toggle Reverse Playback")));
            p.actions["ToggleReversePlayback"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Toggle Reverse Playback Tooltip")));
            p.actions["PlayEveryFrame"]->setText(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Play Every Frame")));
            p.actions["PlayEveryFrame"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Play Every Frame Tooltip")));
            p.actions["InPoint"]->setText(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Go To In Point")));
            p.actions["InPoint"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Go To In Point Tooltip")));
            p.actions["OutPoint"]->setText(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Go To Out Point")));
            p.actions["OutPoint"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Go To Out Point Tooltip")));
            p.actions["StartFrame"]->setText(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Go To Start Frame")));
            p.actions["StartFrame"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Go To Start Frame Tooltip")));
            p.actions["EndFrame"]->setText(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Go To End Frame")));
            p.actions["EndFrame"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Go To End Frame Tooltip")));
            p.actions["NextFrame"]->setText(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Next Frame")));
            p.actions["NextFrame"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Next Frame Tooltip")));
            p.actions["NextFrame10"]->setText(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Next Frame X10")));
            p.actions["NextFrame10"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Next Frame X10 Tooltip")));
            p.actions["NextFrame100"]->setText(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Next Frame X100")));
            p.actions["NextFrame100"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Next Frame X100 Tooltip")));
            p.actions["PrevFrame"]->setText(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Previous Frame")));
            p.actions["PrevFrame"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Previous Frame Tooltip")));
            p.actions["PrevFrame10"]->setText(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Previous Frame X10")));
            p.actions["PrevFrame10"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Previous Frame X10 Tooltip")));
            p.actions["PrevFrame100"]->setText(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Previous Frame X100")));
            p.actions["PrevFrame100"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Previous Frame X100 Tooltip")));
            p.actions["InOutPoints"]->setText(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Enable In/Out Points")));
            p.actions["InOutPoints"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Enable In/Out Points Tooltip")));
            p.actions["SetInPoint"]->setText(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Set In Point")));
            p.actions["SetInPoint"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Set In Point Tooltip")));
            p.actions["SetOutPoint"]->setText(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Set Out Point")));
            p.actions["SetOutPoint"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Set Out Point Tooltip")));
            p.actions["ResetInPoint"]->setText(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Reset In Point")));
            p.actions["ResetInPoint"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Reset In Point Tooltip")));
            p.actions["ResetOutPoint"]->setText(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Reset Out Point")));
            p.actions["ResetOutPoint"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Reset Out Point Tooltip")));

            p.menus["Playback"]->setMenuName(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Playback")));
            p.menus["Loop"]->setMenuName(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Loop")));
            p.menus["Layout"]->setMenuName(_getText(DJV_TEXT("djv::ViewLib::PlaybackSystem", "Layout")));
        }

    } // namespace ViewLib
} // namespace djv
