// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            class FileInfo;
    
        } // namespace FileSystem
    } // namespace Core

    namespace AV
    {
        namespace Image
        {
            class Size;
        
        } // namespace Image

        namespace IO
        {
            class Info;

        } // namespace IO
    } // namespace AV

    namespace UI
    {
        namespace FileBrowser
        {
            //! This class provides a file browser item view.
            //!
            //! \todo Elide names which are too long.
            //! \todo Show an animated spinner for thumbnails that are loading.
            //! \todo Show an error icon for thumbnails that failed to load.
            class ItemView : public UI::Widget
            {
                DJV_NON_COPYABLE(ItemView);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                ItemView();

            public:
                ~ItemView() override;

                static std::shared_ptr<ItemView> create(const std::shared_ptr<Core::Context>&);

                void setViewType(ViewType);
                void setThumbnailSize(const AV::Image::Size&);
                void setSplit(const std::vector<float> &);
                void setItems(const std::vector<Core::FileSystem::FileInfo> &);
                void setCallback(const std::function<void(const Core::FileSystem::FileInfo &)> &);

                float getHeightForWidth(float) const override;

            protected:
                void _layoutEvent(Core::Event::Layout &) override;
                void _clipEvent(Core::Event::Clip &) override;
                void _paintEvent(Core::Event::Paint &) override;
                void _pointerEnterEvent(Core::Event::PointerEnter &) override;
                void _pointerLeaveEvent(Core::Event::PointerLeave &) override;
                void _pointerMoveEvent(Core::Event::PointerMove &) override;
                void _buttonPressEvent(Core::Event::ButtonPress &) override;
                void _buttonReleaseEvent(Core::Event::ButtonRelease &) override;

                std::shared_ptr<ITooltipWidget> _createTooltip(const glm::vec2 & pos) override;

                void _initEvent(Core::Event::Init &) override;
                void _updateEvent(Core::Event::Update &) override;

            private:
                std::string _getTooltip(const Core::FileSystem::FileInfo &) const;
                std::string _getTooltip(const Core::FileSystem::FileInfo &, const AV::IO::Info &) const;
                
                void _iconsUpdate();
                void _thumbnailsSizeUpdate();
                void _itemsUpdate();

                DJV_PRIVATE();
            };

        } // namespace Layout
    } // namespace UI
} // namespace djv

