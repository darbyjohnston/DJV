// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <tlTimeline/ColorOptions.h>

#include <feather-tk/core/ObservableValue.h>

namespace feather_tk
{
    class Context;
    class Settings;
}

namespace djv
{
    namespace app
    {
        //! Color model.
        class ColorModel : public std::enable_shared_from_this<ColorModel>
        {
            FEATHER_TK_NON_COPYABLE(ColorModel);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<feather_tk::Settings>&);

            ColorModel();

        public:
            ~ColorModel();

            //! Create a new model.
            static std::shared_ptr<ColorModel> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<feather_tk::Settings>&);

            //! Get the OpenColorIO options.
            const tl::timeline::OCIOOptions& getOCIOOptions() const;

            //! Observe the OpenColorIO options.
            std::shared_ptr<feather_tk::IObservableValue<tl::timeline::OCIOOptions> > observeOCIOOptions() const;

            //! Set the OpenColorIO options.
            void setOCIOOptions(const tl::timeline::OCIOOptions&);

            //! Get the LUT options.
            const tl::timeline::LUTOptions& getLUTOptions() const;

            //! Observe the LUT options.
            std::shared_ptr<feather_tk::IObservableValue<tl::timeline::LUTOptions> > observeLUTOptions() const;

            //! Set the LUT options.
            void setLUTOptions(const tl::timeline::LUTOptions&);

        private:
            FEATHER_TK_PRIVATE();
        };
    }
}
