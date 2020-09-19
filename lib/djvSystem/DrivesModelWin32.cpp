// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystem/DrivesModel.h>

#include <djvSystem/Path.h>

#include <djvCore/String.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <windows.h>

#include <codecvt>
#include <locale>

//#pragma optimize("", off)

namespace djv
{
    namespace System
    {
        namespace File
        {
            std::vector<Path> DrivesModel::_getDrives()
            {
                std::vector<Path> out;
                if (DWORD result = GetLogicalDriveStringsW(0, NULL))
                {
                    WCHAR * buf = new WCHAR[result];
                    result = GetLogicalDriveStringsW(result, buf);
                    if (result)
                    {
                        try
                        {
                            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16;
                            for (WCHAR* p = buf, *end = buf + result; p < end && *p; ++p)
                            {
                                WCHAR* p2 = p;
                                for (; p2 < end && *p2 && *p2 != '\\'; ++p2)
                                    ;
                                out.push_back(Path(utf16.to_bytes(std::wstring(p, p2 - p))));
                                if ('\\' == *p2)
                                {
                                    p2++;
                                }
                                p = p2;
                            }
                        }
                        catch (const std::exception&)
                        {
                            //! \bug How should we handle this error?
                        }
                    }
                    delete [] buf;
                }
                return out;
            }

        } // namespace File
    } // namespace System
} // namespace djv

