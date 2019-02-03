//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvCore/DrivesModel.h>

#include <djvCore/Path.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <codecvt>
#include <locale>

//#pragma optimize("", off)

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            std::vector<Path> DrivesModel::_getDrives()
            {
                std::vector<Path> out;
                DWORD result = GetLogicalDriveStringsW(0, NULL);
                if (result)
                {
                    WCHAR * buf = new WCHAR[result];
                    result = GetLogicalDriveStringsW(result, buf);
                    if (result)
                    {
                        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16;
                        for (WCHAR * p = buf, *end = buf + result; p < end && *p; ++p)
                        {
                            WCHAR* p2 = p;
                            for (; p2 < end && *p2 && *p2 != '\\'; ++p2)
                                ;
                            out.push_back(utf16.to_bytes(std::wstring(p, p2 - p)));
                            if ('\\' == *p2)
                            {
                                p2++;
                            }
                            p = p2;
                        }
                    }
                    delete [] buf;
                }
                return out;
            }

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

