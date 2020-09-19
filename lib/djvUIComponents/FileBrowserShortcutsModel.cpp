// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/FileBrowserPrivate.h>

#include <djvSystem/Path.h>
#include <djvSystem/PathFunc.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace FileBrowser
        {
            struct ShortcutsModel::Private
            {
                std::shared_ptr<ListSubject<System::File::Path> > shortcuts;
            };

            void ShortcutsModel::_init(const std::shared_ptr<System::Context>& context)
            {
                DJV_PRIVATE_PTR();

                std::vector<System::File::Path> shortcuts;
                for (size_t i = 0; i < static_cast<size_t>(System::File::DirectoryShortcut::Count); ++i)
                {
                    const auto shortcut = System::File::getPath(static_cast<System::File::DirectoryShortcut>(i));
                    shortcuts.push_back(shortcut);
                }

                p.shortcuts = ListSubject<System::File::Path>::create(shortcuts);
            }

            ShortcutsModel::ShortcutsModel() :
                _p(new Private)
            {}

            ShortcutsModel::~ShortcutsModel()
            {}

            std::shared_ptr<ShortcutsModel> ShortcutsModel::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<ShortcutsModel>(new ShortcutsModel);
                out->_init(context);
                return out;
            }

            std::shared_ptr<IListSubject<System::File::Path> > ShortcutsModel::observeShortcuts() const
            {
                return _p->shortcuts;
            }

            void ShortcutsModel::setShortcuts(const std::vector<System::File::Path>& value)
            {
                _p->shortcuts->setIfChanged(value);
            }

            void ShortcutsModel::addShortcut(const System::File::Path& value)
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
