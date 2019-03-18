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

#pragma once

#include <djvUI/Enum.h>
#include <djvUI/ISettings.h>

#include <djvCore/FileInfo.h>
#include <djvCore/MapObserver.h>
#include <djvCore/ListObserver.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            //! This class provides the file browser settings.
            class FileBrowser : public ISettings
            {
                DJV_NON_COPYABLE(FileBrowser);

            protected:
                void _init(Core::Context * context);

                FileBrowser();

            public:
                virtual ~FileBrowser();

                static std::shared_ptr<FileBrowser> create(Core::Context *);

                std::shared_ptr<Core::IMapSubject<std::string, bool> > observeShortcutsBellows() const;
                void setShortcutsBellows(const std::map<std::string, bool> &);

                std::shared_ptr<Core::IListSubject<float> > observeShortcutsSplit() const;
                void setShortcutsSplit(const std::vector<float> &);

                std::shared_ptr<Core::IValueSubject<ViewType> > observeViewType() const;
                void setViewType(ViewType);

                std::shared_ptr<Core::IListSubject<float> > observeListViewHeaderSplit() const;
                void setListViewHeaderSplit(const std::vector<float> &);

                std::shared_ptr<Core::IValueSubject<bool> > observeFileSequences() const;
                void setFileSequences(bool);

                std::shared_ptr<Core::IValueSubject<bool> > observeShowHidden() const;
                void setShowHidden(bool);

                std::shared_ptr<Core::IValueSubject<Core::FileSystem::DirectoryListSort> > observeSort() const;
                void setSort(Core::FileSystem::DirectoryListSort);

                std::shared_ptr<Core::IValueSubject<bool> > observeReverseSort() const;
                void setReverseSort(bool);

                std::shared_ptr<Core::IValueSubject<bool> > observeSortDirectoriesFirst() const;
                void setSortDirectoriesFirst(bool);

                void load(const picojson::value&) override;
                picojson::value save() override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UI
} // namespace djv
