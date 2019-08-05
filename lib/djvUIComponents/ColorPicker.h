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

#include <djvAV/Pixel.h>

#include <djvCore/ISystem.h>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            class Color;
    
        } // Image
    } // AV

    namespace UI
    {
        //! This class provides a popup widget for choosing a color type.
        class ColorTypeWidget : public Widget
        {
            DJV_NON_COPYABLE(ColorTypeWidget);

        protected:
            void _init(Core::Context *);
            ColorTypeWidget();

        public:
            ~ColorTypeWidget() override;

            static std::shared_ptr<ColorTypeWidget> create(Core::Context *);

            AV::Image::Type getType() const;
            void setType(AV::Image::Type);
            void setTypeCallback(const std::function<void(AV::Image::Type)> &);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout &) override;
            void _layoutEvent(Core::Event::Layout &) override;

            void _localeEvent(Core::Event::Locale &) override;

        private:
            DJV_PRIVATE();

            void _widgetUpdate();
        };

        //! This class provides slider widgets for choosing a color in the RGB color space.
        class RGBColorSliders : public Widget
        {
            DJV_NON_COPYABLE(RGBColorSliders);

        protected:
            void _init(Core::Context *);
            RGBColorSliders();

        public:
            ~RGBColorSliders() override;

            static std::shared_ptr<RGBColorSliders> create(Core::Context *);

            const AV::Image::Color & getColor() const;
            void setColor(const AV::Image::Color &);
            void setColorCallback(const std::function<void(const AV::Image::Color &)> &);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout &) override;
            void _layoutEvent(Core::Event::Layout &) override;

            void _localeEvent(Core::Event::Locale &) override;

        private:
            DJV_PRIVATE();

            void _widgetUpdate();
            void _colorUpdate();
            void _textUpdate();
        };

        //! This class provides slider widgets for choosing a color in the HSV color space.
        //!
        //! \todo Implement the HSV color space.
        class HSVColorSliders : public Widget
        {
            DJV_NON_COPYABLE(HSVColorSliders);

        protected:
            void _init(Core::Context *);
            HSVColorSliders();

        public:
            ~HSVColorSliders() override;

            static std::shared_ptr<HSVColorSliders> create(Core::Context *);

            const AV::Image::Color & getColor() const;
            void setColor(const AV::Image::Color &);
            void setColorCallback(const std::function<void(const AV::Image::Color &)> &);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout &) override;
            void _layoutEvent(Core::Event::Layout &) override;

            void _localeEvent(Core::Event::Locale &) override;

        private:
            DJV_PRIVATE();

            void _widgetUpdate();
            void _colorUpdate();
            void _textUpdate();
        };

        //! This class provides a color picker widget.
        class ColorPicker : public Widget
        {
            DJV_NON_COPYABLE(ColorPicker);

        protected:
            void _init(Core::Context *);
            ColorPicker();

        public:
            ~ColorPicker() override;

            static std::shared_ptr<ColorPicker> create(Core::Context *);

            const AV::Image::Color & getColor() const;
            void setColor(const AV::Image::Color &);
            void setColorCallback(const std::function<void(const AV::Image::Color &)> &);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout &) override;
            void _layoutEvent(Core::Event::Layout &) override;

        private:
            DJV_PRIVATE();

            void _colorUpdate();
        };

        //! This class provides a global color picker dialog.
        class ColorPickerDialogSystem : public Core::ISystem
        {
            DJV_NON_COPYABLE(ColorPickerDialogSystem);

        protected:
            void _init(Core::Context*);
            ColorPickerDialogSystem();

        public:
            virtual ~ColorPickerDialogSystem();

            static std::shared_ptr<ColorPickerDialogSystem> create(Core::Context*);

            void colorPicker(
                const std::string& title,
                const AV::Image::Color&,
                const std::function<void(const AV::Image::Color&)>& callback);

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

