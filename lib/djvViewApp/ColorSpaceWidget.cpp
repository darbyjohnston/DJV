// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ColorSpaceWidget.h>

#include <djvViewApp/ColorSpaceWidgetPrivate.h>

#include <djvUI/Bellows.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ColorSpaceWidget::Private
        {
            std::shared_ptr<ColorSpaceConfigWidget> configWidget;
            std::shared_ptr<ColorSpaceDisplayWidget> displayWidget;
            std::shared_ptr<ColorSpaceImageWidget> imageWidget;

            std::shared_ptr<UI::LabelSizeGroup> sizeGroup;
            std::map<std::string, std::shared_ptr<UI::Bellows> > bellows;
            std::shared_ptr<UI::ScrollWidget> scrollWidget;
        };

        void ColorSpaceWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            MDIWidget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ColorSpaceWidget");

            p.sizeGroup = UI::LabelSizeGroup::create();

            p.configWidget = ColorSpaceConfigWidget::create(context);
            p.configWidget->setSizeGroup(p.sizeGroup);
            p.bellows["Config"] = UI::Bellows::create(context);
            p.bellows["Config"]->addChild(p.configWidget);

            p.displayWidget = ColorSpaceDisplayWidget::create(context);
            p.displayWidget->setSizeGroup(p.sizeGroup);
            p.bellows["Display"] = UI::Bellows::create(context);
            p.bellows["Display"]->addChild(p.displayWidget);

            p.imageWidget = ColorSpaceImageWidget::create(context);
            p.imageWidget->setSizeGroup(p.sizeGroup);
            p.bellows["Image"] = UI::Bellows::create(context);
            p.bellows["Image"]->addChild(p.imageWidget);

            for (const auto& i : p.bellows)
            {
                i.second->close(false);
            }

            p.scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Both, context);
            p.scrollWidget->setBorder(false);
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->setBackgroundRole(UI::ColorRole::Background);
            vLayout->addChild(p.bellows["Config"]);
            vLayout->addChild(p.bellows["Display"]);
            vLayout->addChild(p.bellows["Image"]);
            p.scrollWidget->addChild(vLayout);
            addChild(p.scrollWidget);
        }

        ColorSpaceWidget::ColorSpaceWidget() :
            _p(new Private)
        {}

        ColorSpaceWidget::~ColorSpaceWidget()
        {}

        std::shared_ptr<ColorSpaceWidget> ColorSpaceWidget::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<ColorSpaceWidget>(new ColorSpaceWidget);
            out->_init(context);
            return out;
        }

        std::map<std::string, bool> ColorSpaceWidget::getBellowsState() const
        {
            DJV_PRIVATE_PTR();
            std::map<std::string, bool> out;
            for (const auto& i : p.bellows)
            {
                out[i.first] = i.second->isOpen();
            }
            return out;
        }

        void ColorSpaceWidget::setBellowsState(const std::map<std::string, bool>& value)
        {
            DJV_PRIVATE_PTR();
            for (const auto& i : value)
            {
                const auto j = p.bellows.find(i.first);
                if (j != p.bellows.end())
                {
                    j->second->setOpen(i.second, false);
                }
            }
        }

        void ColorSpaceWidget::_initLayoutEvent(Event::InitLayout&)
        {
            _p->sizeGroup->calcMinimumSize();
        }

        void ColorSpaceWidget::_initEvent(Event::Init & event)
        {
            MDIWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                setTitle(_getText(DJV_TEXT("widget_color_space")));
                p.bellows["Config"]->setText(_getText(DJV_TEXT("widget_color_space_config")));
                p.bellows["Display"]->setText(_getText(DJV_TEXT("widget_color_space_display")));
                p.bellows["Image"]->setText(_getText(DJV_TEXT("widget_color_space_image")));
            }
        }

    } // namespace ViewApp
} // namespace djv

