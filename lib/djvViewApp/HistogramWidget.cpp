// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/HistogramWidget.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct HistogramWidget::Private
        {

        };

        void HistogramWidget::_init(const std::shared_ptr<Context>& context)
        {
            MDIWidget::_init(context);
            setClassName("djv::ViewApp::HistogramWidget");
        }

        HistogramWidget::HistogramWidget() :
            _p(new Private)
        {}

        HistogramWidget::~HistogramWidget()
        {}

        std::shared_ptr<HistogramWidget> HistogramWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<HistogramWidget>(new HistogramWidget);
            out->_init(context);
            return out;
        }

        void HistogramWidget::_initEvent(Event::Init & event)
        {
            MDIWidget::_initEvent(event);
            if (event.getData().text)
            {
                setTitle(_getText(DJV_TEXT("Histogram")));
            }
        }

    } // namespace ViewApp
} // namespace djv

