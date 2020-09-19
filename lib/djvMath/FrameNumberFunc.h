// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvMath/FrameNumber.h>

#include <djvCore/RapidJSONFunc.h>

namespace djv
{
    namespace Math
    {
        namespace Frame
        {
            //! \name Utilities
            ///@{

            bool isValid(const Range&);

            ///@}

            //! \name Conversion
            ///@{

            std::vector<Number> toFrames(const Range&);
            std::vector<Number> toFrames(const Sequence&);
            Sequence fromFrames(const std::vector<Number>&);

            std::string toString(Number, size_t pad = 0);
            std::string toString(const Range&, size_t pad = 0);
            std::string toString(const Sequence&);

            //! Throws:
            //! - std::exception
            void fromString(const std::string&, Range&, size_t& pad);

            //! Throws:
            //! - std::exception
            void fromString(const std::string&, Sequence&);

            ///@}

        } // namespace Frame
    } // namespace Math

    std::ostream& operator << (std::ostream&, const Math::Frame::Sequence&);

    //! Throws:
    //! - std::exception
    std::istream& operator >> (std::istream&, Math::Frame::Sequence&);

    rapidjson::Value toJSON(const Math::Frame::Sequence&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Math::Frame::Sequence&);

} // namespace djv

