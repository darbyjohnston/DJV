//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvCoreTest/DirectoryModelTest.h>

#include <djvCore/DirectoryModel.h>
#include <djvCore/FileIO.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        DirectoryModelTest::DirectoryModelTest(const std::shared_ptr<Core::Context>& context) :
            ITickTest("djv::CoreTest::DirectoryModelTest", context)
        {}
        
        void DirectoryModelTest::run(const std::vector<std::string>& args)
        {
            if (auto context = getContext().lock())
            {
                auto model = FileSystem::DirectoryModel::create(context);
                model->setPath(FileSystem::Path("."));
                auto path = model->observePath()->get();
                {
                    std::stringstream ss;
                    ss << "path: " << path;
                    _print(ss.str());
                }
                model->reload();
                for (const auto& i : model->observeFileInfo()->get())
                {
                    std::stringstream ss;
                    ss << "file info: " << i;
                    _print(ss.str());
                }
                for (const auto& i : model->observeFileNames()->get())
                {
                    std::stringstream ss;
                    ss << "file name: " << i;
                    _print(ss.str());
                }

                model->cdUp();
                auto path2 = model->observePath()->get();
                {
                    std::stringstream ss;
                    ss << "path 2: " << path2;
                    _print(ss.str());
                }
                DJV_ASSERT(model->observeHasUp()->get());

                model->setHistoryMax(100);
                model->setHistoryMax(100);
                model->setHistoryMax(10);
                model->setHistoryIndex(0);
                DJV_ASSERT(path == model->observePath()->get());
                model->goForward();
                DJV_ASSERT(path2 == model->observePath()->get());
                model->goBack();
                DJV_ASSERT(path == model->observePath()->get());
                for (const auto& i : model->observeHistory()->get())
                {
                    std::stringstream ss;
                    ss << "history: " << i;
                    _print(ss.str());
                }
                DJV_ASSERT(0 == model->observeHistoryIndex()->get());
                DJV_ASSERT(!model->observeHasBack()->get());
                DJV_ASSERT(model->observeHasForward()->get());
                model->setPath(path2);
                model->setPath(path);
                model->setHistoryMax(0);

                model->setFileExtensions({ ".txt" });
                model->setFileExtensions({ ".txt" });

                model->setFileSequences(true);
                model->setFileSequenceExtensions({ ".txt" });
                model->setFileSequenceExtensions({ ".txt" });
                model->setShowHidden(true);
                DJV_ASSERT(model->observeFileSequences()->get());
                DJV_ASSERT(model->observeShowHidden()->get());

                model->setSort(FileSystem::DirectoryListSort::Size);
                model->setReverseSort(true);
                model->setReverseSort(true);
                model->setSortDirectoriesFirst(false);
                model->setSortDirectoriesFirst(false);
                DJV_ASSERT(FileSystem::DirectoryListSort::Size == model->observeSort()->get());
                DJV_ASSERT(model->observeReverseSort()->get());
                DJV_ASSERT(!model->observeSortDirectoriesFirst()->get());

                DJV_ASSERT(model->observeFilter()->get().empty());
                model->setFilter("txt");
                model->setFilter("txt");
                DJV_ASSERT("txt" == model->observeFilter()->get());
                model->clearFilter();
                DJV_ASSERT(model->observeFilter()->get().empty());
                
                _tickFor(std::chrono::milliseconds(1000));

                FileSystem::FileIO io;
                io.open("DirectoryModelTest", FileSystem::FileIO::Mode::Write);
                io.close();                

                _tickFor(std::chrono::milliseconds(1000));
            }
        }
        
    } // namespace CoreTest
} // namespace djv

