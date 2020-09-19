// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2018-2020 Darby Johnston
// All rights reserved.

#include <djvSystem/FileFunc.h>

#include <stdio.h>

namespace djv
{
    namespace System
    {
        namespace File
        {
            FILE* fopen(const std::string& fileName, const std::string& mode)
            {
                return ::fopen(fileName.c_str(), mode.c_str());
            }

        } // namespace File
    } // namespace System
} // namespace djv

