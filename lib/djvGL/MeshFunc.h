// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvGL/Mesh.h>

#include <djvCore/Enum.h>

namespace djv
{
    namespace GL
    {
        //! \name Information
        ///@{

        //! Get the VBO type byte count.
        size_t getVertexByteCount(VBOType) noexcept;

        ///@}

        DJV_ENUM_HELPERS(VBOType);

    } // namespace GL

    DJV_ENUM_SERIALIZE_HELPERS(GL::VBOType);

} // namespace djv
