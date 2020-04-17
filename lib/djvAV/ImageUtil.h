// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/AV.h>

#include <memory>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            class Color;
            class Data;

            Color getAverageColor(const std::shared_ptr<Data>&);

        } // namespace Image
    } // namespace AV
} // namespace djv
