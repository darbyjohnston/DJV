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
        namespace Settings
        {
            struct Style::Private
            {
                std::shared_ptr<MapSubject<std::string, UI::Style::Palette> > palettes;
                std::shared_ptr<ValueSubject<UI::Style::Palette> > currentPalette;
                std::shared_ptr<ValueSubject<std::string> > currentPaletteName;
                std::shared_ptr<MapSubject<std::string, UI::Style::Metrics> > metrics;
                std::shared_ptr<ValueSubject<UI::Style::Metrics> > currentMetrics;
                std::shared_ptr<ValueSubject<std::string> > currentMetricsName;
            };

            void Style::_init(Context * context)
            {
                ISettings::_init("djv::UI::Settings::Style", context);

                std::map<std::string, UI::Style::Palette> palettes;
                palettes[DJV_TEXT("Default")] = UI::Style::Palette();
                auto palette = UI::Style::Palette();
                palette.setColor(UI::Style::ColorRole::Background, AV::Image::Color(1.f, 1.f, 1.f));
                palette.setColor(UI::Style::ColorRole::Foreground, AV::Image::Color(.1f, .1f, .1f));
                palette.setColor(UI::Style::ColorRole::Trough, AV::Image::Color(.9f, .9f, .9f));
                palette.setColor(UI::Style::ColorRole::Button, AV::Image::Color(.8f, .8f, .8f));
                palette.setColor(UI::Style::ColorRole::Checked, AV::Image::Color(.3f, .5f, .8f));
                palette.setColor(UI::Style::ColorRole::CheckedForeground, AV::Image::Color(1.f, 1.f, 1.f));
                palette.setColor(UI::Style::ColorRole::Hover, AV::Image::Color(0.f, 0.f, 0.f, .05f));
                palette.setColor(UI::Style::ColorRole::Overlay, AV::Image::Color(0.f, 0.f, 0.f, .5f));
                palette.setColor(UI::Style::ColorRole::Border, AV::Image::Color(.9f, .9f, .9f));
                palettes[DJV_TEXT("Light")] = palette;

                std::map<std::string, UI::Style::Metrics> metrics;
                auto m = UI::Style::Metrics();
                m.setScale(1.f);
                metrics[DJV_TEXT("Default")] = m;
                m = UI::Style::Metrics();
                m.setScale(2.f);
                metrics[DJV_TEXT("Large")] = m;

                DJV_PRIVATE_PTR();
                p.palettes = MapSubject<std::string, UI::Style::Palette>::create(palettes);
                p.currentPalette = ValueSubject<UI::Style::Palette>::create(palettes["Default"]);
                p.currentPaletteName = ValueSubject<std::string>::create("Default");
                p.metrics = MapSubject<std::string, UI::Style::Metrics>::create(metrics);
                p.currentMetrics = ValueSubject<UI::Style::Metrics>::create(metrics["Default"]);
                p.currentMetricsName = ValueSubject<std::string>::create(DJV_TEXT("Default"));

                _load();
            }

            Style::Style() :
                _p(new Private)
            {}

            Style::~Style()
            {}

            std::shared_ptr<Style> Style::create(Context * context)
            {
                auto out = std::shared_ptr<Style>(new Style);
                out->_init(context);
                return out;
            }

            std::shared_ptr<IMapSubject<std::string, UI::Style::Palette> > Style::getPalettes() const
            {
                return _p->palettes;
            }

            std::shared_ptr<IValueSubject<UI::Style::Palette> > Style::getCurrentPalette() const
            {
                return _p->currentPalette;
            }

            std::shared_ptr<IValueSubject<std::string> > Style::getCurrentPaletteName() const
            {
                return _p->currentPaletteName;
            }

            void Style::setCurrentPalette(const std::string& name)
            {
                DJV_PRIVATE_PTR();
                if (p.currentPaletteName->setIfChanged(name))
                {
                    if (auto style = getContext()->getSystemT<UI::Style::Style>().lock())
                    {
                        style->setPalette(p.palettes->getItem(name));
                    }
                }
            }

            std::shared_ptr<IMapSubject<std::string, UI::Style::Metrics> > Style::getMetrics() const
            {
                return _p->metrics;
            }

            std::shared_ptr<IValueSubject<UI::Style::Metrics> > Style::getCurrentMetrics() const
            {
                return _p->currentMetrics;
            }

            std::shared_ptr<IValueSubject<std::string> > Style::getCurrentMetricsName() const
            {
                return _p->currentMetricsName;
            }

            void Style::setCurrentMetrics(const std::string& name)
            {
                DJV_PRIVATE_PTR();
                if (p.currentMetricsName->setIfChanged(name))
                {
                    if (auto style = getContext()->getSystemT<UI::Style::Style>().lock())
                    {
                        style->setMetrics(p.metrics->getItem(name));
                    }
                }
            }

            void Style::load(const picojson::value& value)
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

            picojson::value Style::save()
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

        } // namespace Settings
    } // namespace UI
} // namespace djv

