// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/FileSettings.h>

#include <djvImage/ImageData.h>

#include <djvSystem/Context.h>
#include <djvSystem/FileInfoFunc.h>

#include <djvMath/BBoxFunc.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#if defined(GetObject)
#undef GetObject
#endif // GetObject

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct FileSettings::Private
        {
            std::shared_ptr<ValueSubject<size_t> > openMax;
            std::shared_ptr<ListSubject<System::File::Info> > recentFiles;
            std::shared_ptr<ValueSubject<size_t> > recentFilesMax;
            std::map<std::string, bool> recentFilesSettingsBellowsState;
            std::shared_ptr<ValueSubject<bool> > autoDetectSequences;
            std::shared_ptr<ValueSubject<bool> > sequencesFirstFrame;
            std::shared_ptr<ValueSubject<bool> > cacheEnabled;
            std::shared_ptr<ValueSubject<int> > cacheSize;
            std::map<std::string, Math::BBox2f> widgetGeom;
        };

        void FileSettings::_init(const std::shared_ptr<System::Context>& context)
        {
            ISettings::_init("djv::ViewApp::FileSettings", context);
            DJV_PRIVATE_PTR();
            p.openMax = ValueSubject<size_t>::create(16);
            p.recentFiles = ListSubject<System::File::Info>::create();
            p.recentFilesMax = ValueSubject<size_t>::create(10);
            p.autoDetectSequences = ValueSubject<bool>::create(true);
            p.sequencesFirstFrame = ValueSubject<bool>::create(true);
            p.cacheEnabled = ValueSubject<bool>::create(true);
            p.cacheSize = ValueSubject<int>::create(4);
            _load();
        }

        FileSettings::FileSettings() :
            _p(new Private)
        {}

        FileSettings::~FileSettings()
        {}

        std::shared_ptr<FileSettings> FileSettings::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<FileSettings>(new FileSettings);
            out->_init(context);
            return out;
        }

        std::shared_ptr<IValueSubject<size_t> > FileSettings::observeOpenMax() const
        {
            return _p->openMax;
        }

        void FileSettings::setOpenMax(size_t value)
        {
            _p->openMax->setIfChanged(value);
        }
        
        std::shared_ptr<IListSubject<System::File::Info> > FileSettings::observeRecentFiles() const
        {
            return _p->recentFiles;
        }

        std::shared_ptr<IValueSubject<size_t> > FileSettings::observeRecentFilesMax() const
        {
            return _p->recentFilesMax;
        }

        std::map<std::string, bool> FileSettings::getRecentFilesSettingsBellowsState() const
        {
            return _p->recentFilesSettingsBellowsState;
        }

        void FileSettings::setRecentFiles(const std::vector<System::File::Info>& value)
        {
            DJV_PRIVATE_PTR();
            std::vector<System::File::Info> files;
            for (size_t i = 0; i < value.size() && i < p.recentFilesMax->get(); ++i)
            {
                files.push_back(value[i]);
            }
            p.recentFiles->setIfChanged(files);
        }

        void FileSettings::setRecentFilesMax(size_t value)
        {
            DJV_PRIVATE_PTR();
            p.recentFilesMax->setIfChanged(value);
            const auto& files = _p->recentFiles->get();
            std::vector<System::File::Info> filesMax;
            for (size_t i = 0; i < files.size() && i < value; ++i)
            {
                filesMax.push_back(files[i]);
            }
            setRecentFiles(filesMax);
        }

        void FileSettings::setRecentFilesSettingsBellowsState(const std::map<std::string, bool>& value)
        {
            _p->recentFilesSettingsBellowsState = value;
        }

        std::shared_ptr<IValueSubject<bool> > FileSettings::observeAutoDetectSequences() const
        {
            return _p->autoDetectSequences;
        }

        std::shared_ptr<IValueSubject<bool> > FileSettings::observeSequencesFirstFrame() const
        {
            return _p->sequencesFirstFrame;
        }

        void FileSettings::setAutoDetectSequences(bool value)
        {
            _p->autoDetectSequences->setIfChanged(value);
        }

        void FileSettings::setSequencesFirstFrame(bool value)
        {
            _p->sequencesFirstFrame->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<bool> > FileSettings::observeCacheEnabled() const
        {
            return _p->cacheEnabled;
        }

        std::shared_ptr<IValueSubject<int> > FileSettings::observeCacheSize() const
        {
            return _p->cacheSize;
        }

        void FileSettings::setCacheEnabled(bool value)
        {
            _p->cacheEnabled->setIfChanged(value);
        }

        void FileSettings::setCacheSize(int value)
        {
            _p->cacheSize->setIfChanged(value);
        }

        const std::map<std::string, Math::BBox2f>& FileSettings::getWidgetGeom() const
        {
            return _p->widgetGeom;
        }

        void FileSettings::setWidgetGeom(const std::map<std::string, Math::BBox2f>& value)
        {
            _p->widgetGeom = value;
        }

        void FileSettings::load(const rapidjson::Value & value)
        {
            if (value.IsObject())
            {
                DJV_PRIVATE_PTR();
                UI::Settings::read("OpenMax", value, p.openMax);
                std::vector< System::File::Info> recentFiles;
                UI::Settings::read("RecentFiles", value, recentFiles);
                auto i = recentFiles.begin();
                while (i != recentFiles.end())
                {
                    if (!i->doesExist())
                    {
                        i = recentFiles.erase(i);
                    }
                    else
                    {
                        ++i;
                    }
                }
                p.recentFiles->setIfChanged(recentFiles);
                UI::Settings::read("RecentFilesMax", value, p.recentFilesMax);
                UI::Settings::read("RecentFilesSettingsBellows", value, p.recentFilesSettingsBellowsState);
                UI::Settings::read("AutoDetectSequences", value, p.autoDetectSequences);
                UI::Settings::read("SequencesFirstFrame", value, p.sequencesFirstFrame);
                UI::Settings::read("CacheEnabled", value, p.cacheEnabled);
                UI::Settings::read("CacheSize", value, p.cacheSize);
                UI::Settings::read("WidgetGeom", value, p.widgetGeom);
            }
        }

        rapidjson::Value FileSettings::save(rapidjson::Document::AllocatorType& allocator)
        {
            DJV_PRIVATE_PTR();
            rapidjson::Value out(rapidjson::kObjectType);
            UI::Settings::write("OpenMax", p.openMax->get(), out, allocator);
            UI::Settings::write("RecentFiles", p.recentFiles->get(), out, allocator);
            UI::Settings::write("RecentFilesMax", p.recentFilesMax->get(), out, allocator);
            UI::Settings::write("RecentFilesSettingsBellows", p.recentFilesSettingsBellowsState, out, allocator);
            UI::Settings::write("AutoDetectSequences", p.autoDetectSequences->get(), out, allocator);
            UI::Settings::write("SequencesFirstFrame", p.sequencesFirstFrame->get(), out, allocator);
            UI::Settings::write("CacheEnabled", p.cacheEnabled->get(), out, allocator);
            UI::Settings::write("CacheSize", p.cacheSize->get(), out, allocator);
            UI::Settings::write("WidgetGeom", p.widgetGeom, out, allocator);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

