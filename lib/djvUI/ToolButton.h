// Copyright (c) 2017-2018 Darby Johnston
// All rights reserved.
//
// See LICENSE.txt for licensing information.

#pragma once

#include <djvUI/IButton.h>

namespace djv
{
    namespace UI
    {
        namespace Button
        {
            //! This class provides a tool button widget.
            class Tool : public IButton
            {
                DJV_NON_COPYABLE(Tool);

            protected:
                void _init(Core::Context *);
                Tool();

            public:
                virtual ~Tool();

                static std::shared_ptr<Tool> create(Core::Context *);
                static std::shared_ptr<Tool> create(const std::string&, Core::Context *);

                const std::string& getIcon() const;
                void setIcon(const std::string&);

                const Layout::Margin& getInsideMargin() const;
                void setInsideMargin(const Layout::Margin&);

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(Core::Event::PreLayout&) override;
                void _layoutEvent(Core::Event::Layout&) override;
                void _paintEvent(Core::Event::Paint&) override;

                void _updateEvent(Core::Event::Update&) override;

            private:
                struct Private;
                std::unique_ptr<Private> _p;
            };

        } // namespace Button
    } // namespace UI
} // namespace djv
