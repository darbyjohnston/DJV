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

#include <djvUICore/StyleSettings.h>

#include <djvCore/Animation.h>
#include <djvCore/Context.h>
#include <djvCore/Math.h>
#include <djvCore/Memory.h>
#include <djvCore/PicoJSON.h>
#include <djvCore/TextSystem.h>

#include <sstream>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UICore
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

        void StyleSettings::_init(const std::shared_ptr<Context>& context)
        {
            ISettings::_init("djv::UICore::StyleSettings", context);

            std::map<std::string, Palette> palettes;
            palettes[DJV_TEXT("Default")] = Palette();
            auto palette = Palette();
            palette.setColor(ColorRole::Background, AV::Image::Color(1.f, 1.f, 1.f));
            palette.setColor(ColorRole::Foreground, AV::Image::Color(.1f, .1f, .1f));
            palette.setColor(ColorRole::Trough, AV::Image::Color(.9f, .9f, .9f));
            palette.setColor(ColorRole::Button, AV::Image::Color(.8f, .8f, .8f));
            palette.setColor(ColorRole::Checked, AV::Image::Color(.3f, .5f, .8f));
            palette.setColor(ColorRole::CheckedForeground, AV::Image::Color(1.f, 1.f, 1.f));
            palette.setColor(ColorRole::Hover, AV::Image::Color(0.f, 0.f, 0.f, .05f));
            palette.setColor(ColorRole::Overlay, AV::Image::Color(0.f, 0.f, 0.f, .5f));
            palette.setColor(ColorRole::Border, AV::Image::Color(.9f, .9f, .9f));
            palettes[DJV_TEXT("Light")] = palette;

            std::map<std::string, Metrics> metrics;
            auto m = Metrics();
            m.setScale(1.f);
            metrics[DJV_TEXT("Default")] = m;
            m = Metrics();
            m.setScale(2.f);
            metrics[DJV_TEXT("Large")] = m;

            _p->palettes = MapSubject<std::string, Palette>::create(palettes);
            _p->currentPalette = ValueSubject<Palette>::create(palettes["Default"]);
            _p->currentPaletteName = ValueSubject<std::string>::create("Default");
            _p->metrics = MapSubject<std::string, Metrics>::create(metrics);
            _p->currentMetrics = ValueSubject<Metrics>::create(metrics["Default"]);
            _p->currentMetricsName = ValueSubject<std::string>::create(DJV_TEXT("Default"));

            _load();
        }

        StyleSettings::StyleSettings() :
            _p(new Private)
        {}

        StyleSettings::~StyleSettings()
        {}

        std::shared_ptr<StyleSettings> StyleSettings::create(const std::shared_ptr<Context>& context)
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
            if (_p->currentPaletteName->setIfChanged(name))
            {
                if (auto context = getContext().lock())
                {
                    context->getSystemT<Style>()->setPalette(_p->palettes->getItem(name));
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
            if (_p->currentMetricsName->setIfChanged(name))
            {
                if (auto context = getContext().lock())
                {
                    context->getSystemT<Style>()->setMetrics(_p->metrics->getItem(name));
                }
            }
        }

        void StyleSettings::load(const picojson::value& value)
        {
            if (value.is<picojson::object>())
            {
                const auto& object = value.get<picojson::object>();
                _read("Palettes", object, _p->palettes);
                _read("CurrentPalette", object, _p->currentPaletteName);
                _p->currentPalette->setIfChanged(_p->palettes->getItem(_p->currentPaletteName->get()));
                _read("Metrics", object, _p->metrics);
                _read("CurrentMetrics", object, _p->currentMetricsName);
                _p->currentMetrics->setIfChanged(_p->metrics->getItem(_p->currentMetricsName->get()));
            }
        }

        picojson::value StyleSettings::save()
        {
            picojson::value out(picojson::object_type, true);
            auto& object = out.get<picojson::object>();
            _write("Palettes", _p->palettes->get(), object);
            _write("CurrentPalette", _p->currentPaletteName->get(), object);
            _write("Metrics", _p->metrics->get(), object);
            _write("CurrentMetrics", _p->currentMetricsName->get(), object);
            return out;
        }

    } // namespace UICore
} // namespace djv

