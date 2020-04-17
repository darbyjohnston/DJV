// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/FileIO.h>

#include <codecvt>
#include <locale>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            FILE* fopen(const std::string & fileName, const std::string & mode)
            {
                FILE* out = nullptr;
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16;
                _wfopen_s(&out, utf16.from_bytes(fileName).c_str(), utf16.from_bytes(mode).c_str());
                return out;
            }

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

