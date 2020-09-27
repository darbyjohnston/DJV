// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace System
    {
        namespace File
        {
            class Info;
    
        } // namespace File
    } // namespace System

    namespace Image
    {
        class Size;
    
    } // namespace Image

    namespace AV
    {
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
                void _init(SelectionType, const std::shared_ptr<System::Context>&);
                ItemView();

            public:
                ~ItemView() override;

                static std::shared_ptr<ItemView> create(SelectionType, const std::shared_ptr<System::Context>&);

                void setViewType(ViewType);
                void setThumbnailSize(const Image::Size&);
                void setSplit(const std::vector<float>&);

                void setItems(const std::vector<System::File::Info>&);

                std::set<size_t> getSelected() const;

                void setSelected(const std::set<size_t>&);
                void selectAll();
                void selectNone();
                void invertSelection();

                void setSelectedCallback(const std::function<void(const std::vector<System::File::Info>&)>&);
                void setSelectedCallback(const std::function<void(const std::set<size_t>&)>&);
                void setActivatedCallback(const std::function<void(const std::vector<System::File::Info>&)>&);
                void setActivatedCallback(const std::function<void(const std::set<size_t>&)>&);

                float getHeightForWidth(float) const override;

            protected:
                void _layoutEvent(System::Event::Layout&) override;
                void _clipEvent(System::Event::Clip&) override;
                void _paintEvent(System::Event::Paint&) override;
                void _pointerEnterEvent(System::Event::PointerEnter&) override;
                void _pointerLeaveEvent(System::Event::PointerLeave&) override;
                void _pointerMoveEvent(System::Event::PointerMove&) override;
                void _buttonPressEvent(System::Event::ButtonPress&) override;
                void _buttonReleaseEvent(System::Event::ButtonRelease&) override;
                void _keyPressEvent(System::Event::KeyPress&) override;
                void _keyReleaseEvent(System::Event::KeyRelease&) override;

                std::shared_ptr<ITooltipWidget> _createTooltip(const glm::vec2& pos) override;

                void _initEvent(System::Event::Init&) override;
                void _updateEvent(System::Event::Update&) override;

            private:
                std::vector<System::File::Info> _getSelectedItems(const std::set<size_t>&) const;

                std::string _getTooltip(const System::File::Info&) const;
                std::string _getTooltip(const System::File::Info&, const AV::IO::Info&) const;
                
                void _iconsUpdate();
                void _thumbnailsSizeUpdate();
                void _itemsUpdate();

                DJV_PRIVATE();
            };

        } // namespace Layout
    } // namespace UI
} // namespace djv

