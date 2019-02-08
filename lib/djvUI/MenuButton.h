// Copyright (c) 2017-2018 Darby Johnston
// All rights reserved.
//
// See LICENSE.txt for licensing information.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        namespace Button
        {
            //! This class provides a menu button widget.
            class Menu : public Widget
            {
                DJV_NON_COPYABLE(Menu);

            protected:
                void _init(Core::Context *);
                Menu();

            public:
                virtual ~Menu();

                static std::shared_ptr<Menu> create(Core::Context *);
                static std::shared_ptr<Menu> create(const std::string&, Core::Context *);
                static std::shared_ptr<Menu> create(const std::string&, const std::string& icon, Core::Context *);

                const std::string& getIcon() const;
                void setIcon(const std::string&);

                const std::string& getText() const;
                void setText(const std::string&);

                bool isChecked() const;
                void setChecked(bool);
                void setCheckedCallback(const std::function<void(bool)>&);

                bool hasBorder() const;
                void setBorder(bool);

            protected:
                void _preLayoutEvent(Core::Event::PreLayout &) override;
                void _layoutEvent(Core::Event::Layout &) override;
                void _paintEvent(Core::Event::Paint&) override;
                void _pointerEnterEvent(Core::Event::PointerEnter&) override;
                void _pointerLeaveEvent(Core::Event::PointerLeave&) override;
                void _pointerMoveEvent(Core::Event::PointerMove&) override;
                void _buttonPressEvent(Core::Event::ButtonPress&) override;

                void _updateEvent(Core::Event::Update& event) override;

            private:
                bool _isHovered() const;

				DJV_PRIVATE();
            };

        } // namespace Button
    } // namespace UI
} // namespace djv
