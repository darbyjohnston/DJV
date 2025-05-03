// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Models/ColorModel.h>

namespace djv
{
    namespace app
    {
        struct ColorModel::Private
        {
            std::weak_ptr<dtk::Context> context;
            std::shared_ptr<dtk::ObservableValue<tl::timeline::OCIOOptions> > ocioOptions;
            std::shared_ptr<dtk::ObservableValue<tl::timeline::LUTOptions> > lutOptions;
        };

        void ColorModel::_init(const std::shared_ptr<dtk::Context>& context)
        {
            DTK_P();

            p.context = context;

            p.ocioOptions = dtk::ObservableValue<tl::timeline::OCIOOptions>::create();
            p.lutOptions = dtk::ObservableValue<tl::timeline::LUTOptions>::create();
        }

        ColorModel::ColorModel() :
            _p(new Private)
        {}

        ColorModel::~ColorModel()
        {}

        std::shared_ptr<ColorModel> ColorModel::create(const std::shared_ptr<dtk::Context>& context)
        {
            auto out = std::shared_ptr<ColorModel>(new ColorModel);
            out->_init(context);
            return out;
        }

        const tl::timeline::OCIOOptions& ColorModel::getOCIOOptions() const
        {
            return _p->ocioOptions->get();
        }

        std::shared_ptr<dtk::IObservableValue<tl::timeline::OCIOOptions> > ColorModel::observeOCIOOptions() const
        {
            return _p->ocioOptions;
        }

        void ColorModel::setOCIOOptions(const tl::timeline::OCIOOptions& value)
        {
            _p->ocioOptions->setIfChanged(value);
        }

        const tl::timeline::LUTOptions& ColorModel::getLUTOptions() const
        {
            return _p->lutOptions->get();
        }

        std::shared_ptr<dtk::IObservableValue<tl::timeline::LUTOptions> > ColorModel::observeLUTOptions() const
        {
            return _p->lutOptions;
        }

        void ColorModel::setLUTOptions(const tl::timeline::LUTOptions& value)
        {
            _p->lutOptions->setIfChanged(value);
        }
    }
}
