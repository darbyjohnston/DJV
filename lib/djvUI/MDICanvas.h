// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        //! Multiple document interface (MDI).
        namespace MDI
        {
            class IWidget;

            //! MDI canvas widget.
            class Canvas : public Widget
            {
                DJV_NON_COPYABLE(Canvas);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                Canvas();

            public:
                ~Canvas() override;

                //! Create a new MDI canvas.
                static std::shared_ptr<Canvas> create(const std::shared_ptr<System::Context>&);

                //! \name Canvas Size
                ///@{

                const glm::vec2& getCanvasSize() const;

                void setCanvasSize(const glm::vec2&);

                ///@}

                //! \name Active Widget
                ///@{

                const std::shared_ptr<IWidget>& getActiveWidget() const;

                void nextWidget();
                void prevWidget();
                void setActiveCallback(const std::function<void(const std::shared_ptr<IWidget>&)>&);

                ///@}

                //! \name Widget Geometry
                ///@{

                glm::vec2 getWidgetPos(const std::shared_ptr<IWidget>&) const;

                //! \todo We should convert widgets to use local coordinates and then
                //! we could remove this and use Widget::move() instead.
                void setWidgetPos(const std::shared_ptr<IWidget>&, const glm::vec2&);
                void setWidgetGeometry(const std::shared_ptr<IWidget>&, const Math::BBox2f&);

                ///@}

                //! \name Maximized
                ///@{

                bool isMaximized() const;

                void setMaximize(bool);
                void setMaximizeCallback(const std::function<void(bool)>&);

                ///@}

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;

                void _childAddedEvent(System::Event::ChildAdded&) override;
                void _childRemovedEvent(System::Event::ChildRemoved&) override;
                void _childOrderEvent(System::Event::ChildOrder&) override;
                bool _eventFilter(const std::shared_ptr<System::IObject>&, System::Event::Event&) override;

            private:
                void _doActiveCallback();

                DJV_PRIVATE();
            };

        } // namespace MDI
    } // namespace UI
} // namespace djv
