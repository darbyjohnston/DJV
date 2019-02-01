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

#include <djvViewLib/DebugTool.h>

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
        struct DebugTool::Private
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
            std::shared_ptr<UI::Layout::Form> layout;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<ValueObserver<Time::Duration> > durationObserver;
            std::shared_ptr<ValueObserver<Time::Timestamp> > currentTimeObserver;
            std::shared_ptr<ValueObserver<size_t> > videoQueueMaxObserver;
            std::shared_ptr<ValueObserver<size_t> > audioQueueMaxObserver;
            std::shared_ptr<ValueObserver<size_t> > videoQueueCountObserver;
            std::shared_ptr<ValueObserver<size_t> > audioQueueCountObserver;
            std::shared_ptr<ValueObserver<size_t> > alUnqueuedBuffersObserver;
        };

        void DebugTool::_init(Context * context)
        {
            IToolWidget::_init(context);

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

            p.layout = UI::Layout::Form::create(context);
            p.layout->setMargin(UI::Style::MetricsRole::Margin);
            p.text["Duration"] = p.layout->addWidget(std::string(), p.labels["Duration"]);
            p.text["CurrentTime"] = p.layout->addWidget(std::string(), p.labels["CurrentTime"]);
            p.text["VideoQueue"] = p.layout->addWidget(std::string(), p.labels["VideoQueue"]);
            p.text["AudioQueue"] = p.layout->addWidget(std::string(), p.labels["AudioQueue"]);
            p.text["ALUnqueuedBuffers"] = p.layout->addWidget(std::string(), p.labels["ALUnqueuedBuffers"]);
            addWidget(p.layout);

            auto weak = std::weak_ptr<DebugTool>(std::dynamic_pointer_cast<DebugTool>(shared_from_this()));
            if (auto fileSystem = context->getSystemT<FileSystem>().lock())
            {
                p.currentMediaObserver = ValueObserver<std::shared_ptr<Media>>::create(
                    fileSystem->observeCurrentMedia(),
                    [weak](const std::shared_ptr<Media> & value)
                {
                    if (auto tool = weak.lock())
                    {
                        if (value)
                        {
                            tool->_p->durationObserver = ValueObserver<Time::Duration>::create(
                                value->observeDuration(),
                                [weak](Time::Duration value)
                            {
                                if (auto tool = weak.lock())
                                {
                                    tool->_p->duration = value;
                                    tool->_widgetUpdate();
                                }
                            });
                            tool->_p->currentTimeObserver = ValueObserver<Time::Timestamp>::create(
                                value->observeCurrentTime(),
                                [weak](Time::Timestamp value)
                            {
                                if (auto tool = weak.lock())
                                {
                                    tool->_p->currentTime = value;
                                    tool->_widgetUpdate();
                                }
                            });
                            tool->_p->videoQueueMaxObserver = ValueObserver<size_t>::create(
                                value->observeVideoQueueMax(),
                                [weak](size_t value)
                            {
                                if (auto tool = weak.lock())
                                {
                                    tool->_p->videoQueueMax = value;
                                    tool->_widgetUpdate();
                                }
                            });
                            tool->_p->audioQueueMaxObserver = ValueObserver<size_t>::create(
                                value->observeAudioQueueMax(),
                                [weak](size_t value)
                            {
                                if (auto tool = weak.lock())
                                {
                                    tool->_p->audioQueueMax = value;
                                    tool->_widgetUpdate();
                                }
                            });
                            tool->_p->videoQueueCountObserver = ValueObserver<size_t>::create(
                                value->observeVideoQueueCount(),
                                [weak](size_t value)
                            {
                                if (auto tool = weak.lock())
                                {
                                    tool->_p->videoQueueCount = value;
                                    tool->_widgetUpdate();
                                }
                            });
                            tool->_p->audioQueueCountObserver = ValueObserver<size_t>::create(
                                value->observeAudioQueueCount(),
                                [weak](size_t value)
                            {
                                if (auto tool = weak.lock())
                                {
                                    tool->_p->audioQueueCount = value;
                                    tool->_widgetUpdate();
                                }
                            });
                            tool->_p->alUnqueuedBuffersObserver = ValueObserver<size_t>::create(
                                value->observeALUnqueuedBuffers(),
                                [weak](size_t value)
                            {
                                if (auto tool = weak.lock())
                                {
                                    tool->_p->alUnqueuedBuffers = value;
                                    tool->_widgetUpdate();
                                }
                            });
                        }
                        else
                        {
                            tool->_p->duration = 0;
                            tool->_p->currentTime = 0;
                            tool->_p->videoQueueMax = 0;
                            tool->_p->audioQueueMax = 0;
                            tool->_p->videoQueueCount = 0;
                            tool->_p->audioQueueCount = 0;
                            tool->_p->alUnqueuedBuffers = 0;
                            tool->_p->durationObserver.reset();
                            tool->_p->currentTimeObserver.reset();
                            tool->_p->videoQueueMaxObserver.reset();
                            tool->_p->audioQueueMaxObserver.reset();
                            tool->_p->videoQueueCountObserver.reset();
                            tool->_p->audioQueueCountObserver.reset();
                            tool->_p->alUnqueuedBuffersObserver.reset();
                            tool->_widgetUpdate();
                        }
                    }
                });
            }
        }

        DebugTool::DebugTool() :
            _p(new Private)
        {}

        DebugTool::~DebugTool()
        {}

        std::shared_ptr<DebugTool> DebugTool::create(Context * context)
        {
            auto out = std::shared_ptr<DebugTool>(new DebugTool);
            out->_init(context);
            return out;
        }

        void DebugTool::_localeEvent(Event::Locale &)
        {
            DJV_PRIVATE_PTR();
            setTitle(_getText(DJV_TEXT("djv::ViewLib::DebugTool", "Debug")));
            {
                std::stringstream ss;
                ss << _getText(DJV_TEXT("djv::ViewLib::DebugTool", "Duration")) << ":";
                p.layout->setText(p.text["Duration"], ss.str());
            }
            {
                std::stringstream ss;
                ss << _getText(DJV_TEXT("djv::ViewLib::DebugTool", "Current Time")) << ":";
                p.layout->setText(p.text["CurrentTime"], ss.str());
            }
            {
                std::stringstream ss;
                ss << _getText(DJV_TEXT("djv::ViewLib::DebugTool", "Video Queue")) << ":";
                p.layout->setText(p.text["VideoQueue"], ss.str());
            }
            {
                std::stringstream ss;
                ss << _getText(DJV_TEXT("djv::ViewLib::DebugTool", "Audio Queue")) << ":";
                p.layout->setText(p.text["AudioQueue"], ss.str());
            }
            {
                std::stringstream ss;
                ss << _getText(DJV_TEXT("djv::ViewLib::DebugTool", "AL Unqueued Buffers")) << ":";
                p.layout->setText(p.text["ALUnqueuedBuffers"], ss.str());
            }
        }

        void DebugTool::_widgetUpdate()
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

