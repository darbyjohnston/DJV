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

#include <djvCore/Path.h>

#include <djvCore/FileInfo.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <direct.h>

#include <codecvt>
#include <locale>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            void Path::mkdir(const Path & value)
            {
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16;
                if (_wmkdir(utf16.from_bytes(value.get()).c_str()) != 0)
                {
                    std::stringstream s;
                    s << DJV_TEXT("The directory") << " '" << value << "' " << DJV_TEXT("cannot be created") << ".";
                    throw std::invalid_argument(s.str());
                }
            }

            Path Path::getAbsolute(const Path & value)
            {
                wchar_t buf[MAX_PATH];
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16;
                if (!::_wfullpath(buf, utf16.from_bytes(value.get()).c_str(), MAX_PATH))
                {
                    buf[0] = 0;
                }
                return Path(utf16.to_bytes(buf));
            }

            Path Path::getCWD()
            {
                wchar_t buf[MAX_PATH];
                if (!::_wgetcwd(buf, MAX_PATH))
                {
                    buf[0] = 0;
                }
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16;
                return Path(utf16.to_bytes(buf));
            }

            Path Path::getTemp()
            {
                Path out;
                WCHAR buf[MAX_PATH];
                DWORD r = GetTempPathW(MAX_PATH, buf);
                if (r && r < MAX_PATH)
                {
                    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16;
                    out.set(utf16.to_bytes(buf));
                }
                return out;
            }

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

