//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvUI/GroupBox.h>

#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>

#include <djvAV/Render2D.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            namespace
            {
                class HeaderWidget : public Widget
                {
                    DJV_NON_COPYABLE(HeaderWidget);

                protected:
                    void _init(Context * context)
                    {
                        Widget::_init(context);

                        _label = Label::create(context);
                        _label->setTextColorRole(Style::ColorRole::HeaderForeground);
                        _label->setParent(shared_from_this());
                    }

                    HeaderWidget()
                    {}

                public:
                    static std::shared_ptr<HeaderWidget> create(Context * context)
                    {
                        auto out = std::shared_ptr<HeaderWidget>(new HeaderWidget);
                        out->_init(context);
                        return out;
                    }

                    void setText(const std::string & value)
                    {
                        _label->setText(value);
                    }

                protected:
                    void _preLayoutEvent(Event::PreLayout&) override
                    {
                        if (auto style = _getStyle().lock())
                        {
                            const float m = style->getMetric(Style::MetricsRole::Margin);
                            const float ms = style->getMetric(Style::MetricsRole::MarginSmall);
                            const float ml = style->getMetric(Style::MetricsRole::MarginLarge);
                            const float b = style->getMetric(Style::MetricsRole::Border);
                            auto size = _label->getMinimumSize();
                            size.x += ml * 2.f;
                            size.y += ms * 2.f + b * 2.f;
                            _setMinimumSize(size);
                        }
                    }

                    void _layoutEvent(Event::Layout&) override
                    {
                        if (auto style = _getStyle().lock())
                        {
                            const auto & g = getGeometry();
                            const float m = style->getMetric(Style::MetricsRole::Margin);
                            const float ms = style->getMetric(Style::MetricsRole::MarginSmall);
                            const float ml = style->getMetric(Style::MetricsRole::MarginLarge);
                            const auto & labelSize = _label->getMinimumSize();
                            _label->setGeometry(BBox2f(g.min.x + ml, g.min.y + ms, labelSize.x, labelSize.y));
                        }
                    }

                    void _paintEvent(Event::Paint&) override
                    {
                        if (auto render = _getRender().lock())
                        {
                            if (auto style = _getStyle().lock())
                            {
                                const auto & g = getGeometry();
                                const float m = style->getMetric(Style::MetricsRole::Margin);
                                const float ms = style->getMetric(Style::MetricsRole::MarginSmall);
                                const float ml = style->getMetric(Style::MetricsRole::MarginLarge);
                                const float b = style->getMetric(Style::MetricsRole::Border);
                                const auto & labelSize = _label->getMinimumSize();
                                render->setFillColor(_getColorWithOpacity(style->getColor(Style::ColorRole::HeaderBackground)));
                                render->drawRoundedRect(BBox2f(g.min.x, g.min.y, labelSize.x + ml * 2.f, labelSize.y + ms * 2.f), ms, AV::Side::Top);
                                render->drawRect(BBox2f(g.min.x, g.max.y - b * 2.f, g.w(), b * 2.f));
                            }
                        }
                    }

                private:
                    std::shared_ptr<Label> _label;
                };

            } // namespace

            struct GroupBox::Private
            {
                std::shared_ptr<HeaderWidget> headerWidget;
                std::shared_ptr<Layout::Stack> childLayout;
                std::shared_ptr<Layout::Vertical> layout;
            };

            void GroupBox::_init(Context * context)
            {
                IContainer::_init(context);

                setClassName("djv::UI::Layout::GroupBox");

                DJV_PRIVATE_PTR();
                p.headerWidget = HeaderWidget::create(context);
                
                p.childLayout = Layout::Stack::create(context);
                p.childLayout->setMargin(Style::MetricsRole::Margin);

                p.layout = Layout::Vertical::create(context);
                p.layout->setSpacing(Style::MetricsRole::None);
                p.layout->addWidget(p.headerWidget);
                p.layout->addWidget(p.childLayout, Layout::RowStretch::Expand);
                IContainer::addWidget(p.layout);
            }

            GroupBox::GroupBox() :
                _p(new Private)
            {}

            GroupBox::~GroupBox()
            {}

            std::shared_ptr<GroupBox> GroupBox::create(Context * context)
            {
                auto out = std::shared_ptr<GroupBox>(new GroupBox);
                out->_init(context);
                return out;
            }

            std::shared_ptr<GroupBox> GroupBox::create(const std::string & text, Context * context)
            {
                auto out = std::shared_ptr<GroupBox>(new GroupBox);
                out->_init(context);
                out->setText(text);
                return out;
            }

            void GroupBox::setText(const std::string & text)
            {
                _p->headerWidget->setText(text);
            }
            
            void GroupBox::addWidget(const std::shared_ptr<Widget>& value)
            {
                _p->childLayout->addWidget(value);
            }

            void GroupBox::removeWidget(const std::shared_ptr<Widget>& value)
            {
                _p->childLayout->removeWidget(value);
            }

            void GroupBox::clearWidgets()
            {
                _p->childLayout->clearWidgets();
            }

            float GroupBox::getHeightForWidth(float value) const
            {
                return _p->layout->getHeightForWidth(value);
            }

            void GroupBox::_preLayoutEvent(Event::PreLayout& event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void GroupBox::_layoutEvent(Event::Layout& event)
            {
                _p->layout->setGeometry(getGeometry());
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
