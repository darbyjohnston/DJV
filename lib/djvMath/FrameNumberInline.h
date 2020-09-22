// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Math
    {
        namespace Frame
        {
            inline const std::vector<Range>& Sequence::getRanges() const noexcept
            {
                return _ranges;
            }

            inline bool Sequence::isValid() const noexcept
            {
                return _ranges.size() > 0;
            }

            inline size_t Sequence::getPad() const noexcept
            {
                return _pad;
            }

            inline void Sequence::setPad(size_t value)
            {
                _pad = value;
            }

            inline bool Sequence::operator == (const Sequence& value) const
            {
                return _ranges == value._ranges && _pad == value._pad;
            }

            inline bool Sequence::operator != (const Sequence& value) const
            {
                return !(*this == value);
            }
        
        } // namespace Frame
    } // namespace Math
} // namespace djv
