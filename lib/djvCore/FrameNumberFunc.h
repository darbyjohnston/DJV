// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/FrameNumber.h>
#include <djvCore/RapidJSONFunc.h>

namespace djv
{
    namespace Core
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
    } // namespace Core

    std::ostream& operator << (std::ostream&, const Core::Frame::Sequence&);

    //! Throws:
    //! - std::exception
    std::istream& operator >> (std::istream&, Core::Frame::Sequence&);

    rapidjson::Value toJSON(const Core::Frame::Sequence&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Core::Frame::Sequence&);

} // namespace djv

