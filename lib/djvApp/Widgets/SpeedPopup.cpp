// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Widgets/SpeedPopup.h>

#include <feather-tk/ui/ListWidget.h>

#include <feather-tk/core/Format.h>

namespace djv
{
    namespace app
    {
        struct SpeedPopup::Private
        {
            std::vector<double> speeds;
            std::shared_ptr<feather_tk::ListWidget> listWidget;
            std::function<void(double)> callback;
        };

        void SpeedPopup::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            double defaultSpeed,
            const std::shared_ptr<IWidget>& parent)
        {
            IMenuPopup::_init(
                context,
                "djv::app::SpeedPopup",
                parent);
            FEATHER_TK_P();

            p.speeds =
            {
                defaultSpeed,
                24000.0 / 1001.0,
                24.0,
                25.0,
                30000.0 / 1001.0,
                30.0,
                48.0,
                50.0,
                60000.0 / 1001.0,
                60.0,
                90.0,
                96.0,
                120.0,
                18.0,
                16.0,
                15.0,
                12.0,
                6.0,
                3.0,
                1.0
            };

            p.listWidget = feather_tk::ListWidget::create(context, feather_tk::ButtonGroupType::Click);
            setWidget(p.listWidget);

            _widgetUpdate();

            auto weak = std::weak_ptr<SpeedPopup>(std::dynamic_pointer_cast<SpeedPopup>(shared_from_this()));
            p.listWidget->setCallback(
                [weak](int index, bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (value && index >= 0 && index < widget->_p->speeds.size())
                        {
                            if (widget->_p->callback)
                            {
                                widget->_p->callback(widget->_p->speeds[index]);
                            }
                        }
                    }
                });
        }

        SpeedPopup::SpeedPopup() :
            _p(new Private)
        {}

        SpeedPopup::~SpeedPopup()
        {}

        std::shared_ptr<SpeedPopup> SpeedPopup::create(
            const std::shared_ptr<feather_tk::Context>& context,
            double defaultSpeed,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<SpeedPopup>(new SpeedPopup);
            out->_init(context, defaultSpeed, parent);
            return out;
        }

        void SpeedPopup::setCallback(const std::function<void(double)>& value)
        {
            _p->callback = value;
        }

        void SpeedPopup::_widgetUpdate()
        {
            FEATHER_TK_P();
            std::vector<std::string> items;
            for (size_t i = 0; i < p.speeds.size(); ++i)
            {
                items.push_back(0 == i ?
                    feather_tk::Format("Default: {0}").arg(p.speeds[i], 2) :
                    feather_tk::Format("{0}").arg(p.speeds[i], 2));
            }
            p.listWidget->setItems(items);
        }
    }
}
