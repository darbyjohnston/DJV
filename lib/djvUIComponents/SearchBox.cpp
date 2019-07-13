//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvUIComponents/SearchBox.h>

#include <djvUI/Border.h>
#include <djvUI/Icon.h>
#include <djvUI/LineEditBase.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SoloLayout.h>
#include <djvUI/ToolButton.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct SearchBox::Private
        {
            std::shared_ptr<LineEditBase> lineEditBase;
            std::shared_ptr<SoloLayout> soloLayout;
            std::shared_ptr<Border> border;
            std::function<void(const std::string &)> filterCallback;
        };

        void SearchBox::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::SearchBox");
            setVAlign(VAlign::Center);

            p.lineEditBase = LineEditBase::create(context);
            p.lineEditBase->setTextSizeRole(MetricsRole::SearchBox);
            
            auto searchIcon = Icon::create(context);
            searchIcon->setIcon("djvIconSearch");
            searchIcon->setIconColorRole(ColorRole::Foreground);
            
            auto clearButton = ToolButton::create(context);
            clearButton->setIcon("djvIconClear");
            clearButton->setBackgroundRole(ColorRole::None);
            clearButton->setInsideMargin(MetricsRole::None);
            
            auto layout = HorizontalLayout::create(context);
            layout->setSpacing(MetricsRole::None);
            layout->addChild(p.lineEditBase);
            layout->setStretch(p.lineEditBase, RowStretch::Expand);
            p.soloLayout = SoloLayout::create(context);
            p.soloLayout->addChild(searchIcon);
            p.soloLayout->addChild(clearButton);
            layout->addChild(p.soloLayout);
            
            p.border = Border::create(context);
            p.border->setMargin(MetricsRole::Border);
            p.border->addChild(layout);
            addChild(p.border);

            auto weak = std::weak_ptr<SearchBox>(std::dynamic_pointer_cast<SearchBox>(shared_from_this()));
            p.lineEditBase->setFocusCallback(
                [weak](bool value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->border->setBorderColorRole(value ? ColorRole::TextFocus : ColorRole::Border);
                }
            });

            p.lineEditBase->setTextChangedCallback(
                [weak, searchIcon, clearButton](const std::string & value)
            {
                if (auto widget = weak.lock())
                {
                    std::shared_ptr<Widget> currentWidget = searchIcon;
                    if (!value.empty())
                    {
                        currentWidget = clearButton;
                    }
                    widget->_p->soloLayout->setCurrentWidget(currentWidget);
                    if (widget->_p->filterCallback)
                    {
                        widget->_p->filterCallback(value);
                    }
                }
            });

            clearButton->setClickedCallback(
                [weak]
            {
                if (auto widget = weak.lock())
                {
                    widget->clearFilter();
                }
            });
        }

        SearchBox::SearchBox() :
            _p(new Private)
        {}

        SearchBox::~SearchBox()
        {}

        std::shared_ptr<SearchBox> SearchBox::create(Context * context)
        {
            auto out = std::shared_ptr<SearchBox>(new SearchBox);
            out->_init(context);
            return out;
        }

        const std::string & SearchBox::getFilter() const
        {
            return _p->lineEditBase->getText();
        }

        void SearchBox::setFilter(const std::string & value)
        {
            _p->lineEditBase->setText(value);
        }

        void SearchBox::clearFilter()
        {
            _p->lineEditBase->setText(std::string());
        }

        void SearchBox::setFilterCallback(const std::function<void(const std::string &)> & value)
        {
            _p->filterCallback = value;
        }

        float SearchBox::getHeightForWidth(float value) const
        {
            const auto& style = _getStyle();
            const glm::vec2 m = getMargin().getSize(style);
            float out = _p->border->getHeightForWidth(value - m.x) + m.y;
            return out;
        }

        void SearchBox::_preLayoutEvent(Event::PreLayout & event)
        {
            const auto& style = _getStyle();
            _setMinimumSize(_p->border->getMinimumSize() + getMargin().getSize(style));
        }

        void SearchBox::_layoutEvent(Event::Layout & event)
        {
            const BBox2f g = getGeometry();
            const auto& style = _getStyle();
            _p->border->setGeometry(getMargin().bbox(g, style));
        }

    } // namespace UI
} // namespace djv
