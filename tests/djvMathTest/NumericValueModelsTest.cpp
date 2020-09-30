// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvMathTest/NumericValueModelsTest.h>

#include <djvMath/NumericValueModels.h>

using namespace djv::Core;
using namespace djv::Math;

namespace djv
{
    namespace MathTest
    {
        NumericValueModelsTest::NumericValueModelsTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::MathTest::NumericValueModelsTest", tempPath, context)
        {}
        
        void NumericValueModelsTest::run()
        {
            {
                auto model = IntValueModel::create();
                
                IntRange range;
                int value = 0;
                bool isMin = false;
                bool isMax = false;
                int smallIncrement = 0;
                int largeIncrement = 0;
                NumericValueOverflow overflow = NumericValueOverflow::Clamp;
                
                auto rangeObserver = Observer::Value<IntRange>::create(
                    model->observeRange(),
                    [&range](const IntRange& value)
                    {
                        range = value;
                    });
                auto valueObserver = Observer::Value<int>::create(
                    model->observeValue(),
                    [&value](int v)
                    {
                        value = v;
                    });
                auto isMinObserver = Observer::Value<bool>::create(
                    model->observeIsMin(),
                    [&isMin](bool value)
                    {
                        isMin = value;
                    });
                auto isMaxObserver = Observer::Value<bool>::create(
                    model->observeIsMax(),
                    [&isMax](bool value)
                    {
                        isMax = value;
                    });
                auto smallIncrementObserver = Observer::Value<int>::create(
                    model->observeSmallIncrement(),
                    [&smallIncrement](int value)
                    {
                        smallIncrement = value;
                    });
                auto largeIncrementObserver = Observer::Value<int>::create(
                    model->observeLargeIncrement(),
                    [&largeIncrement](int value)
                    {
                        largeIncrement = value;
                    });
                auto overflowObserver = Observer::Value<NumericValueOverflow>::create(
                    model->observeOverflow(),
                    [&overflow](NumericValueOverflow value)
                    {
                        overflow = value;
                    });

                model->setRange(IntRange(10, 1000));
                DJV_ASSERT(IntRange(10, 1000) == range);
                model->setSmallIncrement(10);
                DJV_ASSERT(10 == smallIncrement);
                model->setLargeIncrement(100);
                DJV_ASSERT(100 == largeIncrement);

                model->setValue(1001);
                DJV_ASSERT(1000 == value);
                model->setOverflow(NumericValueOverflow::Wrap);
                DJV_ASSERT(overflow == NumericValueOverflow::Wrap);
                model->setValue(1001);
                DJV_ASSERT(11 == value);
                
                model->setMax();
                DJV_ASSERT(1000 == value);
                DJV_ASSERT(isMax);
                model->setMin();
                DJV_ASSERT(10 == value);
                DJV_ASSERT(isMin);
                
                model->incrementSmall();
                DJV_ASSERT(20 == value);
                model->incrementLarge();
                DJV_ASSERT(120 == value);
                
                model->decrementSmall();
                DJV_ASSERT(110 == value);
                model->decrementLarge();
                DJV_ASSERT(10 == value);
            }

            {
                auto model = FloatValueModel::create();
                
                FloatRange range;
                float value = 0.F;
                bool isMin = false;
                bool isMax = false;
                float smallIncrement = 0.F;
                float largeIncrement = 0.F;
                NumericValueOverflow overflow = NumericValueOverflow::Clamp;
                
                auto rangeObserver = Observer::Value<FloatRange>::create(
                    model->observeRange(),
                    [&range](const FloatRange& value)
                    {
                        range = value;
                    });
                auto valueObserver = Observer::Value<float>::create(
                    model->observeValue(),
                    [&value](float v)
                    {
                        value = v;
                    });
                auto isMinObserver = Observer::Value<bool>::create(
                    model->observeIsMin(),
                    [&isMin](bool value)
                    {
                        isMin = value;
                    });
                auto isMaxObserver = Observer::Value<bool>::create(
                    model->observeIsMax(),
                    [&isMax](bool value)
                    {
                        isMax = value;
                    });
                auto smallIncrementObserver = Observer::Value<float>::create(
                    model->observeSmallIncrement(),
                    [&smallIncrement](float value)
                    {
                        smallIncrement = value;
                    });
                auto largeIncrementObserver = Observer::Value<float>::create(
                    model->observeLargeIncrement(),
                    [&largeIncrement](float value)
                    {
                        largeIncrement = value;
                    });
                auto overflowObserver = Observer::Value<NumericValueOverflow>::create(
                    model->observeOverflow(),
                    [&overflow](NumericValueOverflow value)
                    {
                        overflow = value;
                    });

                model->setRange(FloatRange(10.F, 1000.F));
                DJV_ASSERT(FloatRange(10.F, 1000.F) == range);
                model->setSmallIncrement(10.F);
                DJV_ASSERT(10.F == smallIncrement);
                model->setLargeIncrement(100.F);
                DJV_ASSERT(100.F == largeIncrement);

                model->setValue(1001.F);
                DJV_ASSERT(1000.F == value);
                model->setOverflow(NumericValueOverflow::Wrap);
                DJV_ASSERT(overflow == NumericValueOverflow::Wrap);
                model->setValue(1001.F);
                DJV_ASSERT(11.F == value);
                
                model->setMax();
                DJV_ASSERT(1000.F == value);
                DJV_ASSERT(isMax);
                model->setMin();
                DJV_ASSERT(10.F == value);
                DJV_ASSERT(isMin);
                
                model->incrementSmall();
                DJV_ASSERT(20.F == value);
                model->incrementLarge();
                DJV_ASSERT(120.F == value);
                
                model->decrementSmall();
                DJV_ASSERT(110.F == value);
                model->decrementLarge();
                DJV_ASSERT(10.F == value);
            }
        }
        
    } // namespace MathTest
} // namespace djv

