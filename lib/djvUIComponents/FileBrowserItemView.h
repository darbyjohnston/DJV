//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

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
            struct Size;
        
        } // namespace Image

        namespace IO
        {
            struct Info;

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
                void _init(Core::Context *);
                ItemView();

            public:
                ~ItemView() override;

                static std::shared_ptr<ItemView> create(Core::Context *);

                void setViewType(ViewType);
                void setThumbnailSize(const AV::Image::Size&);
                void setSplit(const std::vector<float> &);
                void setItems(const std::vector<Core::FileSystem::FileInfo> &);
                void setCallback(const std::function<void(const Core::FileSystem::FileInfo &)> &);

                float getHeightForWidth(float) const override;

            protected:
                void _styleEvent(Core::Event::Style &) override;
                void _preLayoutEvent(Core::Event::PreLayout &) override;
                void _layoutEvent(Core::Event::Layout &) override;
                void _clipEvent(Core::Event::Clip &) override;
                void _paintEvent(Core::Event::Paint &) override;
                void _pointerEnterEvent(Core::Event::PointerEnter &) override;
                void _pointerLeaveEvent(Core::Event::PointerLeave &) override;
                void _pointerMoveEvent(Core::Event::PointerMove &) override;
                void _buttonPressEvent(Core::Event::ButtonPress &) override;
                void _buttonReleaseEvent(Core::Event::ButtonRelease &) override;

                std::shared_ptr<Widget> _createTooltip(const glm::vec2 & pos) override;

                void _updateEvent(Core::Event::Update &) override;
                void _localeEvent(Core::Event::Locale &) override;

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

