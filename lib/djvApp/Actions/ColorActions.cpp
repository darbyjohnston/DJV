// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Actions/ColorActions.h>

#include <djvApp/Models/ColorModel.h>
#include <djvApp/App.h>

namespace djv
{
    namespace app
    {
        struct ColorActions::Private
        {
            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::OCIOOptions> > ocioObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::LUTOptions> > lutObserver;
        };

        void ColorActions::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app)
        {
            IActions::_init(context, app, "Color");
            FEATHER_TK_P();

            auto appWeak = std::weak_ptr<App>(app);
            _actions["OCIO"] = feather_tk::Action::create(
                "Enable OCIO",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getColorModel()->getOCIOOptions();
                        options.enabled = value;
                        app->getColorModel()->setOCIOOptions(options);
                    }
                });
            _actions["LUT"] = feather_tk::Action::create(
                "Enable LUT",
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getColorModel()->getLUTOptions();
                        options.enabled = value;
                        app->getColorModel()->setLUTOptions(options);
                    }
                });

            _tooltips =
            {
                { "OCIO", "Toggle whether OCIO is enabled." },
                { "LUT", "Toggle whether the LUT is enabled." }
            };

            _shortcutsUpdate(app->getSettingsModel()->getShortcuts());

            p.ocioObserver = feather_tk::ValueObserver<tl::timeline::OCIOOptions>::create(
                app->getColorModel()->observeOCIOOptions(),
                [this](const tl::timeline::OCIOOptions& value)
                {
                    _actions["OCIO"]->setChecked(value.enabled);
                });

            p.lutObserver = feather_tk::ValueObserver<tl::timeline::LUTOptions>::create(
                app->getColorModel()->observeLUTOptions(),
                [this](const tl::timeline::LUTOptions& value)
                {
                    _actions["LUT"]->setChecked(value.enabled);
                });
        }

        ColorActions::ColorActions() :
            _p(new Private)
        {}

        ColorActions::~ColorActions()
        {}

        std::shared_ptr<ColorActions> ColorActions::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app)
        {
            auto out = std::shared_ptr<ColorActions>(new ColorActions);
            out->_init(context, app);
            return out;
        }
    }
}
