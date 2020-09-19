// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvMath/NumericValueModels.h>

namespace djv
{
    namespace Math
    {
        void IntValueModel::_init()
        {
            INumericValueModel<int>::_init();
            setRange(IntRange(0, 100));
            setSmallIncrement(1);
            setLargeIncrement(10);
        }

        IntValueModel::IntValueModel()
        {}

        std::shared_ptr<IntValueModel> IntValueModel::create()
        {
            auto out = std::shared_ptr<IntValueModel>(new IntValueModel);
            out->_init();
            return out;
        }

        void FloatValueModel::_init()
        {
            INumericValueModel<float>::_init();
            setRange(FloatRange(0.F, 1.F));
            setSmallIncrement(.1F);
            setLargeIncrement(.2F);
        }

        FloatValueModel::FloatValueModel()
        {}

        std::shared_ptr<FloatValueModel> FloatValueModel::create()
        {
            auto out = std::shared_ptr<FloatValueModel>(new FloatValueModel);
            out->_init();
            return out;
        }

    } // namespace Math
} // namespace djv
