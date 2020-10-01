// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Core
    {
        namespace OS
        {
            inline char getListSeparator(ListSeparator value)
            {
                return ListSeparator::Unix == value ? ':' : ';';
            }
            
            inline char getCurrentListSeparator()
            {
#if defined(DJV_PLATFORM_WINDOWS)
                return getListSeparator(ListSeparator::Windows);
#else
                return getListSeparator(ListSeparator::Unix);
#endif // DJV_PLATFORM_WINDOWS
            }
        
        } // namespace OS
    } // namespace Core
} // namespace djv
