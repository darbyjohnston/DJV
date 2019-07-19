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

#include <djvViewApp/FileSettings.h>

#include <djvAV/ImageData.h>

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
        struct FileSettings::Private
        {
            std::shared_ptr<ListSubject<Core::FileSystem::FileInfo> > recentFiles;
            std::shared_ptr<ValueSubject<bool> > autoDetectSequences;
            std::shared_ptr<ValueSubject<bool> > cacheEnabled;
            std::shared_ptr<ValueSubject<int> > cacheMax;
        };

        void FileSettings::_init(Context * context)
        {
            ISettings::_init("djv::ViewApp::FileSettings", context);
            DJV_PRIVATE_PTR();
            p.recentFiles = ListSubject<Core::FileSystem::FileInfo>::create();
            p.autoDetectSequences = ValueSubject<bool>::create(true);
            p.cacheEnabled = ValueSubject<bool>::create(true);
            p.cacheMax = ValueSubject<int>::create(4);
            _load();
        }

        FileSettings::FileSettings() :
            _p(new Private)
        {}

        FileSettings::~FileSettings()
        {}

        std::shared_ptr<FileSettings> FileSettings::create(Context * context)
        {
            auto out = std::shared_ptr<FileSettings>(new FileSettings);
            out->_init(context);
            return out;
        }

        std::shared_ptr<IListSubject<Core::FileSystem::FileInfo> > FileSettings::observeRecentFiles() const
        {
            return _p->recentFiles;
        }

        void FileSettings::setRecentFiles(const std::vector<Core::FileSystem::FileInfo> & value)
        {
            _p->recentFiles->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<bool> > FileSettings::observeAutoDetectSequences() const
        {
            return _p->autoDetectSequences;
        }

        void FileSettings::setAutoDetectSequences(bool value)
        {
            _p->autoDetectSequences->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<bool> > FileSettings::observeCacheEnabled() const
        {
            return _p->cacheEnabled;
        }

        std::shared_ptr<IValueSubject<int> > FileSettings::observeCacheMax() const
        {
            return _p->cacheMax;
        }

        void FileSettings::setCacheEnabled(bool value)
        {
            _p->cacheEnabled->setIfChanged(value);
        }

        void FileSettings::setCacheMax(int value)
        {
            _p->cacheMax->setIfChanged(value);
        }

        void FileSettings::load(const picojson::value & value)
        {
            if (value.is<picojson::object>())
            {
                DJV_PRIVATE_PTR();
                const auto & object = value.get<picojson::object>();
                UI::Settings::read("RecentFiles", object, p.recentFiles);
                UI::Settings::read("AutoDetectSequences", object, p.autoDetectSequences);
                UI::Settings::read("CacheEnabled", object, p.cacheEnabled);
                UI::Settings::read("CacheMax", object, p.cacheMax);
            }
        }

        picojson::value FileSettings::save()
        {
            DJV_PRIVATE_PTR();
            picojson::value out(picojson::object_type, true);
            auto & object = out.get<picojson::object>();
            UI::Settings::write("RecentFiles", p.recentFiles->get(), object);
            UI::Settings::write("AutoDetectSequences", p.autoDetectSequences->get(), object);
            UI::Settings::write("CacheEnabled", p.cacheEnabled->get(), object);
            UI::Settings::write("CacheMax", p.cacheMax->get(), object);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

