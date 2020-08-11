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
            //! This class provides a splitter widget.
            class Splitter : public Widget
            {
                DJV_NON_COPYABLE(Splitter);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                Splitter();

            public:
                ~Splitter() override;

                static std::shared_ptr<Splitter> create(Orientation, const std::shared_ptr<Core::Context>&);

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
                void _preLayoutEvent(Core::Event::PreLayout&) override;
                void _layoutEvent(Core::Event::Layout&) override;
                void _paintEvent(Core::Event::Paint&) override;
                void _pointerLeaveEvent(Core::Event::PointerLeave&) override;
                void _pointerMoveEvent(Core::Event::PointerMove&) override;
                void _buttonPressEvent(Core::Event::ButtonPress&) override;
                void _buttonReleaseEvent(Core::Event::ButtonRelease&) override;

                void _initEvent(Core::Event::Init&) override;

            private:
                float _valueToPos(float) const;
                float _posToValue(float) const;
                std::vector<Core::BBox2f> _getChildGeometry() const;
                std::vector<Core::BBox2f> _getHandleGeometry() const;

                DJV_PRIVATE();
            };

        } // namespace Layout
    } // namespace UI
} // namespace djv

