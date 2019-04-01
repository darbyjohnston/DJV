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

#include <djvViewLib/AnnotationsTool.h>

#include <djvViewLib/AnnotationsData.h>
#include <djvViewLib/AnnotationsGroup.h>
#include <djvViewLib/ImageView.h>
#include <djvViewLib/Session.h>
#include <djvViewLib/ViewContext.h>

#include <djvUI/IconLibrary.h>
#include <djvUI/ToolButton.h>

#include <djvAV/Color.h>

#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QComboBox>
#include <QListView>
#include <QPointer>
#include <QSpinBox>
#include <QTabBar>
#include <QTextEdit>

namespace djv
{
    namespace ViewLib
    {
        struct AnnotationsTool::Private
        {
            Enum::ANNOTATIONS_PRIMITIVE primitive = static_cast<Enum::ANNOTATIONS_PRIMITIVE>(0);
            Enum::ANNOTATIONS_COLOR color = static_cast<Enum::ANNOTATIONS_COLOR>(0);
            size_t lineWidth = 1;
            std::shared_ptr<Annotations::AbstractPrimitive> currentPrimitive;
            QPointer<QListView> listView;
            QPointer<QTabBar> tabBar;
            QPointer<UI::ToolButton> prevButton;
            QPointer<UI::ToolButton> nextButton;
            QPointer<UI::ToolButton> removeButton;
            QPointer<UI::ToolButton> addButton;
            QPointer<QTextEdit> textEdit;
            QPointer<QButtonGroup> primitiveButtonGroup;
            QPointer<QComboBox> colorComboBox;
            QPointer<QSpinBox> lineWidthSpinBox;
            QPointer<UI::ToolButton> clearButton;
            QPointer<QHBoxLayout> prevNextLayout;
            QPointer<QHBoxLayout> addRemoveLayout;
            QPointer<QHBoxLayout> primitiveLayout;
        };

        AnnotationsTool::AnnotationsTool(
            const QPointer<Session> & session,
            const QPointer<ViewContext> & context,
            QWidget * parent) :
            AbstractTool(session, context, parent),
            _p(new Private)
        {
            // Create the widgets.
            _p->listView = new QListView;

            _p->tabBar = new QTabBar;
            _p->tabBar->addTab("Annotation");
            _p->tabBar->addTab("Frames");
            _p->tabBar->addTab("Summary");
            _p->tabBar->addTab("Export");

            _p->prevButton = new UI::ToolButton(context.data());
            _p->nextButton = new UI::ToolButton(context.data());
            _p->removeButton = new UI::ToolButton(context.data());
            _p->addButton = new UI::ToolButton(context.data());

            _p->textEdit = new QTextEdit;

            _p->primitiveButtonGroup = new QButtonGroup;
            _p->primitiveButtonGroup->setExclusive(true);
            for (int i = 0; i < Enum::ANNOTATIONS_PRIMITIVE_COUNT; ++i)
            {
                auto button = new UI::ToolButton(context.data());
                button->setCheckable(true);
                _p->primitiveButtonGroup->addButton(button, i);
            }

            _p->colorComboBox = new QComboBox;
            const auto & colorLabels = Enum::annotationsColorLabels();
            for (int i = 0; i < Enum::ANNOTATIONS_COLOR_COUNT; ++i)
            {
                _p->colorComboBox->addItem(colorLabels[i]);
            }

            _p->lineWidthSpinBox = new QSpinBox;
            _p->lineWidthSpinBox->setRange(1, 100);

            _p->clearButton = new UI::ToolButton(context.data());

            // Layout the widgets.
            auto layout = new QVBoxLayout(this);
            layout->setMargin(0);
            layout->addWidget(_p->tabBar);

            auto listTab = new QWidget;
            auto vLayout = new QVBoxLayout(listTab);
            vLayout->setMargin(0);
            //vLayout->addWidget(_p->listView);
            auto hLayout = new QHBoxLayout;
            hLayout->setMargin(0);
            _p->prevNextLayout = new QHBoxLayout;
            _p->prevNextLayout->setMargin(0);
            _p->prevNextLayout->addWidget(_p->prevButton);
            _p->prevNextLayout->addWidget(_p->nextButton);
            hLayout->addLayout(_p->prevNextLayout);
            hLayout->addStretch();
            _p->addRemoveLayout = new QHBoxLayout;
            _p->addRemoveLayout->setMargin(0);
            _p->addRemoveLayout->addWidget(_p->removeButton);
            _p->addRemoveLayout->addWidget(_p->addButton);
            hLayout->addLayout(_p->addRemoveLayout);
            //vLayout->addLayout(hLayout);
            vLayout->addWidget(_p->textEdit);
            hLayout = new QHBoxLayout;
            hLayout->setMargin(0);
            _p->primitiveLayout = new QHBoxLayout;
            _p->primitiveLayout->setMargin(0);
            Q_FOREACH(auto button, _p->primitiveButtonGroup->buttons())
            {
                _p->primitiveLayout->addWidget(button);
            }
            hLayout->addLayout(_p->primitiveLayout);
            hLayout->addWidget(_p->colorComboBox);
            hLayout->addWidget(_p->lineWidthSpinBox);
            hLayout->addWidget(_p->clearButton);
            vLayout->addLayout(hLayout);
            layout->addWidget(listTab);

            // Initialize.
            setWindowTitle(qApp->translate("djv::ViewLib::AnnotationsTool", "Annotations"));
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            styleUpdate();
            widgetUpdate();

            // Setup the callbacks.
            connect(
                _p->primitiveButtonGroup,
                SIGNAL(buttonClicked(int)),
                SLOT(primitiveCallback(int)));

            connect(
                _p->colorComboBox,
                SIGNAL(activated(int)),
                SLOT(colorCallback(int)));

            connect(
                _p->lineWidthSpinBox,
                SIGNAL(valueChanged(int)),
                SLOT(lineWidthCallback(int)));

            connect(
                _p->clearButton,
                SIGNAL(clicked()),
                SIGNAL(clearPrimitives()));

            auto group = session->annotationsGroup();
            connect(
                group,
                SIGNAL(primitiveChanged(djv::ViewLib::Enum::ANNOTATIONS_PRIMITIVE)),
                SLOT(setPrimitive(djv::ViewLib::Enum::ANNOTATIONS_PRIMITIVE)));
            connect(
                group,
                SIGNAL(colorChanged(djv::ViewLib::Enum::ANNOTATIONS_COLOR)),
                SLOT(setColor(djv::ViewLib::Enum::ANNOTATIONS_COLOR)));
            connect(
                group,
                SIGNAL(lineWidthChanged(size_t)),
                SLOT(setLineWidth(size_t)));

            auto viewWidget = session->viewWidget();
            connect(
                viewWidget,
                SIGNAL(pickPressed(const glm::ivec2 &)),
                SLOT(pickPressedCallback(const glm::ivec2 &)));
            connect(
                viewWidget,
                SIGNAL(pickReleased(const glm::ivec2 &)),
                SLOT(pickReleasedCallback(const glm::ivec2 &)));
            connect(
                viewWidget,
                SIGNAL(pickMoved(const glm::ivec2 &)),
                SLOT(pickMovedCallback(const glm::ivec2 &)));
        }

        AnnotationsTool::~AnnotationsTool()
        {}

        void AnnotationsTool::setPrimitive(djv::ViewLib::Enum::ANNOTATIONS_PRIMITIVE value)
        {
            if (value == _p->primitive)
                return;
            _p->primitive = value;
            widgetUpdate();
            Q_EMIT primitiveChanged(_p->primitive);
        }

        void AnnotationsTool::setColor(djv::ViewLib::Enum::ANNOTATIONS_COLOR value)
        {
            if (value == _p->color)
                return;
            _p->color = value;
            widgetUpdate();
            Q_EMIT colorChanged(_p->color);
        }

        void AnnotationsTool::setLineWidth(size_t value)
        {
            if (value == _p->lineWidth)
                return;
            _p->lineWidth = value;
            widgetUpdate();
            Q_EMIT lineWidthChanged(_p->lineWidth);
        }

        void AnnotationsTool::primitiveCallback(int value)
        {
            setPrimitive(static_cast<Enum::ANNOTATIONS_PRIMITIVE>(value));
        }

        void AnnotationsTool::colorCallback(int value)
        {
            setColor(static_cast<Enum::ANNOTATIONS_COLOR>(value));
        }

        void AnnotationsTool::lineWidthCallback(int value)
        {
            setLineWidth(value);
        }

        void AnnotationsTool::pickPressedCallback(const glm::ivec2 & in)
        {
            if (isVisible())
            {
                auto group = session()->annotationsGroup();
                _p->currentPrimitive = Annotations::PrimitiveFactory::create(group->primitive(), group->color(), group->lineWidth());
                Annotations::Data data;
                data.primitives.push_back(_p->currentPrimitive);
                group->addAnnotation(data);
            }
        }

        void AnnotationsTool::pickReleasedCallback(const glm::ivec2 & in)
        {
            _p->currentPrimitive.reset();
        }

        void AnnotationsTool::pickMovedCallback(const glm::ivec2 & in)
        {
            if (isVisible() && _p->currentPrimitive)
            {
                const auto view = session()->viewWidget();
                const glm::ivec2 & viewPos = view->viewPos();
                const float viewZoom = view->viewZoom();
                _p->currentPrimitive->mouse(glm::ivec2(
                    (in.x - viewPos.x) / viewZoom,
                    (in.y - viewPos.y) / viewZoom));
                session()->viewWidget()->update();
            }
        }

        void AnnotationsTool::styleUpdate()
        {
            _p->prevButton->setIcon(context()->iconLibrary()->icon("djv/UI/LeftIcon"));
            _p->nextButton->setIcon(context()->iconLibrary()->icon("djv/UI/RightIcon"));
            _p->removeButton->setIcon(context()->iconLibrary()->icon("djv/UI/RemoveIcon"));
            _p->addButton->setIcon(context()->iconLibrary()->icon("djv/UI/AddIcon"));
            const auto primitiveIcons = std::vector<std::string>(
            {
                "djv/UI/PrimitivePolyline",
                "djv/UI/PrimitiveRect",
                "djv/UI/PrimitiveEllipse"
            });
            auto primitiveButtons = _p->primitiveButtonGroup->buttons();
            for (int i = 0; i < static_cast<int>(primitiveButtons.size()); ++i)
            {
                primitiveButtons[i]->setIcon(context()->iconLibrary()->icon(primitiveIcons[i].c_str()));
            }
            _p->clearButton->setIcon(context()->iconLibrary()->icon("djv/UI/RemoveIcon"));

            _p->prevNextLayout->setSpacing(style()->pixelMetric(QStyle::PM_ToolBarItemSpacing));
            _p->addRemoveLayout->setSpacing(style()->pixelMetric(QStyle::PM_ToolBarItemSpacing));
            _p->primitiveLayout->setSpacing(style()->pixelMetric(QStyle::PM_ToolBarItemSpacing));
        }

        void AnnotationsTool::widgetUpdate()
        {
            Core::SignalBlocker signalBlocker(QObjectList() <<
                _p->primitiveButtonGroup <<
                _p->colorComboBox <<
                _p->lineWidthSpinBox);
            auto primitiveButtons = _p->primitiveButtonGroup->buttons();
            primitiveButtons[_p->primitive]->setChecked(true);
            _p->colorComboBox->setCurrentIndex(_p->color);
            _p->lineWidthSpinBox->setValue(static_cast<int>(_p->lineWidth));
        }

    } // namespace ViewLib
} // namespace djv
