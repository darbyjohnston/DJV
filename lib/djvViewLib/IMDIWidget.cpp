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

#include <djvViewLib/IMDIWidget.h>

#include <djvUI/Border.h>
#include <djvUI/Label.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolButton.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct IMDIWidget::Private
        {
			bool shown = false;
			MDIResize resize = MDIResize::First;
            std::shared_ptr<UI::Label> titleLabel;
			std::shared_ptr<UI::HorizontalLayout> titleBar;
			std::shared_ptr<UI::HorizontalLayout> titleBarWidgetLayout;
			std::shared_ptr<UI::ToolButton> closeButton;
            std::shared_ptr<UI::VerticalLayout> childLayout;
            std::shared_ptr<UI::VerticalLayout> layout;
            std::shared_ptr<UI::Border> border;
            std::function<void(void)> closeCallback;
        };

        void IMDIWidget::_init(MDIResize resize, Context * context)
        {
            IWidget::_init(context);

            DJV_PRIVATE_PTR();
			p.resize = resize;

            p.titleLabel = UI::Label::create(context);
            p.titleLabel->setTextHAlign(UI::TextHAlign::Left);
            p.titleLabel->setMargin(UI::Style::MetricsRole::Margin);

            p.closeButton = UI::ToolButton::create(context);
			p.closeButton->setIcon("djvIconCloseSmall");

            p.titleBar = UI::HorizontalLayout::create(context);
			p.titleBar->setSpacing(UI::Style::MetricsRole::None);
			p.titleBar->addWidget(p.titleLabel, UI::Layout::RowStretch::Expand);
			p.titleBarWidgetLayout = UI::HorizontalLayout::create(context);
			p.titleBarWidgetLayout->setSpacing(UI::Style::MetricsRole::None);
			p.titleBar->addWidget(p.titleBarWidgetLayout);
			p.titleBar->addWidget(p.closeButton);

            p.childLayout = UI::VerticalLayout::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setBackgroundRole(UI::Style::ColorRole::Background);
            p.layout->setSpacing(UI::Style::MetricsRole::None);
            p.layout->addWidget(p.titleBar);
            p.layout->addSeparator();
            p.layout->addWidget(p.childLayout, UI::Layout::RowStretch::Expand);

            p.border = UI::Border::create(context);
            p.border->setMargin(UI::Style::MetricsRole::Handle);
            p.border->addWidget(p.layout);
            IContainer::addWidget(p.border);

            auto weak = std::weak_ptr<IMDIWidget>(std::dynamic_pointer_cast<IMDIWidget>(shared_from_this()));
            p.closeButton->setClickedCallback(
                [weak]
            {
                if (auto widget = weak.lock())
                {
					widget->hide();
                }
            });
        }

        IMDIWidget::IMDIWidget() :
            _p(new Private)
        {}

        IMDIWidget::~IMDIWidget()
        {}

        const std::string & IMDIWidget::getTitle() const
        {
            return _p->titleLabel->getText();
        }

        void IMDIWidget::setTitle(const std::string & text)
        {
            _p->titleLabel->setText(text);
        }

        void IMDIWidget::setCloseCallback(const std::function<void(void)> & value)
        {
            _p->closeCallback = value;
        }

		void IMDIWidget::addTitleBarWidget(const std::shared_ptr<Widget> & widget)
		{
			_p->titleBarWidgetLayout->addWidget(widget);
		}

        void IMDIWidget::addWidget(const std::shared_ptr<Widget>& widget)
        {
            _p->childLayout->addWidget(widget, UI::Layout::RowStretch::Expand);
        }

        void IMDIWidget::removeWidget(const std::shared_ptr<Widget>& widget)
        {
            _p->childLayout->addWidget(widget);
        }

        void IMDIWidget::clearWidgets()
        {
            _p->childLayout->clearWidgets();
        }

		void IMDIWidget::setVisible(bool value)
		{
			const bool prev = isVisible();
			IWidget::setVisible(value);
			DJV_PRIVATE_PTR();
			if (prev != isVisible())
			{
				if (value)
				{
					if (!p.shown)
					{
						p.shown = true;
						if (auto style = _getStyle().lock())
						{
							if (auto canvas = std::dynamic_pointer_cast<UI::MDI::Canvas>(getParent().lock()))
							{
								const BBox2f & g = canvas->getGeometry();
								const glm::vec2 c = g.getCenter();
								const float m = style->getMetric(UI::Style::MetricsRole::MarginDialog);
								const glm::vec2 & minimumSize = getMinimumSize();
								switch (p.resize)
								{
								case MDIResize::Minimum:
									canvas->setWidgetPos(
										std::dynamic_pointer_cast<IWidget>(shared_from_this()),
										glm::vec2(floorf(c.x - minimumSize.x / 2.f), floorf(c.y - minimumSize.y / 2.f)));
									resize(minimumSize);
									break;
								case MDIResize::Maximum:
									canvas->setWidgetPos(std::dynamic_pointer_cast<IWidget>(shared_from_this()), glm::vec2(m, m));
									resize(g.margin(-m).getSize());
									break;
								default: break;
								}
							}
						}
					}
				}
				else
				{
					if (p.closeCallback)
					{
						p.closeCallback();
					}
				}
			}
		}

        float IMDIWidget::getHeightForWidth(float value) const
        {
            return _p->border->getHeightForWidth(value);
        }

        void IMDIWidget::_preLayoutEvent(Event::PreLayout& event)
        {
            _setMinimumSize(_p->border->getMinimumSize());
        }

        void IMDIWidget::_layoutEvent(Event::Layout&)
        {
            _p->border->setGeometry(getGeometry());
        }

		void IMDIWidget::_localeEvent(Event::Locale &)
		{
			DJV_PRIVATE_PTR();
			p.closeButton->setTooltip(_getText(DJV_TEXT("djv::ViewLib::IMDIWidget", "Close Tooltip")));
		}

    } // namespace ViewLib
} // namespace djv

