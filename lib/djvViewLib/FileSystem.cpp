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

#include <djvViewLib/FileSystem.h>

#include <djvViewLib/Application.h>
#include <djvViewLib/Media.h>
#include <djvViewLib/MediaSession.h>

#include <djvUI/Action.h>
#include <djvUI/Dialog.h>
#include <djvUI/FileBrowser.h>
#include <djvUI/Menu.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct FileSystem::Private
        {
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<MediaSession> mediaSession;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
        };

        void FileSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewLib::FileSystem", context);
            DJV_PRIVATE_PTR();

            _p->actions["Open"] = UI::Action::create();
            _p->actions["Open"]->setText("Open");
            _p->actions["Open"]->setShortcut(GLFW_KEY_O, GLFW_MOD_CONTROL);

            _p->actions["Reload"] = UI::Action::create();
            _p->actions["Reload"]->setText("Reload");

            _p->actions["Close"] = UI::Action::create();
            _p->actions["Close"]->setText("Close");

            _p->actions["Export"] = UI::Action::create();
            _p->actions["Export"]->setText("Export");

            _p->actions["Exit"] = UI::Action::create();
            _p->actions["Exit"]->setText("Exit");
            _p->actions["Exit"]->setShortcut(GLFW_KEY_Q, GLFW_MOD_CONTROL);

            auto weak = std::weak_ptr<FileSystem>(std::dynamic_pointer_cast<FileSystem>(shared_from_this()));
            _p->clickedObservers["Open"] = ValueObserver<bool>::create(
                _p->actions["Open"]->observeClicked(),
                [weak, context](bool value)
            {
                if (value)
                {
                    if (auto system = weak.lock())
                    {
                        if (auto fileBrowser = context->getSystemT<UI::FileBrowser::DialogSystem>().lock())
                        {
                            fileBrowser->show(
                                [weak, context](const Core::FileSystem::FileInfo & value)
                            {
                                if (auto system = weak.lock())
                                {
                                    if (system->_p->mediaSession)
                                    {
                                        system->_p->mediaSession->openMedia(value.getPath());
                                    }
                                }
                            });
                        }
                    }
                }
            });

            _p->clickedObservers["Exit"] = ValueObserver<bool>::create(
                _p->actions["Exit"]->observeClicked(),
                [weak, context](bool value)
            {
                if (value)
                {
                    if (auto system = weak.lock())
                    {
                        if (auto dialogSystem = context->getSystemT<UI::DialogSystem>().lock())
                        {
                            dialogSystem->confirmation(
                                DJV_TEXT("Are you sure you want to exit?"),
                                DJV_TEXT("Yes"),
                                DJV_TEXT("No"),
                                [context](bool value)
                            {
                                if (value)
                                {
                                    dynamic_cast<Application *>(context)->exit();
                                }
                            });
                        }
                    }
                }
            });
        }

        FileSystem::FileSystem() :
            _p(new Private)
        {}

        FileSystem::~FileSystem()
        {}

        std::shared_ptr<FileSystem> FileSystem::create(Context * context)
        {
            auto out = std::shared_ptr<FileSystem>(new FileSystem);
            out->_init(context);
            return out;
        }

        std::map<std::string, std::shared_ptr<UI::Action> > FileSystem::getActions()
        {
            return _p->actions;
        }
        
        std::string FileSystem::getMenuSortKey() const
        {
            return "0";
        }
        
        std::shared_ptr<UI::Menu> FileSystem::createMenu()
        {
            DJV_PRIVATE_PTR();
            auto menu = UI::Menu::create("File", getContext());
            menu->addAction(p.actions["Open"]);
            menu->addAction(p.actions["Close"]);
            menu->addAction(p.actions["Export"]);
            menu->addSeparator();
            menu->addAction(p.actions["Exit"]);
            return menu;
        }

        void FileSystem::setMediaSession(const std::shared_ptr<MediaSession> & session)
        {
            DJV_PRIVATE_PTR();
            p.actions["Open"]->setEnabled(session ? true : false);
            p.mediaSession = session;
            auto weak = std::weak_ptr<FileSystem>(std::dynamic_pointer_cast<FileSystem>(shared_from_this()));
            _p->currentMediaObserver = ValueObserver<std::shared_ptr<Media> >::create(
                session->getCurrentMedia(),
                [weak](const std::shared_ptr<Media> & value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->actions["Close"]->setEnabled(value ? true : false);
                    system->_p->actions["Export"]->setEnabled(value ? true : false);
                }
            });
        }

    } // namespace ViewLib
} // namespace djv

