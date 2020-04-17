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
            //! This class provides a bellows widget.
            class Bellows : public Widget
            {
                DJV_NON_COPYABLE(Bellows);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                Bellows();

            public:
                virtual ~Bellows();

                static std::shared_ptr<Bellows> create(const std::shared_ptr<Core::Context>&);
                
                const std::string & getText() const;
                void setText(const std::string &);

                bool isOpen() const;
                void setOpen(bool);
                void open();
                void close();
                void setOpenCallback(const std::function<void(bool)> &);

                float getHeightForWidth(float) const override;

                void addChild(const std::shared_ptr<IObject> &) override;
                void removeChild(const std::shared_ptr<IObject> &) override;
                void clearChildren() override;

            protected:
                void _preLayoutEvent(Core::Event::PreLayout &) override;
                void _layoutEvent(Core::Event::Layout &) override;

            private:
                void _childrenUpdate();

                DJV_PRIVATE();
            };

        } // namespace Layout

        using Layout::Bellows;

    } // namespace UI
} // namespace djv

