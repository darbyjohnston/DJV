// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystem/File.h>

#include <codecvt>
#include <locale>

namespace djv
{
    namespace System
    {
        namespace File
        {
            FILE* fopen(const std::string& fileName, const std::string& mode)
            {
                FILE* out = nullptr;
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16;
                _wfopen_s(&out, utf16.from_bytes(fileName).c_str(), utf16.from_bytes(mode).c_str());
                return out;
            }

        } // namespace File
    } // namespace System
} // namespace djv

