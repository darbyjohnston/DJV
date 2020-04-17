// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/INumericValueModel.h>

namespace djv
{
    namespace Core
    {
        //! This class provides an integer number model.
        class IntValueModel : public INumericValueModel<int>
        {
            DJV_NON_COPYABLE(IntValueModel);

        protected:
            void _init();
            IntValueModel();

        public:
            static std::shared_ptr<IntValueModel> create();
        };

        //! This class provides a floating-point number model.
        class FloatValueModel : public INumericValueModel<float>
        {
            DJV_NON_COPYABLE(FloatValueModel);

        protected:
            void _init();
            FloatValueModel();

        public:
            static std::shared_ptr<FloatValueModel> create();
        };

    } // namespace Core
} // namespace djv
