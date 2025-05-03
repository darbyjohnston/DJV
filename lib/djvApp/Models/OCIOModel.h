// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <tlTimeline/ColorOptions.h>

#include <dtk/core/ObservableValue.h>

namespace dtk
{
    class Context;
}

namespace djv
{
    namespace app
    {
        //! OpenColorIO model data.
        struct OCIOModelData
        {
            bool enabled = false;
            std::string fileName;
            std::vector<std::string> inputs;
            size_t inputIndex = 0;
            std::vector<std::string> displays;
            size_t displayIndex = 0;
            std::vector<std::string> views;
            size_t viewIndex = 0;
            std::vector<std::string> looks;
            size_t lookIndex = 0;

            bool operator == (const OCIOModelData&) const;
            bool operator != (const OCIOModelData&) const;
        };

        //! OpenColorIO model.
        class OCIOModel : public std::enable_shared_from_this<OCIOModel>
        {
            DTK_NON_COPYABLE(OCIOModel);

        protected:
            void _init(const std::shared_ptr<dtk::Context>&);

            OCIOModel();

        public:
            ~OCIOModel();

            //! Create a new model.
            static std::shared_ptr<OCIOModel> create(const std::shared_ptr<dtk::Context>&);

            //! Observe the options.
            std::shared_ptr<dtk::IObservableValue<tl::timeline::OCIOOptions> > observeOptions() const;

            //! Set the options.
            void setOptions(const tl::timeline::OCIOOptions&);

            //! Set whether the color configuration is enabled.
            void setEnabled(bool);

            //! Set the color configuration.
            void setConfig(const std::string& fileName);

            //! Observe the model data.
            std::shared_ptr<dtk::IObservableValue<OCIOModelData> > observeData() const;

            //! Set the input index.
            void setInputIndex(size_t);

            //! Set the display index.
            void setDisplayIndex(size_t);

            //! Set the view index.
            void setViewIndex(size_t);

            //! Set the look index.
            void setLookIndex(size_t);

        private:
            OCIOModelData _getData(const tl::timeline::OCIOOptions&) const;

            DTK_PRIVATE();
        };
    }
}
