// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace System
    {
        namespace File
        {
            inline const std::string& IO::getFileName() const
            {
                return _fileName;
            }

            inline size_t IO::getSize() const
            {
                return _size;
            }

            inline size_t IO::getPos() const
            {
                return _pos;
            }
                    
#if defined(DJV_MMAP)
            inline const uint8_t* IO::mmapP() const
            {
                return _mmapP;
            }

            inline const uint8_t* IO::mmapEnd() const
            {
                return _mmapEnd;
            }
#endif // DJV_MMAP

            inline bool IO::hasEndianConversion() const
            {
                return _endianConversion;
            }
            
            inline void IO::setEndianConversion(bool in)
            {
                _endianConversion = in;
            }

        } // namespace File
    } // namespace System
} // namespace djv
