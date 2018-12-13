//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvUI/StyleSettings.h>

#include <djvUI/StyleJSON.h>

#include <djvCore/Animation.h>
#include <djvCore/Context.h>
#include <djvCore/Math.h>
#include <djvCore/Memory.h>
#include <djvCore/TextSystem.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct StyleSettings::Private
        {
            std::shared_ptr<MapSubject<std::string, Palette> > palettes;
            std::shared_ptr<ValueSubject<Palette> > currentPalette;
            std::shared_ptr<ValueSubject<std::string> > currentPaletteName;
            std::shared_ptr<MapSubject<std::string, Metrics> > metrics;
            std::shared_ptr<ValueSubject<Metrics> > currentMetrics;
            std::shared_ptr<ValueSubject<std::string> > currentMetricsName;
        };

        void StyleSettings::_init(Context * context)
        {
            ISettings::_init("djv::UI::StyleSettings", context);

            std::map<std::string, Palette> palettes;
            palettes[DJV_TEXT("Default")] = Palette();
            auto palette = Palette();
            palette.setColor(ColorRole::Background, AV::Color(1.f, 1.f, 1.f));
            palette.setColor(ColorRole::Foreground, AV::Color(.1f, .1f, .1f));
            palette.setColor(ColorRole::Trough, AV::Color(.9f, .9f, .9f));
            palette.setColor(ColorRole::Button, AV::Color(.8f, .8f, .8f));
            palette.setColor(ColorRole::Checked, AV::Color(.3f, .5f, .8f));
            palette.setColor(ColorRole::CheckedForeground, AV::Color(1.f, 1.f, 1.f));
            palette.setColor(ColorRole::Hover, AV::Color(0.f, 0.f, 0.f, .05f));
            palette.setColor(ColorRole::Overlay, AV::Color(0.f, 0.f, 0.f, .5f));
            palette.setColor(ColorRole::Border, AV::Color(.9f, .9f, .9f));
            palettes[DJV_TEXT("Light")] = palette;

            std::map<std::string, Metrics> metrics;
            auto m = Metrics();
            m.setScale(1.f);
            metrics[DJV_TEXT("Default")] = m;
            m = Metrics();
            m.setScale(2.f);
            metrics[DJV_TEXT("Large")] = m;

            DJV_PRIVATE_PTR();
            p.palettes = MapSubject<std::string, Palette>::create(palettes);
            p.currentPalette = ValueSubject<Palette>::create(palettes["Default"]);
            p.currentPaletteName = ValueSubject<std::string>::create("Default");
            p.metrics = MapSubject<std::string, Metrics>::create(metrics);
            p.currentMetrics = ValueSubject<Metrics>::create(metrics["Default"]);
            p.currentMetricsName = ValueSubject<std::string>::create(DJV_TEXT("Default"));

            _load();
        }

        StyleSettings::StyleSettings() :
            _p(new Private)
        {}

        StyleSettings::~StyleSettings()
        {}

        std::shared_ptr<StyleSettings> StyleSettings::create(Context * context)
        {
            auto out = std::shared_ptr<StyleSettings>(new StyleSettings);
            out->_init(context);
            return out;
        }

        std::shared_ptr<IMapSubject<std::string, Palette> > StyleSettings::getPalettes() const
        {
            return _p->palettes;
        }

        std::shared_ptr<IValueSubject<Palette> > StyleSettings::getCurrentPalette() const
        {
            return _p->currentPalette;
        }

        std::shared_ptr<IValueSubject<std::string> > StyleSettings::getCurrentPaletteName() const
        {
            return _p->currentPaletteName;
        }

        void StyleSettings::setCurrentPalette(const std::string& name)
        {
            DJV_PRIVATE_PTR();
            if (p.currentPaletteName->setIfChanged(name))
            {
                if (auto style = getContext()->getSystemT<Style>().lock())
                {
                    style->setPalette(p.palettes->getItem(name));
                }
            }
        }

        std::shared_ptr<IMapSubject<std::string, Metrics> > StyleSettings::getMetrics() const
        {
            return _p->metrics;
        }

        std::shared_ptr<IValueSubject<Metrics> > StyleSettings::getCurrentMetrics() const
        {
            return _p->currentMetrics;
        }

        std::shared_ptr<IValueSubject<std::string> > StyleSettings::getCurrentMetricsName() const
        {
            return _p->currentMetricsName;
        }

        void StyleSettings::setCurrentMetrics(const std::string& name)
        {
            DJV_PRIVATE_PTR();
            if (p.currentMetricsName->setIfChanged(name))
            {
                if (auto style = getContext()->getSystemT<Style>().lock())
                {
                    style->setMetrics(p.metrics->getItem(name));
                }
            }
        }

        void StyleSettings::load(const picojson::value& value)
        {
            if (value.is<picojson::object>())
            {
                DJV_PRIVATE_PTR();
                const auto& object = value.get<picojson::object>();
                _read("Palettes", object, p.palettes);
                _read("CurrentPalette", object, p.currentPaletteName);
                p.currentPalette->setIfChanged(p.palettes->getItem(p.currentPaletteName->get()));
                _read("Metrics", object, p.metrics);
                _read("CurrentMetrics", object, p.currentMetricsName);
                p.currentMetrics->setIfChanged(p.metrics->getItem(p.currentMetricsName->get()));
            }
        }

        picojson::value StyleSettings::save()
        {
            DJV_PRIVATE_PTR();
            picojson::value out(picojson::object_type, true);
            auto& object = out.get<picojson::object>();
            _write("Palettes", p.palettes->get(), object);
            _write("CurrentPalette", p.currentPaletteName->get(), object);
            _write("Metrics", p.metrics->get(), object);
            _write("CurrentMetrics", p.currentMetricsName->get(), object);
            return out;
        }

    } // namespace UI
} // namespace djv

