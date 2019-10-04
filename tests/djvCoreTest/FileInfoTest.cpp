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

#include <djvCoreTest/FileInfoTest.h>

#include <djvCore/FileIO.h>
#include <djvCore/FileInfo.h>

namespace djv
{
    using namespace Core;

    namespace CoreTest
    {
        FileInfoTest::FileInfoTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::FileInfoTest", context),
            _fileName("FileInfoTest")
        {}
        
        void FileInfoTest::run(const std::vector<std::string>& args)
        {
            _enum();
            _path();
            _info();
            _sequences();
            _util();
            _operators();
            _serialize();
        }

        void FileInfoTest::_enum()
        {
            for (auto i : FileSystem::getFileTypeEnums())
            {
                std::stringstream ss;
                ss << "File type string: " << i;
                _print(ss.str());
            }
            for (auto i :
                {
                    FileSystem::FilePermissions::Read,
                    FileSystem::FilePermissions::Write,
                    FileSystem::FilePermissions::Exec
                })
            {
                std::stringstream ss;
                ss << "File permissions string: " << FileSystem::getFilePermissionsLabel(static_cast<int>(i));
                _print(ss.str());
            }
            for (auto i : FileSystem::getDirectoryListSortEnums())
            {
                std::stringstream ss;
                ss << "Directory list sort string: " << i;
                _print(ss.str());
            }
        }

        void FileInfoTest::_path()
        {
            {
                FileSystem::FileIO io;
                io.open(_fileName, FileSystem::FileIO::Mode::Write);
                io.close();
                FileSystem::FileInfo fileInfo;
                fileInfo = _fileName;
            }
        }

        void FileInfoTest::_info()
        {

        }

        void FileInfoTest::_sequences()
        {

        }

        void FileInfoTest::_util()
        {

        }

        void FileInfoTest::_operators()
        {

        }

        void FileInfoTest::_serialize()
        {

        }
        
    } // namespace CoreTest
} // namespace djv

