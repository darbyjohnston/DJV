// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/Spacer.h>

#include <djvUI/Style.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            struct Spacer::Private
            {
                Orientation orientation = Orientation::First;
                MetricsRole spacerSize = MetricsRole::Spacing;
                MetricsRole spacerOppositeSize = MetricsRole::None;
            };

            void Spacer::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                setClassName("djv::UI::Layout::Spacer");
            }

            Spacer::Spacer() :
                _p(new Private)
            {}

            Spacer::~Spacer()
            {}

            std::shared_ptr<Spacer> Spacer::create(Orientation orientation, const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<Spacer>(new Spacer);
                out->_init(context);
                out->setOrientation(orientation);
                return out;
            }

            Orientation Spacer::getOrientation() const
            {
                return _p->orientation;
            }

            void Spacer::setOrientation(Orientation value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.orientation)
                    return;
                p.orientation = value;
                _resize();
            }

            MetricsRole Spacer::getSpacerSize() const
            {
                return _p->spacerSize;
            }

            void Spacer::setSpacerSize(MetricsRole value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.spacerSize)
                    return;
                p.spacerSize = value;
                _resize();
            }

            MetricsRole Spacer::getSpacerOppositeSize() const
            {
                return _p->spacerOppositeSize;
            }

            void Spacer::setSpacerOppositeSize(MetricsRole value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.spacerOppositeSize)
                    return;
                p.spacerOppositeSize = value;
                _resize();
            }

            void Spacer::_preLayoutEvent(System::Event::PreLayout& event)
            {
                glm::vec2 minimumSize = glm::vec2(0.F, 0.F);
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                switch (p.orientation)
                {
                case Orientation::Horizontal:
                    minimumSize.x = style->getMetric(p.spacerSize);
                    minimumSize.y = style->getMetric(p.spacerOppositeSize);
                    break;
                case Orientation::Vertical:
                    minimumSize.x = style->getMetric(p.spacerOppositeSize);
                    minimumSize.y = style->getMetric(p.spacerSize);
                    break;
                default: break;
                }
                _setMinimumSize(minimumSize + getMargin().getSize(style));
            }

            HorizontalSpacer::HorizontalSpacer()
            {}

            std::shared_ptr<HorizontalSpacer> HorizontalSpacer::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<HorizontalSpacer>(new HorizontalSpacer);
                out->_init(context);
                out->setOrientation(Orientation::Horizontal);
                return out;
            }

            VerticalSpacer::VerticalSpacer()
            {}

            std::shared_ptr<VerticalSpacer> VerticalSpacer::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<VerticalSpacer>(new VerticalSpacer);
                out->_init(context);
                out->setOrientation(Orientation::Vertical);
                return out;
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
