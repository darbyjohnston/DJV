// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/Separator.h>

#include <djvUI/Style.h>

#include <djvRender2D/Render.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            struct Separator::Private
            {
                float width = 0.F;
            };

            void Separator::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                setClassName("djv::UI::Layout::Separator");
                setBackgroundRole(ColorRole::Border);
            }

            Separator::Separator() :
                _p(new Private)
            {}

            Separator::~Separator()
            {}

            std::shared_ptr<Separator> Separator::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<Separator>(new Separator);
                out->_init(context);
                return out;
            }

            void Separator::_preLayoutEvent(System::Event::PreLayout& event)
            {
                DJV_PRIVATE_PTR();

                const auto& style = _getStyle();
                p.width = style->getMetric(MetricsRole::Border);

                glm::vec2 minimumSize = glm::vec2(0.F, 0.F);
                minimumSize += p.width;
                _setMinimumSize(minimumSize);
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
