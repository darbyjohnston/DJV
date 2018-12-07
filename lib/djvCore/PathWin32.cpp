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

#include <djvCore/Path.h>

#include <djvCore/FileInfo.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <direct.h>

namespace djv
{
    namespace Core
    {
        char Path::getCurrentPathSeparator()
        {
            return getPathSeparator(PathSeparator::Windows);
        }
        
        void Path::mkdir(const Path& value)
        {
            if (_mkdir(value.get().c_str()) != 0)
            {
                std::stringstream s;
                s << "Cannot create directory: " << value;
                throw std::invalid_argument(s.str());
            }
        }

        Path Path::getAbsolute(const Path& value)
        {
            char buf[MAX_PATH];
            if (!::_fullpath(buf, value._value.c_str(), MAX_PATH))
            {
                buf[0] = 0;
            }
            return Path(buf);
        }

        Path Path::getCWD()
        {
            char buf[MAX_PATH];
            if (!::_getcwd(buf, MAX_PATH))
            {
                buf[0] = 0;
            }
            return Path(buf);
        }

        Path Path::getTemp()
        {
            Path out;
            TCHAR buf[MAX_PATH];
            DWORD r = GetTempPath(MAX_PATH, buf);
            if (r && r < MAX_PATH)
            {
                out.set(buf);
            }
            return out;
        }

    } // namespace Core
} // namespace djv

