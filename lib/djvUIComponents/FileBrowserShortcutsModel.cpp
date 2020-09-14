// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/FileBrowserPrivate.h>

#include <djvCore/Path.h>
#include <djvCore/OSFunc.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace FileBrowser
        {
            struct ShortcutsModel::Private
            {
                std::shared_ptr<ListSubject<FileSystem::Path> > shortcuts;
            };

            void ShortcutsModel::_init(const std::shared_ptr<Context>& context)
            {
                DJV_PRIVATE_PTR();

                std::vector<FileSystem::Path> shortcuts;
                for (size_t i = 0; i < static_cast<size_t>(OS::DirectoryShortcut::Count); ++i)
                {
                    const auto shortcut = OS::getPath(static_cast<OS::DirectoryShortcut>(i));
                    shortcuts.push_back(shortcut);
                }

                p.shortcuts = ListSubject<FileSystem::Path>::create(shortcuts);
            }

            ShortcutsModel::ShortcutsModel() :
                _p(new Private)
            {}

            ShortcutsModel::~ShortcutsModel()
            {}

            std::shared_ptr<ShortcutsModel> ShortcutsModel::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<ShortcutsModel>(new ShortcutsModel);
                out->_init(context);
                return out;
            }

            std::shared_ptr<IListSubject<FileSystem::Path> > ShortcutsModel::observeShortcuts() const
            {
                return _p->shortcuts;
            }

            void ShortcutsModel::setShortcuts(const std::vector<Core::FileSystem::Path>& value)
            {
                _p->shortcuts->setIfChanged(value);
            }

            void ShortcutsModel::addShortcut(const Core::FileSystem::Path& value)
            {
                auto shortcuts = _p->shortcuts->get();
                const auto i = std::find(shortcuts.begin(), shortcuts.end(), value);
                if (i != shortcuts.end())
                {
                    shortcuts.erase(i);
                }
                shortcuts.push_back(value);
                _p->shortcuts->setIfChanged(shortcuts);
            }

            void ShortcutsModel::removeShortcut(size_t index)
            {
                if (index < _p->shortcuts->getSize())
                {
                    _p->shortcuts->removeItem(index);
                }
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv
