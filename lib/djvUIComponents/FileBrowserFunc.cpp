// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/FileBrowserPrivate.h>

#include <djvSystem/PathFunc.h>

using namespace djv::Core;

namespace djv
{
    namespace UIComponents
    {
        namespace FileBrowser
        {
            std::string getPathLabel(const System::File::Path& value)
            {
                std::string out = value.getFileName();
                if (out.empty())
                {
                    out = value.get();
                    System::File::removeTrailingSeparator(out);
                    out = std::string(System::File::Path(out).getFileName());
                    if (out.empty())
                    {
                        out = value.get();
                    }
                }
                return out;
            }

        } // namespace FileBrowser
    } // namespace UIComponents
} // namespace djv

