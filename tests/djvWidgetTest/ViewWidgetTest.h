//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvWidgetTest/AbstractWidgetTest.h>

#include <djvCore/Sequence.h>
#include <djvCore/Speed.h>

#include <QPointer>
#include <QWidget>

class QComboBox;

namespace djv
{
    namespace ViewLib
    {
        class FrameWidget;
        class FrameSlider;
        class FrameDisplay;
        class SpeedWidget;
        class SpeedDisplay;

    } // namespace ViewLib

    namespace WidgetTest
    {
        class ViewWidget : public QWidget
        {
            Q_OBJECT

        public:
            ViewWidget(const QPointer<UI::UIContext> &);

        private Q_SLOTS:
            void timeUnitsCallback(int);
            void frameCallback(qint64);
            void speedCallback(const djv::Core::Speed &);

            void widgetUpdate();
            void frameUpdate();

        private:
            qint64 _frame;
            bool _inOutEnabled;
            qint64 _inPoint;
            qint64 _outPoint;
            Core::FrameList _frameList;
            Core::FrameList _cachedFrames;
            Core::Speed _speed;

            QPointer<UI::UIContext> _context;
            QComboBox * _timeUnitsWidget = nullptr;
            ViewLib::FrameWidget *  _frameWidget = nullptr;
            ViewLib::FrameSlider *  _frameSlider = nullptr;
            ViewLib::FrameDisplay * _frameDisplay = nullptr;
            ViewLib::SpeedWidget *  _speedWidget = nullptr;
            ViewLib::SpeedDisplay * _speedDisplay = nullptr;
        };

        class ViewWidgetTest : public AbstractWidgetTest
        {
            Q_OBJECT

        public:
            ViewWidgetTest(const QPointer<UI::UIContext> &);

            QString name() override;
            void run(const QStringList & args = QStringList()) override;
        };

    } // namespace WidgetTest
} // namespace djv
