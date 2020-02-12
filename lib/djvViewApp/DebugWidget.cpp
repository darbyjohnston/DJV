//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvViewApp/DebugWidget.h>

#include <djvViewApp/FileSystem.h>
#include <djvViewApp/Media.h>

#include <djvUIComponents/LineGraphWidget.h>
#include <djvUIComponents/ThermometerWidget.h>

#include <djvUI/Bellows.h>
#include <djvUI/EventSystem.h>
#include <djvUI/IconSystem.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>

#include <djvAV/IO.h>
#include <djvAV/FontSystem.h>
#include <djvAV/Render2D.h>
#include <djvAV/ThumbnailSystem.h>

#include <djvCore/Context.h>
#include <djvCore/Timer.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        namespace
        {
            class IDebugWidget : public UI::Widget
            {
            public:
                virtual ~IDebugWidget() = 0;
                
            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

                void _initEvent(Event::Init&) override;

                virtual void _widgetUpdate() = 0;

                std::map<std::string, std::shared_ptr<UI::Label> > _labels;
                std::map<std::string, std::shared_ptr<UI::LineGraphWidget> > _lineGraphs;
                std::map<std::string, std::shared_ptr<UI::ThermometerWidget> > _thermometerWidgets;
                std::shared_ptr<UI::VerticalLayout> _layout;
                std::shared_ptr<Time::Timer> _timer;
            };
            
            IDebugWidget::~IDebugWidget()
            {}

            void IDebugWidget::_preLayoutEvent(Event::PreLayout&)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void IDebugWidget::_layoutEvent(Event::Layout&)
            {
                _layout->setGeometry(getGeometry());
            }

            void IDebugWidget::_initEvent(Event::Init& event)
            {
                Widget::_initEvent(event);
                _widgetUpdate();
            }

            class GeneralDebugWidget : public IDebugWidget
            {
                DJV_NON_COPYABLE(GeneralDebugWidget);

            protected:
                void _init(const std::shared_ptr<Context>&);
                GeneralDebugWidget();

            public:
                static std::shared_ptr<GeneralDebugWidget> create(const std::shared_ptr<Context>&);

            protected:
                void _widgetUpdate() override;
            };

            void GeneralDebugWidget::_init(const std::shared_ptr<Context>& context)
            {
                IDebugWidget::_init(context);

                setClassName("djv::ViewApp::GeneralDebugWidget");

                _labels["FPS"] = UI::Label::create(context);
                _labels["FPSValue"] = UI::Label::create(context);
                _labels["FPSValue"]->setFont(AV::Font::familyMono);
                _lineGraphs["FPS"] = UI::LineGraphWidget::create(context);
                _lineGraphs["FPS"]->setPrecision(2);

                _labels["TotalSystemTime"] = UI::Label::create(context);
                _labels["TotalSystemTimeValue"] = UI::Label::create(context);
                _lineGraphs["TotalSystemTime"] = UI::LineGraphWidget::create(context);
                _lineGraphs["TotalSystemTime"]->setPrecision(0);

                _labels["TopSystemTime"] = UI::Label::create(context);
                _labels["TopSystemTimeValue"] = UI::Label::create(context);
                _lineGraphs["TopSystemTime"] = UI::LineGraphWidget::create(context);
                _lineGraphs["TopSystemTime"]->setPrecision(0);

                _labels["ObjectCount"] = UI::Label::create(context);
                _labels["ObjectCountValue"] = UI::Label::create(context);
                _labels["ObjectCountValue"]->setFont(AV::Font::familyMono);
                _lineGraphs["ObjectCount"] = UI::LineGraphWidget::create(context);
                _lineGraphs["ObjectCount"]->setPrecision(0);

                _labels["WidgetCount"] = UI::Label::create(context);
                _labels["WidgetCountValue"] = UI::Label::create(context);
                _labels["WidgetCountValue"]->setFont(AV::Font::familyMono);
                _lineGraphs["WidgetCount"] = UI::LineGraphWidget::create(context);
                _lineGraphs["WidgetCount"]->setPrecision(0);

                _labels["Hover"] = UI::Label::create(context);
                _labels["Grab"] = UI::Label::create(context);
                _labels["KeyGrab"] = UI::Label::create(context);
                _labels["TextFocus"] = UI::Label::create(context);

                _labels["GlyphCache"] = UI::Label::create(context);
                _labels["GlyphCacheValue"] = UI::Label::create(context);
                _labels["GlyphCacheValue"]->setFont(AV::Font::familyMono);
                _thermometerWidgets["GlyphCache"] = UI::ThermometerWidget::create(context);

                _labels["ThumbnailInfoCache"] = UI::Label::create(context);
                _labels["ThumbnailInfoCacheValue"] = UI::Label::create(context);
                _labels["ThumbnailInfoCacheValue"]->setFont(AV::Font::familyMono);
                _thermometerWidgets["ThumbnailInfoCache"] = UI::ThermometerWidget::create(context);

                _labels["ThumbnailImageCache"] = UI::Label::create(context);
                _labels["ThumbnailImageCacheValue"] = UI::Label::create(context);
                _labels["ThumbnailInfoCacheValue"]->setFont(AV::Font::familyMono);
                _thermometerWidgets["ThumbnailImageCache"] = UI::ThermometerWidget::create(context);

                _labels["IconCache"] = UI::Label::create(context);
                _labels["IconCacheValue"] = UI::Label::create(context);
                _labels["IconCacheValue"]->setFont(AV::Font::familyMono);
                _thermometerWidgets["IconCache"] = UI::ThermometerWidget::create(context);

                for (auto& i : _labels)
                {
                    i.second->setTextHAlign(UI::TextHAlign::Left);
                }

                _layout = UI::VerticalLayout::create(context);
                _layout->setMargin(UI::Layout::Margin(UI::MetricsRole::Margin));
                auto hLayout = UI::HorizontalLayout::create(context);
                hLayout->addChild(_labels["FPS"]);
                hLayout->addChild(_labels["FPSValue"]);
                _layout->addChild(hLayout);
                _layout->addChild(_lineGraphs["FPS"]);
                hLayout = UI::HorizontalLayout::create(context);
                hLayout->addChild(_labels["TotalSystemTime"]);
                hLayout->addChild(_labels["TotalSystemTimeValue"]);
                _layout->addChild(hLayout);
                _layout->addChild(_lineGraphs["TotalSystemTime"]);
                hLayout = UI::HorizontalLayout::create(context);
                hLayout->addChild(_labels["TopSystemTime"]);
                hLayout->addChild(_labels["TopSystemTimeValue"]);
                _layout->addChild(hLayout);
                _layout->addChild(_lineGraphs["TopSystemTime"]);
                hLayout = UI::HorizontalLayout::create(context);
                hLayout->addChild(_labels["ObjectCount"]);
                hLayout->addChild(_labels["ObjectCountValue"]);
                _layout->addChild(hLayout);
                _layout->addChild(_lineGraphs["ObjectCount"]);
                hLayout = UI::HorizontalLayout::create(context);
                hLayout->addChild(_labels["WidgetCount"]);
                hLayout->addChild(_labels["WidgetCountValue"]);
                _layout->addChild(hLayout);
                _layout->addChild(_lineGraphs["WidgetCount"]);
                _layout->addChild(_labels["Hover"]);
                _layout->addChild(_labels["Grab"]);
                _layout->addChild(_labels["KeyGrab"]);
                _layout->addChild(_labels["TextFocus"]);
                hLayout = UI::HorizontalLayout::create(context);
                hLayout->addChild(_labels["GlyphCache"]);
                hLayout->addChild(_labels["GlyphCacheValue"]);
                _layout->addChild(hLayout);
                _layout->addChild(_thermometerWidgets["GlyphCache"]);
                hLayout = UI::HorizontalLayout::create(context);
                hLayout->addChild(_labels["ThumbnailInfoCache"]);
                hLayout->addChild(_labels["ThumbnailInfoCacheValue"]);
                _layout->addChild(hLayout);
                _layout->addChild(_thermometerWidgets["ThumbnailInfoCache"]);
                hLayout = UI::HorizontalLayout::create(context);
                hLayout->addChild(_labels["ThumbnailImageCache"]);
                hLayout->addChild(_labels["ThumbnailImageCacheValue"]);
                _layout->addChild(hLayout);
                _layout->addChild(_thermometerWidgets["ThumbnailImageCache"]);
                hLayout = UI::HorizontalLayout::create(context);
                hLayout->addChild(_labels["IconCache"]);
                hLayout->addChild(_labels["IconCacheValue"]);
                _layout->addChild(hLayout);
                _layout->addChild(_thermometerWidgets["IconCache"]);
                addChild(_layout);

                _timer = Time::Timer::create(context);
                _timer->setRepeating(true);
                auto weak = std::weak_ptr<GeneralDebugWidget>(std::dynamic_pointer_cast<GeneralDebugWidget>(shared_from_this()));
                _timer->start(
                    Core::Time::getTime(Core::Time::TimerValue::Medium),
                    [weak](const std::chrono::steady_clock::time_point&, const Time::Unit&)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_widgetUpdate();
                    }
                });
            }

            GeneralDebugWidget::GeneralDebugWidget()
            {}

            std::shared_ptr<GeneralDebugWidget> GeneralDebugWidget::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<GeneralDebugWidget>(new GeneralDebugWidget);
                out->_init(context);
                return out;
            }

            void GeneralDebugWidget::_widgetUpdate()
            {
                if (auto context = getContext().lock())
                {
                    const float fps = context->getFPSAverage();
                    const auto& systemTickTimes = context->getSystemTickTimes();
                    Time::Unit totalSystemTime = Time::Unit::zero();
                    for (const auto& i : systemTickTimes)
                    {
                        totalSystemTime += i.second;
                    }
                    std::string topSystemTime;
                    Time::Unit topSystemTimeValue = Time::Unit::zero();
                    if (systemTickTimes.size())
                    {
                        auto i = systemTickTimes.begin();
                        topSystemTime = i->first;
                        topSystemTimeValue = i->second;
                    }
                    const size_t objectCount = IObject::getGlobalObjectCount();
                    const size_t widgetCount = UI::Widget::getGlobalWidgetCount();
                    auto eventSystem = context->getSystemT<UI::EventSystem>();
                    auto fontSystem = context->getSystemT<AV::Font::System>();
                    const float glyphCachePercentage = fontSystem->getGlyphCachePercentage();
                    auto thumbnailSystem = context->getSystemT<AV::ThumbnailSystem>();
                    const float thumbnailInfoCachePercentage = thumbnailSystem->getInfoCachePercentage();
                    const float thumbnailImageCachePercentage = thumbnailSystem->getImageCachePercentage();
                    auto iconSystem = context->getSystemT<UI::IconSystem>();
                    const float iconCachePercentage = iconSystem->getCachePercentage();

                    _lineGraphs["FPS"]->addSample(fps);
                    _lineGraphs["TotalSystemTime"]->addSample(totalSystemTime.count());
                    _lineGraphs["TopSystemTime"]->addSample(topSystemTimeValue.count());
                    _lineGraphs["ObjectCount"]->addSample(objectCount);
                    _lineGraphs["WidgetCount"]->addSample(widgetCount);
                    _thermometerWidgets["ThumbnailInfoCache"]->setPercentage(thumbnailInfoCachePercentage);
                    _thermometerWidgets["ThumbnailImageCache"]->setPercentage(thumbnailImageCachePercentage);
                    _thermometerWidgets["IconCache"]->setPercentage(iconCachePercentage);
                    _thermometerWidgets["GlyphCache"]->setPercentage(glyphCachePercentage);

                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("FPS")) << ":";
                        _labels["FPS"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss.precision(2);
                        ss << std::fixed << fps;
                        _labels["FPSValue"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("Total system time")) << ":";
                        _labels["TotalSystemTime"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << std::fixed << totalSystemTime.count();
                        _labels["TotalSystemTimeValue"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("Top system time")) << ":";
                        _labels["TopSystemTime"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << std::fixed << topSystemTime << ", " << topSystemTimeValue.count();
                        _labels["TopSystemTimeValue"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("Object count")) << ":";
                        _labels["ObjectCount"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << objectCount;
                        _labels["ObjectCountValue"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("Widget count")) << ":";
                        _labels["WidgetCount"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << widgetCount;
                        _labels["WidgetCountValue"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        auto object = eventSystem->observeHover()->get();
                        ss << _getText(DJV_TEXT("Hover")) << ": " << (object ? object->getClassName() : _getText(DJV_TEXT("None")));
                        _labels["Hover"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        auto object = eventSystem->observeGrab()->get();
                        ss << _getText(DJV_TEXT("Grab")) << ": " << (object ? object->getClassName() : _getText(DJV_TEXT("None")));
                        _labels["Grab"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        auto object = eventSystem->observeKeyGrab()->get();
                        ss << _getText(DJV_TEXT("Key grab")) << ": " << (object ? object->getClassName() : _getText(DJV_TEXT("None")));
                        _labels["KeyGrab"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        std::string name;
                        if (auto object = eventSystem->getTextFocus().lock())
                        {
                            name = object->getClassName();
                        }
                        else
                        {
                            name = _getText(DJV_TEXT("None"));
                        }
                        ss << _getText(DJV_TEXT("Text focus")) << ": " << name;
                        _labels["TextFocus"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("Font system glyph cache")) << ":";
                        _labels["GlyphCache"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss.precision(2);
                        ss << std::fixed << glyphCachePercentage << "%";
                        _labels["GlyphCacheValue"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("Thumbnail system information cache")) << ":";
                        _labels["ThumbnailInfoCache"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss.precision(2);
                        ss << std::fixed << thumbnailInfoCachePercentage << "%";
                        _labels["ThumbnailInfoCacheValue"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("Thumbnail system image cache")) << ":";
                        _labels["ThumbnailImageCache"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss.precision(2);
                        ss << std::fixed << thumbnailImageCachePercentage << "%";
                        _labels["ThumbnailImageCacheValue"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("Icon system cache")) << ":";
                        _labels["IconCache"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss.precision(2);
                        ss << std::fixed << iconCachePercentage << "%";
                        _labels["IconCacheValue"]->setText(ss.str());
                    }
                }
            }

            class RenderDebugWidget : public IDebugWidget
            {
                DJV_NON_COPYABLE(RenderDebugWidget);

            protected:
                void _init(const std::shared_ptr<Context>&);
                RenderDebugWidget();

            public:
                static std::shared_ptr<RenderDebugWidget> create(const std::shared_ptr<Context>&);

            protected:
                void _widgetUpdate() override;
            };

            void RenderDebugWidget::_init(const std::shared_ptr<Context>& context)
            {
                IDebugWidget::_init(context);

                setClassName("djv::ViewApp::RenderDebugWidget");

                _labels["TextureAtlas"] = UI::Label::create(context);
                _labels["TextureAtlasValue"] = UI::Label::create(context);
                _labels["TextureAtlasValue"]->setFont(AV::Font::familyMono);
                _thermometerWidgets["TextureAtlas"] = UI::ThermometerWidget::create(context);

                _labels["DynamicTextureCount"] = UI::Label::create(context);
                _labels["DynamicTextureCountValue"] = UI::Label::create(context);
                _labels["DynamicTextureCountValue"]->setFont(AV::Font::familyMono);
                _lineGraphs["DynamicTextureCount"] = UI::LineGraphWidget::create(context);
                _lineGraphs["DynamicTextureCount"]->setPrecision(0);

                _labels["VBOSize"] = UI::Label::create(context);
                _labels["VBOSizeValue"] = UI::Label::create(context);
                _labels["VBOSizeValue"]->setFont(AV::Font::familyMono);
                _lineGraphs["VBOSize"] = UI::LineGraphWidget::create(context);
                _lineGraphs["VBOSize"]->setPrecision(0);

                for (auto& i : _labels)
                {
                    i.second->setTextHAlign(UI::TextHAlign::Left);
                }

                _layout = UI::VerticalLayout::create(context);
                _layout->setMargin(UI::Layout::Margin(UI::MetricsRole::Margin));
                auto hLayout = UI::HorizontalLayout::create(context);
                hLayout->addChild(_labels["TextureAtlas"]);
                hLayout->addChild(_labels["TextureAtlasValue"]);
                _layout->addChild(hLayout);
                _layout->addChild(_thermometerWidgets["TextureAtlas"]);
                hLayout = UI::HorizontalLayout::create(context);
                hLayout->addChild(_labels["DynamicTextureCount"]);
                hLayout->addChild(_labels["DynamicTextureCountValue"]);
                _layout->addChild(hLayout);
                _layout->addChild(_lineGraphs["DynamicTextureCount"]);
                hLayout = UI::HorizontalLayout::create(context);
                hLayout->addChild(_labels["VBOSize"]);
                hLayout->addChild(_labels["VBOSizeValue"]);
                _layout->addChild(hLayout);
                _layout->addChild(_lineGraphs["VBOSize"]);
                addChild(_layout);

                _timer = Time::Timer::create(context);
                _timer->setRepeating(true);
                auto weak = std::weak_ptr<RenderDebugWidget>(std::dynamic_pointer_cast<RenderDebugWidget>(shared_from_this()));
                _timer->start(
                    Core::Time::getTime(Core::Time::TimerValue::Medium),
                    [weak](const std::chrono::steady_clock::time_point&, const Time::Unit&)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_widgetUpdate();
                    }
                });
            }

            RenderDebugWidget::RenderDebugWidget()
            {}

            std::shared_ptr<RenderDebugWidget> RenderDebugWidget::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<RenderDebugWidget>(new RenderDebugWidget);
                out->_init(context);
                return out;
            }

            void RenderDebugWidget::_widgetUpdate()
            {
                auto render = _getRender();
                const float textureAtlasPercentage = render->getTextureAtlasPercentage();
                const size_t dynamicTextureCount = render->getDynamicTextureCount();
                const size_t vboSize = render->getVBOSize();

                _thermometerWidgets["TextureAtlas"]->setPercentage(textureAtlasPercentage);
                _lineGraphs["DynamicTextureCount"]->addSample(dynamicTextureCount);
                _lineGraphs["VBOSize"]->addSample(vboSize);

                {
                    std::stringstream ss;
                    ss << _getText(DJV_TEXT("Texture atlas")) << ":";
                    _labels["TextureAtlas"]->setText(ss.str());
                }
                {
                    std::stringstream ss;
                    ss.precision(2);
                    ss << std::fixed << textureAtlasPercentage << "%";
                    _labels["TextureAtlasValue"]->setText(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << _getText(DJV_TEXT("Dynamic texture count")) << ":";
                    _labels["DynamicTextureCount"]->setText(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << dynamicTextureCount;
                    _labels["DynamicTextureCountValue"]->setText(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << _getText(DJV_TEXT("VBO size")) << ":";
                    _labels["VBOSize"]->setText(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << vboSize;
                    _labels["VBOSizeValue"]->setText(ss.str());
                }
            }

            class MediaDebugWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(MediaDebugWidget);

            protected:
                void _init(const std::shared_ptr<Context>&);
                MediaDebugWidget();

            public:
                static std::shared_ptr<MediaDebugWidget> create(const std::shared_ptr<Context>&);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

                void _initEvent(Event::Init&) override;

            private:
                void _widgetUpdate();

                Frame::Sequence _sequence;
                Frame::Index _currentFrame = 0;
                size_t _videoQueueMax = 0;
                size_t _videoQueueCount = 0;
                size_t _audioQueueMax = 0;
                size_t _audioQueueCount = 0;
                std::map<std::string, std::shared_ptr<UI::Label> > _labels;
                std::map<std::string, std::shared_ptr<UI::LineGraphWidget> > _lineGraphs;
                std::shared_ptr<UI::VerticalLayout> _layout;
                std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > _currentMediaObserver;
                std::shared_ptr<ValueObserver<Frame::Sequence> > _sequenceObserver;
                std::shared_ptr<ValueObserver<Frame::Index> > _currentFrameObserver;
                std::shared_ptr<ValueObserver<size_t> > _videoQueueMaxObserver;
                std::shared_ptr<ValueObserver<size_t> > _videoQueueCountObserver;
                std::shared_ptr<ValueObserver<size_t> > _audioQueueMaxObserver;
                std::shared_ptr<ValueObserver<size_t> > _audioQueueCountObserver;
            };

            void MediaDebugWidget::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                setClassName("djv::ViewApp::MediaDebugWidget");

                _labels["CurrentFrame"] = UI::Label::create(context);
                _labels["CurrentFrameValue"] = UI::Label::create(context);
                _labels["CurrentFrameValue"]->setFont(AV::Font::familyMono);
                
                _labels["VideoQueue"] = UI::Label::create(context);
                _lineGraphs["VideoQueue"] = UI::LineGraphWidget::create(context);
                _lineGraphs["VideoQueue"]->setPrecision(0);

                _labels["AudioQueue"] = UI::Label::create(context);
                _lineGraphs["AudioQueue"] = UI::LineGraphWidget::create(context);
                _lineGraphs["AudioQueue"]->setPrecision(0);

                for (auto& i : _labels)
                {
                    i.second->setTextHAlign(UI::TextHAlign::Left);
                }

                _layout = UI::VerticalLayout::create(context);
                _layout->setMargin(UI::Layout::Margin(UI::MetricsRole::Margin));
                auto hLayout = UI::HorizontalLayout::create(context);
                hLayout->addChild(_labels["CurrentFrame"]);
                hLayout->addChild(_labels["CurrentFrameValue"]);
                _layout->addChild(hLayout);
                _layout->addChild(_labels["VideoQueue"]);
                _layout->addChild(_lineGraphs["VideoQueue"]);
                _layout->addChild(_labels["AudioQueue"]);
                _layout->addChild(_lineGraphs["AudioQueue"]);
                addChild(_layout);

                auto weak = std::weak_ptr<MediaDebugWidget>(std::dynamic_pointer_cast<MediaDebugWidget>(shared_from_this()));
                if (auto fileSystem = context->getSystemT<FileSystem>())
                {
                    _currentMediaObserver = ValueObserver<std::shared_ptr<Media>>::create(
                        fileSystem->observeCurrentMedia(),
                        [weak](const std::shared_ptr<Media> & value)
                    {
                        if (auto widget = weak.lock())
                        {
                            for (const auto& i : widget->_lineGraphs)
                            {
                                i.second->resetSamples();
                            }

                            if (value)
                            {
                                widget->_sequenceObserver = ValueObserver<Frame::Sequence>::create(
                                    value->observeSequence(),
                                    [weak](const Frame::Sequence& value)
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        widget->_sequence = value;
                                        widget->_widgetUpdate();
                                    }
                                });
                                widget->_currentFrameObserver = ValueObserver<Frame::Index>::create(
                                    value->observeCurrentFrame(),
                                    [weak](Frame::Index value)
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        widget->_currentFrame = value;
                                        widget->_widgetUpdate();
                                    }
                                });
                                widget->_videoQueueMaxObserver = ValueObserver<size_t>::create(
                                    value->observeVideoQueueMax(),
                                    [weak](size_t value)
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        widget->_videoQueueMax = value;
                                        widget->_widgetUpdate();
                                    }
                                });
                                widget->_videoQueueCountObserver = ValueObserver<size_t>::create(
                                    value->observeVideoQueueCount(),
                                    [weak](size_t value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_videoQueueCount = value;
                                            widget->_lineGraphs["VideoQueue"]->addSample(value);
                                            widget->_widgetUpdate();
                                        }
                                    });
                                widget->_audioQueueMaxObserver = ValueObserver<size_t>::create(
                                    value->observeAudioQueueMax(),
                                    [weak](size_t value)
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        widget->_audioQueueMax = value;
                                        widget->_widgetUpdate();
                                    }
                                });
                                widget->_audioQueueCountObserver = ValueObserver<size_t>::create(
                                    value->observeAudioQueueCount(),
                                    [weak](size_t value)
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        widget->_audioQueueCount = value;
                                        widget->_lineGraphs["AudioQueue"]->addSample(value);
                                        widget->_widgetUpdate();
                                    }
                                });
                            }
                            else
                            {
                                widget->_sequence = Frame::Sequence();
                                widget->_currentFrame = 0;
                                widget->_videoQueueMax = 0;
                                widget->_videoQueueCount = 0;
                                widget->_audioQueueMax = 0;
                                widget->_audioQueueCount = 0;
                                widget->_sequenceObserver.reset();
                                widget->_currentFrameObserver.reset();
                                widget->_videoQueueMaxObserver.reset();
                                widget->_videoQueueCountObserver.reset();
                                widget->_audioQueueMaxObserver.reset();
                                widget->_audioQueueCountObserver.reset();
                                widget->_widgetUpdate();
                            }
                        }
                    });
                }
            }

            MediaDebugWidget::MediaDebugWidget()
            {}

            std::shared_ptr<MediaDebugWidget> MediaDebugWidget::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<MediaDebugWidget>(new MediaDebugWidget);
                out->_init(context);
                return out;
            }

            void MediaDebugWidget::_preLayoutEvent(Event::PreLayout&)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void MediaDebugWidget::_layoutEvent(Event::Layout&)
            {
                _layout->setGeometry(getGeometry());
            }

            void MediaDebugWidget::_initEvent(Event::Init& event)
            {
                Widget::_initEvent(event);
                {
                    std::stringstream ss;
                    ss << _getText(DJV_TEXT("Video queue")) << ":";
                    _labels["VideoQueue"]->setText(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << _getText(DJV_TEXT("Audio queue")) << ":";
                    _labels["AudioQueue"]->setText(ss.str());
                }
                _widgetUpdate();
            }

            void MediaDebugWidget::_widgetUpdate()
            {
                {
                    std::stringstream ss;
                    ss << _getText(DJV_TEXT("Current time")) << ":";
                    _labels["CurrentFrame"]->setText(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << _currentFrame << " / " << _sequence.getSize();
                    _labels["CurrentFrameValue"]->setText(ss.str());
                }
            }

        } // namespace

        struct DebugWidget::Private
        {
            std::map < std::string, std::shared_ptr<UI::Bellows> > bellows;
        };

        void DebugWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            MDIWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::DebugWidget");

            auto layout = UI::VerticalLayout::create(context);
            layout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));

            auto generalDebugWidget = GeneralDebugWidget::create(context);
            p.bellows["General"] = UI::Bellows::create(context);
            p.bellows["General"]->addChild(generalDebugWidget);
            layout->addChild(p.bellows["General"]);

            auto renderDebugWidget = RenderDebugWidget::create(context);
            p.bellows["Render"] = UI::Bellows::create(context);
            p.bellows["Render"]->setOpen(false);
            p.bellows["Render"]->addChild(renderDebugWidget);
            layout->addChild(p.bellows["Render"]);

            auto mediaDebugWidget = MediaDebugWidget::create(context);
            p.bellows["Media"] = UI::Bellows::create(context);
            p.bellows["Media"]->setOpen(false);
            p.bellows["Media"]->addChild(mediaDebugWidget);
            layout->addChild(p.bellows["Media"]);

            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->setBackgroundRole(UI::ColorRole::Background);
            scrollWidget->setShadowOverlay({ UI::Side::Top });
            scrollWidget->addChild(layout);
            addChild(scrollWidget);
        }

        DebugWidget::DebugWidget() :
            _p(new Private)
        {}

        DebugWidget::~DebugWidget()
        {}

        std::shared_ptr<DebugWidget> DebugWidget::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<DebugWidget>(new DebugWidget);
            out->_init(context);
            return out;
        }

        std::map<std::string, bool> DebugWidget::getBellowsState() const
        {
            DJV_PRIVATE_PTR();
            std::map<std::string, bool> out;
            for (const auto& i : p.bellows)
            {
                out[i.first] = i.second->isOpen();
            }
            return out;
        }

        void DebugWidget::setBellowsState(const std::map<std::string, bool>& value)
        {
            DJV_PRIVATE_PTR();
            for (const auto& i : value)
            {
                const auto j = p.bellows.find(i.first);
                if (j != p.bellows.end())
                {
                    j->second->setOpen(i.second);
                }
            }
        }

        void DebugWidget::_initEvent(Event::Init & event)
        {
            MDIWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            setTitle(_getText(DJV_TEXT("Debug")));
            p.bellows["General"]->setText(_getText(DJV_TEXT("General")));
            p.bellows["Render"]->setText(_getText(DJV_TEXT("Render")));
            p.bellows["Media"]->setText(_getText(DJV_TEXT("Media")));
        }

    } // namespace ViewApp
} // namespace djv

