// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystem/FileInfoFunc.h>

#include <djvSystem/FileInfoPrivate.h>
#include <djvSystem/PathFunc.h>

#include <djvCore/Memory.h>
#include <djvCore/StringFunc.h>

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

using namespace djv::Core;

namespace djv
{
    namespace System
    {
        namespace File
        {
            namespace
            {
                class NetOpenEnum
                {
                public:
                    NetOpenEnum(LPNETRESOURCE lpnr)
                    {
                        dwResult = WNetOpenEnum(RESOURCE_GLOBALNET, RESOURCETYPE_ANY, 0, lpnr, &hEnum);
                    }

                    ~NetOpenEnum()
                    {
                        if (hEnum)
                        {
                            WNetCloseEnum(hEnum);
                        }
                    }

                    HANDLE hEnum = NULL;
                    DWORD dwResult = NO_ERROR;
                };

                class NetResource
                {
                public:
                    NetResource(DWORD size)
                    {
                        p = (LPNETRESOURCE)GlobalAlloc(GPTR, size);
                    }

                    ~NetResource()
                    {
                        if (p != NULL)
                        {
                            GlobalFree((HGLOBAL)p);
                        }
                    }

                    LPNETRESOURCE p = NULL;
                };

                bool EnumerateFunc(LPNETRESOURCE lpnr, std::vector<std::string>& out)
                {
                    NetOpenEnum netOpenEnum(lpnr);
                    if (netOpenEnum.dwResult != NO_ERROR)
                    {
                        return false;
                    }

                    DWORD cbBuffer = 16384;
                    NetResource netResource(cbBuffer);
                    if (NULL == netResource.p)
                    {
                        return false;
                    }

                    bool r = true;
                    DWORD dwResultEnum = 0;
                    do
                    {
                        ZeroMemory(netResource.p, cbBuffer);
                        DWORD cEntries = -1;
                        dwResultEnum = WNetEnumResource(netOpenEnum.hEnum, &cEntries, netResource.p, &cbBuffer);
                        if (dwResultEnum == NO_ERROR)
                        {
                            for (DWORD i = 0; i < cEntries; i++)
                            {
                                switch (netResource.p[i].dwDisplayType)
                                {
                                case RESOURCEDISPLAYTYPE_SHARE:
                                    if (netResource.p[i].lpRemoteName)
                                    {
                                        out.push_back(netResource.p[i].lpRemoteName);
                                    }
                                    break;
                                default: break;
                                }
                                if (RESOURCEUSAGE_CONTAINER == (netResource.p[i].dwUsage & RESOURCEUSAGE_CONTAINER))
                                {
                                    r |= EnumerateFunc(&netResource.p[i], out);
                                }
                            }
                        }
                    } while (dwResultEnum != ERROR_NO_MORE_ITEMS);
                    return r;
                }

            } // namespace

            std::vector<Info> directoryList(const Path& value, const DirectoryListOptions& options)
            {
                std::vector<Info> out;
                if (!value.isEmpty())
                {
                    // Prepare the path.
                    const std::wstring path = String::toWide(value.get() + Path::getSeparator(PathSeparator::Windows) + '*');
                    WCHAR pathBuf[MAX_PATH];
                    size_t size = std::min(path.size(), static_cast<size_t>(MAX_PATH - 1));
                    memcpy(pathBuf, path.c_str(), size * sizeof(WCHAR));
                    pathBuf[size++] = 0;

                    // List the directory contents.
                    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16;
                    WIN32_FIND_DATAW ffd;
                    HANDLE hFind = FindFirstFileW(pathBuf, &ffd);
                    if (hFind != INVALID_HANDLE_VALUE)
                    {
                        try
                        {
                            do
                            {
                                const std::string fileName = utf16.to_bytes(ffd.cFileName);

                                bool filter = false;
                                if (ffd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
                                {
                                    filter = !options.showHidden;
                                }
                                if (fileName.size() == 1 && '.' == fileName[0])
                                {
                                    filter = true;
                                }
                                if (fileName.size() == 2 && '.' == fileName[0] && '.' == fileName[1])
                                {
                                    filter = true;
                                }
                                if (options.filter.size() && !String::match(fileName, options.filter))
                                {
                                    filter = true;
                                }
                                if (!filter && !(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && options.extensions.size())
                                {
                                    bool match = false;
                                    for (const auto& i : options.extensions)
                                    {
                                        if (String::match(fileName, '\\' + i + '$'))
                                        {
                                            match = true;
                                            break;
                                        }
                                    }
                                    if (!match)
                                    {
                                        filter = true;
                                    }
                                }

                                if (!filter)
                                {
                                    Info info(Path(value, fileName));
                                    sequence(info, options, out);
                                }
                            } while (FindNextFileW(hFind, &ffd) != 0);
                        }
                        catch (const std::exception&)
                        {
                            //! \bug How should we handle this error?
                        }
                        FindClose(hFind);
                    }
                    else if (value.isServer())
                    {
                        std::string path = value.get();
                        removeTrailingSeparator(path);

                        const size_t bufSize = path.size() + 1;
                        std::vector<char> buf(bufSize);
                        strcpy_s(buf.data(), bufSize, path.c_str());
                        NetResource netResource(16384);
                        netResource.p->lpRemoteName = buf.data();

                        std::vector<std::string> shares;
                        EnumerateFunc(netResource.p, shares);
                        for (const auto& i : shares)
                        {
                            out.push_back(i);
                        }
                    }
                    
                    // Sort the items.
                    sort(options, out);
                }
                return out;
            }

        } // namespace File
    } // namespace System
} // namespace djv

