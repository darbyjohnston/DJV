// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

#include <djvAV/Pixel.h>

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
            void _init(const std::shared_ptr<Core::Context>&);
            ColorTypeWidget();

        public:
            ~ColorTypeWidget() override;

            static std::shared_ptr<ColorTypeWidget> create(const std::shared_ptr<Core::Context>&);

            AV::Image::Type getType() const;
            void setType(AV::Image::Type);
            void setTypeCallback(const std::function<void(AV::Image::Type)> &);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout &) override;
            void _layoutEvent(Core::Event::Layout &) override;

            void _initEvent(Core::Event::Init &) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        //! This class provides slider widgets for choosing a color.
        //!
        //! \todo Implement HSV.
        class ColorSliders : public Widget
        {
            DJV_NON_COPYABLE(ColorSliders);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ColorSliders();

        public:
            ~ColorSliders() override;

            static std::shared_ptr<ColorSliders> create(const std::shared_ptr<Core::Context>&);

            const AV::Image::Color & getColor() const;
            void setColor(const AV::Image::Color &);
            void setColorCallback(const std::function<void(const AV::Image::Color &)> &);
            
            bool hasHSV() const;
            void setHSV(bool);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout &) override;
            void _layoutEvent(Core::Event::Layout &) override;

            void _initEvent(Core::Event::Init &) override;

        private:
            void _widgetUpdate();
            void _colorUpdate();
            void _textUpdate();

            DJV_PRIVATE();
        };

        //! This class provides a color picker widget.
        class ColorPicker : public Widget
        {
            DJV_NON_COPYABLE(ColorPicker);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ColorPicker();

        public:
            ~ColorPicker() override;

            static std::shared_ptr<ColorPicker> create(const std::shared_ptr<Core::Context>&);

            const AV::Image::Color& getColor() const;
            void setColor(const AV::Image::Color&);
            void setColorCallback(const std::function<void(const AV::Image::Color&)>&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

        private:
            void _colorUpdate();

            DJV_PRIVATE();
        };

        //! This class provides a color swatch with a color picker.
        class ColorPickerSwatch : public Widget
        {
            DJV_NON_COPYABLE(ColorPickerSwatch);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ColorPickerSwatch();

        public:
            ~ColorPickerSwatch() override;

            static std::shared_ptr<ColorPickerSwatch> create(const std::shared_ptr<Core::Context>&);

            const AV::Image::Color& getColor() const;
            void setColor(const AV::Image::Color&);
            void setColorCallback(const std::function<void(const AV::Image::Color&)>&);

            MetricsRole getSwatchSizeRole() const;
            void setSwatchSizeRole(MetricsRole);

            void open();
            void close();

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

        private:
            void _colorUpdate();

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

