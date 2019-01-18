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
#include <djvViewLib/FileSystemSettings.h>
#include <djvViewLib/Media.h>

#include <djvUI/Action.h>
#include <djvUI/DialogSystem.h>
#include <djvUI/FileBrowser.h>
#include <djvUI/FileBrowserItemView.h>
#include <djvUI/IWindowSystem.h>
#include <djvUI/Label.h>
#include <djvUI/Menu.h>
#include <djvUI/Overlay.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/ToolButton.h>
#include <djvUI/Window.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>
#include <djvCore/RecentFilesModel.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        namespace
        {
            class RecentFilesWidget : public UI::Layout::Vertical
            {
                DJV_NON_COPYABLE(RecentFilesWidget);

            protected:
                void _init(Context * context)
                {
                    Vertical::_init(context);

                    setSpacing(UI::Style::MetricsRole::None);
                    setBackgroundRole(UI::Style::ColorRole::Background);
                    setPointerEnabled(true);

                    _titleLabel = UI::Label::create(context);
                    _titleLabel->setFontSizeRole(UI::Style::MetricsRole::FontHeader);
                    _titleLabel->setTextHAlign(UI::TextHAlign::Left);
                    _titleLabel->setTextColorRole(UI::Style::ColorRole::ForegroundHeader);
                    _titleLabel->setMargin(UI::Layout::Margin(
                        UI::Style::MetricsRole::Margin,
                        UI::Style::MetricsRole::None,
                        UI::Style::MetricsRole::MarginSmall,
                        UI::Style::MetricsRole::MarginSmall));

                    _closeButton = UI::Button::Tool::create(context);
                    _closeButton->setIcon("djvIconClose");
                    _closeButton->setForegroundColorRole(UI::Style::ColorRole::ForegroundHeader);
                    _closeButton->setCheckedForegroundColorRole(UI::Style::ColorRole::ForegroundHeader);
                    _closeButton->setInsideMargin(UI::Style::MetricsRole::MarginSmall);

                    _itemView = UI::FileBrowser::ItemView::create(context);
                    auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
                    scrollWidget->addWidget(_itemView);

                    auto hLayout = UI::Layout::Horizontal::create(context);
                    hLayout->setSpacing(UI::Style::MetricsRole::None);
                    hLayout->setBackgroundRole(UI::Style::ColorRole::BackgroundHeader);
                    hLayout->addWidget(_titleLabel, UI::Layout::RowStretch::Expand);
                    hLayout->addWidget(_closeButton);
                    addWidget(hLayout);
                    addWidget(scrollWidget, UI::Layout::RowStretch::Expand);
                }

                RecentFilesWidget()
                {}

            public:
                static std::shared_ptr<RecentFilesWidget> create(Context * context)
                {
                    auto out = std::shared_ptr<RecentFilesWidget>(new RecentFilesWidget);
                    out->_init(context);
                    return out;
                }

                void setRecentFiles(const std::vector<Core::FileSystem::FileInfo> & value)
                {
                    _itemView->setItems(value);
                }

                void setCallback(const std::function<void(const Core::FileSystem::FileInfo &)> & value)
                {
                    _itemView->setCallback(value);
                }

                void setCloseCallback(const std::function<void(void)> & value)
                {
                    _closeButton->setClickedCallback(value);
                }

            protected:
                void _buttonPressEvent(Event::ButtonPress& event) override
                {
                    event.accept();
                }

                void _buttonReleaseEvent(Event::ButtonRelease& event) override
                {
                    event.accept();
                }

                void _localeEvent(Event::Locale &) override
                {
                    _titleLabel->setText(_getText(DJV_TEXT("djv::ViewLib", "Recent  Files")));
                }

            private:
                std::shared_ptr<UI::Label> _titleLabel;
                std::shared_ptr<UI::Button::Tool> _closeButton;
                std::shared_ptr<UI::FileBrowser::ItemView> _itemView;
            };

        } // namespace

        struct FileSystem::Private
        {
            std::shared_ptr<ValueSubject<std::pair<std::shared_ptr<Media>, glm::vec2> > > opened;
            std::shared_ptr<ValueSubject<bool> > close;
            std::shared_ptr<Core::FileSystem::RecentFilesModel> recentFilesModel;
            std::shared_ptr<RecentFilesWidget> recentFilesWidget;
            std::shared_ptr<UI::Layout::Overlay> overlay;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::map<std::string, std::shared_ptr<UI::Menu> > menus;
            std::shared_ptr<FileSystemSettings> settings;
            std::shared_ptr<ListObserver<Core::FileSystem::FileInfo> > recentFilesObserver;
            std::shared_ptr<ListObserver<Core::FileSystem::FileInfo> > recentFilesObserver2;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
        };

        void FileSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewLib::FileSystem", context);

            DJV_PRIVATE_PTR();
            p.opened = ValueSubject<std::pair<std::shared_ptr<Media>, glm::vec2> >::create();
            p.close = ValueSubject<bool>::create();
            p.recentFilesModel = Core::FileSystem::RecentFilesModel::create(context);

            p.recentFilesWidget = RecentFilesWidget::create(context);
            p.overlay = UI::Layout::Overlay::create(context);
            p.overlay->setBackgroundRole(UI::Style::ColorRole::Overlay);
            p.overlay->setMargin(UI::Style::MetricsRole::MarginDialog);
            p.overlay->addWidget(p.recentFilesWidget);

            p.actions["Open"] = UI::Action::create();
            p.actions["Open"]->setShortcut(GLFW_KEY_O, GLFW_MOD_CONTROL);

            p.actions["Recent"] = UI::Action::create();
            p.actions["Recent"]->setShortcut(GLFW_KEY_T, GLFW_MOD_CONTROL);

            //! \todo Implement me!
            p.actions["Reload"] = UI::Action::create();
            p.actions["Reload"]->setShortcut(GLFW_KEY_R, GLFW_MOD_CONTROL);
            p.actions["Reload"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["ReloadFrame"] = UI::Action::create();
            p.actions["ReloadFrame"]->setShortcut(GLFW_KEY_R, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT);
            p.actions["ReloadFrame"]->setEnabled(false);

            p.actions["Close"] = UI::Action::create();
            p.actions["Close"]->setShortcut(GLFW_KEY_E, GLFW_MOD_CONTROL);

            //! \todo Implement me!
            p.actions["Export"] = UI::Action::create();
            p.actions["Export"]->setShortcut(GLFW_KEY_X, GLFW_MOD_CONTROL);
            p.actions["Export"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["Layers"] = UI::Action::create();
            p.actions["Layers"]->setShortcut(GLFW_KEY_L, GLFW_MOD_CONTROL);
            p.actions["Layers"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["NextLayer"] = UI::Action::create();
            p.actions["NextLayer"]->setShortcut(GLFW_KEY_EQUAL, GLFW_MOD_CONTROL);
            p.actions["NextLayer"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["PrevLayer"] = UI::Action::create();
            p.actions["PrevLayer"]->setShortcut(GLFW_KEY_MINUS, GLFW_MOD_CONTROL);
            p.actions["PrevLayer"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["8BitConversion"] = UI::Action::create();
            p.actions["8BitConversion"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["8BitConversion"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["MemoryCache"] = UI::Action::create();
            p.actions["MemoryCache"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["MemoryCache"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["ClearCache"] = UI::Action::create();
            p.actions["ClearCache"]->setEnabled(false);

            p.actions["Exit"] = UI::Action::create();
            p.actions["Exit"]->setShortcut(GLFW_KEY_Q, GLFW_MOD_CONTROL);

            p.settings = FileSystemSettings::create(context);

            auto weak = std::weak_ptr<FileSystem>(std::dynamic_pointer_cast<FileSystem>(shared_from_this()));
            p.recentFilesObserver = ListObserver<Core::FileSystem::FileInfo>::create(
                p.settings->observeRecentFiles(),
                [weak](const std::vector<Core::FileSystem::FileInfo> & value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->recentFilesModel->setFiles(value);
                }
            });
            p.recentFilesObserver2 = ListObserver<Core::FileSystem::FileInfo>::create(
                p.recentFilesModel->getFiles(),
                [weak](const std::vector<Core::FileSystem::FileInfo> & value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->recentFilesWidget->setRecentFiles(value);
                    system->_p->settings->setRecentFiles(value);
                }
            });
            p.clickedObservers["Open"] = ValueObserver<bool>::create(
                p.actions["Open"]->observeClicked(),
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
                                    system->open(value.getPath());
                                }
                            });
                        }
                    }
                }
            });
            p.clickedObservers["Recent"] = ValueObserver<bool>::create(
                p.actions["Recent"]->observeClicked(),
                [weak, context](bool value)
            {
                if (value)
                {
                    if (auto system = weak.lock())
                    {
                        system->showRecentFiles();
                    }
                }
            });
            p.clickedObservers["Close"] = ValueObserver<bool>::create(
                p.actions["Close"]->observeClicked(),
                [weak, context](bool value)
            {
                if (value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->close->setAlways(true);
                    }
                }
            });
            p.clickedObservers["Exit"] = ValueObserver<bool>::create(
                p.actions["Exit"]->observeClicked(),
                [weak, context](bool value)
            {
                if (value)
                {
                    if (auto system = weak.lock())
                    {
                        if (auto dialogSystem = context->getSystemT<UI::DialogSystem>().lock())
                        {
                            dialogSystem->confirmation(
                                context->getText(DJV_TEXT("djv::ViewLib", "Exit")),
                                context->getText(DJV_TEXT("djv::ViewLib", "Are you sure you want to exit?")),
                                context->getText(DJV_TEXT("djv::ViewLib", "Yes")),
                                context->getText(DJV_TEXT("djv::ViewLib", "No")),
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

        std::shared_ptr<IValueSubject<std::pair<std::shared_ptr<Media>, glm::vec2> > > FileSystem::observeOpened() const
        {
            return _p->opened;
        }

        std::shared_ptr<IValueSubject<bool> > FileSystem::observeClose() const
        {
            return _p->close;
        }

        void FileSystem::open(const std::string & value, const glm::vec2 & pos)
        {
            auto media = Media::create(value, getContext());
            _p->opened->setAlways(std::make_pair(media, pos));
            _p->recentFilesModel->addFile(value);
        }

        void FileSystem::showRecentFiles()
        {
            DJV_PRIVATE_PTR();
            auto context = getContext();
            if (auto windowSystem = context->getSystemT<UI::IWindowSystem>().lock())
            {
                if (auto window = windowSystem->observeCurrentWindow()->get())
                {
                    window->addWidget(p.overlay);
                    p.overlay->show();

                    auto weak = std::weak_ptr<FileSystem>(std::dynamic_pointer_cast<FileSystem>(shared_from_this()));
                    p.recentFilesWidget->setCallback(
                        [weak, window](const Core::FileSystem::FileInfo & value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->open(value);
                            window->removeWidget(system->_p->overlay);
                        }
                    });
                    p.recentFilesWidget->setCloseCallback(
                        [weak, window]
                    {
                        if (auto system = weak.lock())
                        {
                            window->removeWidget(system->_p->overlay);
                        }
                    });
                    p.overlay->setCloseCallback(
                        [weak, window]
                    {
                        if (auto system = weak.lock())
                        {
                            window->removeWidget(system->_p->overlay);
                        }
                    });
                }
            }
        }

        std::map<std::string, std::shared_ptr<UI::Action> > FileSystem::getActions()
        {
            return _p->actions;
        }
        
        NewMenu FileSystem::createMenu()
        {
            DJV_PRIVATE_PTR();
            auto context = getContext();
            p.menus["File"] = UI::Menu::create(_getText(DJV_TEXT("djv::ViewLib", "File")), context);
            p.menus["File"]->addAction(p.actions["Open"]);
            p.menus["File"]->addAction(p.actions["Recent"]);
            p.menus["File"]->addAction(p.actions["Reload"]);
            p.menus["File"]->addAction(p.actions["ReloadFrame"]);
            p.menus["File"]->addAction(p.actions["Close"]);
            p.menus["File"]->addAction(p.actions["Export"]);
            p.menus["File"]->addSeparator();
            p.menus["File"]->addAction(p.actions["Layers"]);
            p.menus["File"]->addAction(p.actions["NextLayer"]);
            p.menus["File"]->addAction(p.actions["PrevLayer"]);
            //! \todo Implement me!
            p.menus["ProxyScale"] = UI::Menu::create(_getText(DJV_TEXT("djv::ViewLib", "Proxy Scale")), context);
            p.menus["File"]->addMenu(p.menus["ProxyScale"]);
            p.menus["File"]->addSeparator();
            p.menus["File"]->addAction(p.actions["Exit"]);
            return { p.menus["File"], "A" };
        }

        void FileSystem::setCurrentMedia(const std::shared_ptr<Media> & media)
        {
            DJV_PRIVATE_PTR();
            p.actions["Close"]->setEnabled(media ? true : false);
            p.actions["Export"]->setEnabled(media ? true : false);
        }

        void FileSystem::_localeEvent(Event::Locale &)
        {
            DJV_PRIVATE_PTR();
            p.actions["Open"]->setText(_getText(DJV_TEXT("djv::ViewLib", "Open")));
            p.actions["Recent"]->setText(_getText(DJV_TEXT("djv::ViewLib", "Recent")));
            p.actions["Reload"]->setText(_getText(DJV_TEXT("djv::ViewLib", "Reload")));
            p.actions["ReloadFrame"]->setText(_getText(DJV_TEXT("djv::ViewLib", "Reload Frame")));
            p.actions["Close"]->setText(_getText(DJV_TEXT("djv::ViewLib", "Close")));
            p.actions["Export"]->setText(_getText(DJV_TEXT("djv::ViewLib", "Export")));
            p.actions["Layers"]->setText(_getText(DJV_TEXT("djv::ViewLib", "Layers")));
            p.actions["NextLayer"]->setText(_getText(DJV_TEXT("djv::ViewLib", "Next Layer")));
            p.actions["PrevLayer"]->setText(_getText(DJV_TEXT("djv::ViewLib", "Previous Layer")));
            p.actions["8BitConversion"]->setText(_getText(DJV_TEXT("djv::ViewLib", "8-bit Conversion")));
            p.actions["MemoryCache"]->setText(_getText(DJV_TEXT("djv::ViewLib", "Memory Cache")));
            p.actions["ClearCache"]->setText(_getText(DJV_TEXT("djv::ViewLib", "Clear Cache")));
            p.actions["Exit"]->setText(_getText(DJV_TEXT("djv::ViewLib", "Exit")));

            p.menus["File"]->setMenuName(_getText(DJV_TEXT("djv::ViewLib", "File")));
            p.menus["ProxyScale"]->setMenuName(_getText(DJV_TEXT("djv::ViewLib", "Proxy Scale")));
        }

    } // namespace ViewLib
} // namespace djv

