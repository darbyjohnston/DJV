// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            //! Splitter widget.
            class Splitter : public Widget
            {
                DJV_NON_COPYABLE(Splitter);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                Splitter();

            public:
                ~Splitter() override;

                static std::shared_ptr<Splitter> create(Orientation, const std::shared_ptr<System::Context>&);

                Orientation getOrientation() const;

                void setOrientation(Orientation);

                const std::vector<float>& getSplit() const;

                void setSplit(const std::vector<float>&);
                void setSplitCallback(const std::function<void(const std::vector<float>&)>&);
                void distributeEvenly();

                ColorRole getHandleColorRole() const;

                void setHandleColorRole(ColorRole);

                float getHeightForWidth(float) const override;
                void addChild(const std::shared_ptr<IObject>&) override;
                void removeChild(const std::shared_ptr<IObject>&) override;

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;
                void _paintEvent(System::Event::Paint&) override;
                void _pointerLeaveEvent(System::Event::PointerLeave&) override;
                void _pointerMoveEvent(System::Event::PointerMove&) override;
                void _buttonPressEvent(System::Event::ButtonPress&) override;
                void _buttonReleaseEvent(System::Event::ButtonRelease&) override;

                void _initEvent(System::Event::Init&) override;

            private:
                float _valueToPos(float) const;
                float _posToValue(float) const;
                std::vector<Math::BBox2f> _getChildGeometry() const;
                std::vector<Math::BBox2f> _getHandleGeometry() const;

                DJV_PRIVATE();
            };

        } // namespace Layout
    } // namespace UI
} // namespace djv

