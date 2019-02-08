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

#include <djvViewLib/DebugWidget.h>

#include <djvViewLib/FileSystem.h>
#include <djvViewLib/Media.h>

#include <djvUI/FormLayout.h>
#include <djvUI/Label.h>

#include <djvAV/IO.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct DebugWidget::Private
        {
            Time::Duration duration = 0;
            Time::Timestamp currentTime = 0;
            size_t videoQueueMax = 0;
            size_t audioQueueMax = 0;
            size_t videoQueueCount = 0;
            size_t audioQueueCount = 0;
            size_t alUnqueuedBuffers = 0;
            std::map<std::string, std::shared_ptr<UI::Label> > labels;
            std::map<std::string, size_t> text;
            std::shared_ptr<UI::FormLayout> layout;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<ValueObserver<Time::Duration> > durationObserver;
            std::shared_ptr<ValueObserver<Time::Timestamp> > currentTimeObserver;
            std::shared_ptr<ValueObserver<size_t> > videoQueueMaxObserver;
            std::shared_ptr<ValueObserver<size_t> > audioQueueMaxObserver;
            std::shared_ptr<ValueObserver<size_t> > videoQueueCountObserver;
            std::shared_ptr<ValueObserver<size_t> > audioQueueCountObserver;
            std::shared_ptr<ValueObserver<size_t> > alUnqueuedBuffersObserver;
        };

        void DebugWidget::_init(Context * context)
        {
            IMDIWidget::_init(MDIResize::Minimum, context);

            DJV_PRIVATE_PTR();
            p.labels["Duration"] = UI::Label::create(context);
            p.labels["CurrentTime"] = UI::Label::create(context);
            p.labels["VideoQueue"] = UI::Label::create(context);
            p.labels["AudioQueue"] = UI::Label::create(context);
            p.labels["ALUnqueuedBuffers"] = UI::Label::create(context);
            for (auto & i : p.labels)
            {
                i.second->setTextHAlign(UI::TextHAlign::Left);
            }

            p.layout = UI::FormLayout::create(context);
            p.layout->setMargin(UI::Style::MetricsRole::Margin);
            p.text["Duration"] = p.layout->addWidget(std::string(), p.labels["Duration"]);
            p.text["CurrentTime"] = p.layout->addWidget(std::string(), p.labels["CurrentTime"]);
            p.text["VideoQueue"] = p.layout->addWidget(std::string(), p.labels["VideoQueue"]);
            p.text["AudioQueue"] = p.layout->addWidget(std::string(), p.labels["AudioQueue"]);
            p.text["ALUnqueuedBuffers"] = p.layout->addWidget(std::string(), p.labels["ALUnqueuedBuffers"]);
            addWidget(p.layout);

            auto weak = std::weak_ptr<DebugWidget>(std::dynamic_pointer_cast<DebugWidget>(shared_from_this()));
            if (auto fileSystem = context->getSystemT<FileSystem>().lock())
            {
                p.currentMediaObserver = ValueObserver<std::shared_ptr<Media>>::create(
                    fileSystem->observeCurrentMedia(),
                    [weak](const std::shared_ptr<Media> & value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (value)
                        {
                            widget->_p->durationObserver = ValueObserver<Time::Duration>::create(
                                value->observeDuration(),
                                [weak](Time::Duration value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    widget->_p->duration = value;
                                    widget->_widgetUpdate();
                                }
                            });
                            widget->_p->currentTimeObserver = ValueObserver<Time::Timestamp>::create(
                                value->observeCurrentTime(),
                                [weak](Time::Timestamp value)
                            {
                                if (auto widget = weak.lock())
                                {
									widget->_p->currentTime = value;
									widget->_widgetUpdate();
                                }
                            });
							widget->_p->videoQueueMaxObserver = ValueObserver<size_t>::create(
                                value->observeVideoQueueMax(),
                                [weak](size_t value)
                            {
                                if (auto widget = weak.lock())
                                {
									widget->_p->videoQueueMax = value;
									widget->_widgetUpdate();
                                }
                            });
							widget->_p->audioQueueMaxObserver = ValueObserver<size_t>::create(
                                value->observeAudioQueueMax(),
                                [weak](size_t value)
                            {
                                if (auto widget = weak.lock())
                                {
									widget->_p->audioQueueMax = value;
									widget->_widgetUpdate();
                                }
                            });
							widget->_p->videoQueueCountObserver = ValueObserver<size_t>::create(
                                value->observeVideoQueueCount(),
                                [weak](size_t value)
                            {
                                if (auto widget = weak.lock())
                                {
									widget->_p->videoQueueCount = value;
									widget->_widgetUpdate();
                                }
                            });
							widget->_p->audioQueueCountObserver = ValueObserver<size_t>::create(
                                value->observeAudioQueueCount(),
                                [weak](size_t value)
                            {
                                if (auto widget = weak.lock())
                                {
									widget->_p->audioQueueCount = value;
									widget->_widgetUpdate();
                                }
                            });
							widget->_p->alUnqueuedBuffersObserver = ValueObserver<size_t>::create(
                                value->observeALUnqueuedBuffers(),
                                [weak](size_t value)
                            {
                                if (auto widget = weak.lock())
                                {
									widget->_p->alUnqueuedBuffers = value;
									widget->_widgetUpdate();
                                }
                            });
                        }
                        else
                        {
							widget->_p->duration = 0;
							widget->_p->currentTime = 0;
							widget->_p->videoQueueMax = 0;
							widget->_p->audioQueueMax = 0;
							widget->_p->videoQueueCount = 0;
							widget->_p->audioQueueCount = 0;
							widget->_p->alUnqueuedBuffers = 0;
							widget->_p->durationObserver.reset();
							widget->_p->currentTimeObserver.reset();
							widget->_p->videoQueueMaxObserver.reset();
							widget->_p->audioQueueMaxObserver.reset();
							widget->_p->videoQueueCountObserver.reset();
							widget->_p->audioQueueCountObserver.reset();
							widget->_p->alUnqueuedBuffersObserver.reset();
							widget->_widgetUpdate();
                        }
                    }
                });
            }
        }

        DebugWidget::DebugWidget() :
            _p(new Private)
        {}

        DebugWidget::~DebugWidget()
        {}

        std::shared_ptr<DebugWidget> DebugWidget::create(Context * context)
        {
            auto out = std::shared_ptr<DebugWidget>(new DebugWidget);
            out->_init(context);
            return out;
        }

        void DebugWidget::_localeEvent(Event::Locale & event)
        {
			IMDIWidget::_localeEvent(event);
            DJV_PRIVATE_PTR();
            setTitle(_getText(DJV_TEXT("djv::ViewLib::DebugWidget", "Debug")));
            {
                std::stringstream ss;
                ss << _getText(DJV_TEXT("djv::ViewLib::DebugWidget", "Duration")) << ":";
                p.layout->setText(p.text["Duration"], ss.str());
            }
            {
                std::stringstream ss;
                ss << _getText(DJV_TEXT("djv::ViewLib::DebugWidget", "Current Time")) << ":";
                p.layout->setText(p.text["CurrentTime"], ss.str());
            }
            {
                std::stringstream ss;
                ss << _getText(DJV_TEXT("djv::ViewLib::DebugWidget", "Video Queue")) << ":";
                p.layout->setText(p.text["VideoQueue"], ss.str());
            }
            {
                std::stringstream ss;
                ss << _getText(DJV_TEXT("djv::ViewLib::DebugWidget", "Audio Queue")) << ":";
                p.layout->setText(p.text["AudioQueue"], ss.str());
            }
            {
                std::stringstream ss;
                ss << _getText(DJV_TEXT("djv::ViewLib::DebugWidget", "AL Unqueued Buffers")) << ":";
                p.layout->setText(p.text["ALUnqueuedBuffers"], ss.str());
            }
        }

        void DebugWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            {
                std::stringstream ss;
                ss << p.duration;
                p.labels["Duration"]->setText(ss.str());
            }
            {
                std::stringstream ss;
                ss << p.currentTime;
                p.labels["CurrentTime"]->setText(ss.str());
            }
            {
                std::stringstream ss;
                ss << p.videoQueueCount << "/" << p.videoQueueMax;
                p.labels["VideoQueue"]->setText(ss.str());
            }
            {
                std::stringstream ss;
                ss << p.audioQueueCount << "/" << p.audioQueueMax;
                p.labels["AudioQueue"]->setText(ss.str());
            }
            {
                std::stringstream ss;
                ss << p.alUnqueuedBuffers;
                p.labels["ALUnqueuedBuffers"]->setText(ss.str());
            }
        }

    } // namespace ViewLib
} // namespace djv

