// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <memory>

namespace djv
{
    namespace Image
    {
        class Color;
        class Data;

        //! \name Utility
        ///@{

        Color getAverageColor(const std::shared_ptr<Data>&);

        ///@}
    
    } // namespace Image
} // namespace djv

