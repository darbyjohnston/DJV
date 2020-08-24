// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/FileBrowserPrivate.h>

#include <djvUIComponents/FileBrowserSettings.h>

#include <djvUI/Bellows.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>

#include <djvCore/Context.h>
#include <djvCore/DirectoryModel.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace FileBrowser
        {
            struct DrawerWidget::Private
            {
                std::shared_ptr<ShortcutsWidget> shortcutsWidget;
                std::shared_ptr<RecentPathsWidget> recentPathsWidget;
                std::shared_ptr<DrivesWidget> drivesWidget;
                std::map<std::string, std::shared_ptr<Bellows> > bellows;
                std::shared_ptr<VerticalLayout> layout;

                std::shared_ptr<ValueObserver<FileSystem::Path> > pathObserver;
                std::shared_ptr<MapObserver<std::string, bool> > pathsBellowsStateObserver;
            };

            void DrawerWidget::_init(
                const std::shared_ptr<FileSystem::DirectoryModel>& directoryModel,
                const std::shared_ptr<ShortcutsModel>& shortcutsModel,
                const std::shared_ptr<FileSystem::RecentFilesModel>& recentFilesModel,
                const std::shared_ptr<FileSystem::DrivesModel>& drivesModel,
                const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UI::FileBrowser::DrawerWidget");

                p.shortcutsWidget = ShortcutsWidget::create(shortcutsModel, context);
                p.bellows["Shortcuts"] = Bellows::create(context);
                p.bellows["Shortcuts"]->addChild(p.shortcutsWidget);

                auto recentPathsWidget = RecentPathsWidget::create(recentFilesModel, context);
                p.bellows["Recent"] = Bellows::create(context);
                p.bellows["Recent"]->addChild(recentPathsWidget);

                auto drivesWidget = DrivesWidget::create(drivesModel, context);
                p.bellows["Drives"] = Bellows::create(context);
                p.bellows["Drives"]->addChild(drivesWidget);

                p.layout = VerticalLayout::create(context);
                p.layout->setSpacing(MetricsRole::None);
                p.layout->addChild(p.bellows["Shortcuts"]);
                p.layout->addChild(p.bellows["Recent"]);
                p.layout->addChild(p.bellows["Drives"]);
                addChild(p.layout);

                auto weak = std::weak_ptr<DrawerWidget>(std::dynamic_pointer_cast<DrawerWidget>(shared_from_this()));
                p.shortcutsWidget->setCallback(
                    [directoryModel](const FileSystem::Path& value)
                    {
                        directoryModel->setPath(value);
                    });

                recentPathsWidget->setCallback(
                    [directoryModel](const FileSystem::Path& value)
                    {
                        directoryModel->setPath(value);
                    });

                drivesWidget->setCallback(
                    [directoryModel](const FileSystem::Path& value)
                    {
                        directoryModel->setPath(value);
                    });

                p.pathObserver = ValueObserver<FileSystem::Path>::create(
                    directoryModel->observePath(),
                    [weak](const FileSystem::Path& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->shortcutsWidget->setPath(value);
                        }
                    });

                auto settingsSystem = context->getSystemT<Settings::System>();
                auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>();
                p.pathsBellowsStateObserver = MapObserver<std::string, bool>::create(
                    fileBrowserSettings->observePathsBellowsState(),
                    [weak](const std::map<std::string, bool>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            for (const auto& i : value)
                            {
                                const auto j = widget->_p->bellows.find(i.first);
                                if (j != widget->_p->bellows.end())
                                {
                                    j->second->setOpen(i.second, false);
                                }
                            }
                        }
                    });

                for (const auto& i : p.bellows)
                {
                    i.second->setOpenCallback(
                        [weak](bool value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_saveBellowsState();
                            }
                        });
                }
            }

            DrawerWidget::DrawerWidget() :
                _p(new Private)
            {}

            DrawerWidget::~DrawerWidget()
            {}

            std::shared_ptr<DrawerWidget> DrawerWidget::create(
                const std::shared_ptr<FileSystem::DirectoryModel>& directoryModel,
                const std::shared_ptr<ShortcutsModel>& shortcutsModel,
                const std::shared_ptr<FileSystem::RecentFilesModel>& recentFilesModel,
                const std::shared_ptr<FileSystem::DrivesModel>& drivesModel,
                const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<DrawerWidget>(new DrawerWidget);
                out->_init(directoryModel, shortcutsModel, recentFilesModel, drivesModel, context);
                return out;
            }

            void DrawerWidget::_preLayoutEvent(Event::PreLayout& event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void DrawerWidget::_layoutEvent(Event::Layout& event)
            {
                _p->layout->setGeometry(getGeometry());
            }

            void DrawerWidget::_initEvent(Event::Init& event)
            {
                DJV_PRIVATE_PTR();
                if (event.getData().text)
                {
                    p.bellows["Shortcuts"]->setText(_getText(DJV_TEXT("file_browser_shortcuts")));
                    p.bellows["Recent"]->setText(_getText(DJV_TEXT("file_browser_recent_paths")));
                    p.bellows["Drives"]->setText(_getText(DJV_TEXT("file_browser_drives")));
                }
            }

            void DrawerWidget::_saveBellowsState()
            {
                DJV_PRIVATE_PTR();
                if (auto context = getContext().lock())
                {
                    if (!p.bellows.empty())
                    {
                        std::map<std::string, bool> state;
                        for (const auto& i : p.bellows)
                        {
                            state[i.first] = i.second->isOpen();
                        }
                        auto settingsSystem = context->getSystemT<Settings::System>();
                        auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>();
                        fileBrowserSettings->setPathsBellowsState(state);
                    }
                }
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv
