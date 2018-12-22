// Copyright (c) 2017-2018 Darby Johnston
// All rights reserved.
//
// See LICENSE.txt for licensing information.

#pragma once

#include <djvUI/IButton.h>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            class Path;

        } // namespace FileSystem
    } // namespace Core

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
                static std::shared_ptr<Tool> create(const Core::FileSystem::Path&, Core::Context *);

                const Core::FileSystem::Path& getIcon() const;
                void setIcon(const Core::FileSystem::Path&);

                const Layout::Margin& getInsideMargin() const;
                void setInsideMargin(const Layout::Margin&);

                float getHeightForWidth(float) const override;

                void updateEvent(Core::Event::Update&) override;
                void preLayoutEvent(Core::Event::PreLayout&) override;
                void layoutEvent(Core::Event::Layout&) override;
                void paintEvent(Core::Event::Paint&) override;

            private:
                struct Private;
                std::unique_ptr<Private> _p;
            };

        } // namespace Button
    } // namespace UI
} // namespace djv
