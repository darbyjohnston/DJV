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

#include <djvCore/DrivesModel.h>

#include <djvCore/Path.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <lm.h>

#include <codecvt>
#include <locale>

//#pragma optimize("", off)

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            namespace
            {
                bool EnumerateFunc(LPNETRESOURCE lpnr, std::vector<std::string> & out)
                {
                    HANDLE hEnum;
                    DWORD dwResult = WNetOpenEnum(RESOURCE_GLOBALNET, RESOURCETYPE_ANY, 0, lpnr, &hEnum);
                    if (dwResult != NO_ERROR)
                    {
                        return false;
                    }

                    DWORD cbBuffer = 16384;
                    const LPNETRESOURCE lpnrLocal = (LPNETRESOURCE)GlobalAlloc(GPTR, cbBuffer);
                    if (NULL == lpnrLocal)
                    {
                        return false;
                    }

                    DWORD dwResultEnum = 0;
                    do
                    {
                        ZeroMemory(lpnrLocal, cbBuffer);
                        DWORD cEntries = -1;
                        dwResultEnum = WNetEnumResource(hEnum, &cEntries, lpnrLocal, &cbBuffer);
                        if (dwResultEnum == NO_ERROR)
                        {
                            for (DWORD i = 0; i < cEntries; i++)
                            {
                                if (RESOURCEUSAGE_CONTAINER == (lpnrLocal[i].dwUsage & RESOURCEUSAGE_CONTAINER))
                                {
                                    switch (lpnrLocal[i].dwDisplayType)
                                    {
                                    case RESOURCEDISPLAYTYPE_SERVER:
                                        if (lpnrLocal[i].lpRemoteName)
                                        {
                                            out.push_back(lpnrLocal[i].lpRemoteName);
                                        }
                                        break;
                                    }
                                    EnumerateFunc(&lpnrLocal[i], out);
                                }
                            }
                        }
                    }
                    while (dwResultEnum != ERROR_NO_MORE_ITEMS);
                    GlobalFree((HGLOBAL)lpnrLocal);
                    dwResult = WNetCloseEnum(hEnum);
                    if (dwResult != NO_ERROR)
                    {
                        return false;
                    }

                    return true;
                }

            } // namespace

            std::vector<Path> DrivesModel::_getDrives()
            {
                std::vector<Path> out;

                if (DWORD result = GetLogicalDriveStringsW(0, NULL))
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

                //! \todo Windows network shares need more work/testing before being enabled.
                /*LPNETRESOURCE lpnr = NULL;
                std::vector<std::string> servers;
                EnumerateFunc(lpnr, servers);
                for (const auto & i : servers)
                {
                    PSHARE_INFO_502 BufPtr;
                    DWORD res    = 0;
                    DWORD er     = 0;
                    DWORD tr     = 0;
                    DWORD resume = 0;
                    do
                    {
                        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16;
                        res = NetShareEnum(&utf16.from_bytes(i)[0], 502, (LPBYTE *)&BufPtr, MAX_PREFERRED_LENGTH, &er, &tr, &resume);
                        if (res == ERROR_SUCCESS || res == ERROR_MORE_DATA)
                        {
                            PSHARE_INFO_502 p = BufPtr;
                            for (DWORD j = 0; j < er; ++j, ++p)
                            {
                                if (STYPE_DISKTREE == (p->shi502_type & STYPE_MASK) &&
                                    p->shi502_security_descriptor && IsValidSecurityDescriptor(p->shi502_security_descriptor))
                                {
                                    out.push_back(Path(i, utf16.to_bytes(p->shi502_netname)));
                                }
                            }
                            NetApiBufferFree(BufPtr);
                        }
                    } while (res == ERROR_MORE_DATA);
                }*/

                return out;
            }

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

