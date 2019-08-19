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

#include <djvUIComponents/FileBrowserPrivate.h>

#include <djvCore/Path.h>
#include <djvCore/OS.h>

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

            void ShortcutsModel::setShortcuts(const std::vector<Core::FileSystem::Path> & value)
            {
                _p->shortcuts->setIfChanged(value);
            }

            void ShortcutsModel::addShortcut(const Core::FileSystem::Path & value)
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
