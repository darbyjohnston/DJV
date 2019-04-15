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

#include <djvViewApp/FileSystemSettings.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>

// These need to be included last on OSX.
#include <djvCore/PicoJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct FileSystemSettings::Private
        {
            std::shared_ptr<ListSubject<Core::FileSystem::FileInfo> > recentFiles;
            std::shared_ptr<ValueSubject<glm::ivec2> > recentThumbnailSize;
        };

        void FileSystemSettings::_init(Context * context)
        {
            ISettings::_init("djv::ViewApp::FileSystemSettings", context);

            DJV_PRIVATE_PTR();
            p.recentFiles = ListSubject<Core::FileSystem::FileInfo>::create();
            p.recentThumbnailSize = ValueSubject<glm::ivec2>::create(glm::ivec2(100, 50));
            _load();
        }

        FileSystemSettings::FileSystemSettings() :
            _p(new Private)
        {}

        FileSystemSettings::~FileSystemSettings()
        {}

        std::shared_ptr<FileSystemSettings> FileSystemSettings::create(Context * context)
        {
            auto out = std::shared_ptr<FileSystemSettings>(new FileSystemSettings);
            out->_init(context);
            return out;
        }

        std::shared_ptr<IListSubject<Core::FileSystem::FileInfo> > FileSystemSettings::observeRecentFiles() const
        {
            return _p->recentFiles;
        }

        void FileSystemSettings::setRecentFiles(const std::vector<Core::FileSystem::FileInfo> & value)
        {
            _p->recentFiles->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<glm::ivec2> > FileSystemSettings::observeRecentThumbnailSize() const
        {
            return _p->recentThumbnailSize;
        }

        void FileSystemSettings::setRecentThumbnailSize(const glm::ivec2 & value)
        {
            _p->recentThumbnailSize->setIfChanged(value);
        }

        void FileSystemSettings::load(const picojson::value & value)
        {
            if (value.is<picojson::object>())
            {
                DJV_PRIVATE_PTR();
                const auto & object = value.get<picojson::object>();
                std::vector<std::string> tmp;
                UI::Settings::read("RecentFiles", object, tmp);
                std::vector<Core::FileSystem::FileInfo> fileInfoList;
                for (const auto & i : tmp)
                {
                    fileInfoList.push_back(i);
                }
                p.recentFiles->setIfChanged(fileInfoList);
                UI::Settings::read("RecentThumbnailSize", object, p.recentThumbnailSize);
            }
        }

        picojson::value FileSystemSettings::save()
        {
            DJV_PRIVATE_PTR();
            picojson::value out(picojson::object_type, true);
            auto & object = out.get<picojson::object>();
            std::vector<std::string> tmp;
            for (const auto & i : p.recentFiles->get())
            {
                tmp.push_back(i);
            }
            UI::Settings::write("RecentFiles", tmp, object);
            UI::Settings::write("RecentThumbnailSize", p.recentThumbnailSize->get(), object);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

