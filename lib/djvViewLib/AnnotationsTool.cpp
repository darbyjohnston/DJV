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

#include <QApplication>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QComboBox>
#include <QListView>
#include <QPointer>
#include <QTabWidget>
#include <QTextEdit>

namespace djv
{
    namespace ViewLib
    {
        struct AnnotationsTool::Private
        {
            std::shared_ptr<Annotations::AbstractPrimitive> primitive;
            QPointer<QListView> listView;
            QPointer<QTabWidget> tabWidget;
            QPointer<UI::ToolButton> prevButton;
            QPointer<UI::ToolButton> nextButton;
            QPointer<UI::ToolButton> removeButton;
            QPointer<UI::ToolButton> addButton;
            QPointer<QTextEdit> textEdit;
            QPointer<QButtonGroup> primitiveButtonGroup;
            QPointer<QComboBox> colorComboBox;
            QPointer<QComboBox> lineWidthComboBox;
            QPointer<UI::ToolButton> clearButton;
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

            _p->tabWidget = new QTabWidget;

            _p->prevButton = new UI::ToolButton(context.data());
            _p->nextButton = new UI::ToolButton(context.data());
            _p->removeButton = new UI::ToolButton(context.data());
            _p->addButton = new UI::ToolButton(context.data());

            _p->textEdit = new QTextEdit;

            _p->primitiveButtonGroup = new QButtonGroup;
            _p->primitiveButtonGroup->setExclusive(true);
            for (size_t i = 0; i < Enum::PRIMITIVE_COUNT; ++i)
            {
                auto button = new UI::ToolButton(context.data());
                button->setCheckable(true);
                _p->primitiveButtonGroup->addButton(button);
            }
            _p->colorComboBox = new QComboBox;
            _p->lineWidthComboBox = new QComboBox;
            _p->clearButton = new UI::ToolButton(context.data());

            // Layout the widgets.
            auto layout = new QVBoxLayout(this);
            layout->setMargin(0);
            layout->addWidget(_p->tabWidget);

            auto listTab = new QWidget;
            auto vLayout = new QVBoxLayout(listTab);
            auto hLayout = new QHBoxLayout;
            hLayout->addWidget(_p->prevButton);
            hLayout->addWidget(_p->nextButton);
            hLayout->addStretch();
            hLayout->addWidget(_p->removeButton);
            hLayout->addWidget(_p->addButton);
            vLayout->addLayout(hLayout);
            vLayout->addWidget(_p->textEdit);
            hLayout = new QHBoxLayout;
            Q_FOREACH(auto button, _p->primitiveButtonGroup->buttons())
            {
                hLayout->addWidget(button);
            }
            hLayout->addWidget(_p->colorComboBox);
            hLayout->addWidget(_p->lineWidthComboBox);
            hLayout->addWidget(_p->clearButton);
            vLayout->addLayout(hLayout);
            _p->tabWidget->addTab(listTab, "List");

            auto summaryTab = new QWidget;
            _p->tabWidget->addTab(summaryTab, "Summary");

            auto exportTab = new QWidget;
            _p->tabWidget->addTab(exportTab, "Export");

            // Initialize.
            setWindowTitle(qApp->translate("djv::ViewLib::AnnotationsTool", "Annotations"));
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            styleUpdate();
            widgetUpdate();

            // Setup the callbacks.
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

        void AnnotationsTool::pickPressedCallback(const glm::ivec2 & in)
        {
            if (isVisible())
            {
                auto group = session()->annotationsGroup();
                _p->primitive = Annotations::PrimitiveFactory::create(group->primitive(), group->color(), group->lineWidth());
                Annotations::Data data;
                data.primitives.push_back(_p->primitive);
                group->addAnnotation(data);
            }
        }

        void AnnotationsTool::pickReleasedCallback(const glm::ivec2 & in)
        {
            _p->primitive.reset();
        }

        void AnnotationsTool::pickMovedCallback(const glm::ivec2 & in)
        {
            if (isVisible() && _p->primitive)
            {
                const auto view = session()->viewWidget();
                const glm::ivec2 & viewPos = view->viewPos();
                const float viewZoom = view->viewZoom();
                _p->primitive->mouse(glm::ivec2(
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
        }

        void AnnotationsTool::widgetUpdate()
        {
        }

    } // namespace ViewLib
} // namespace djv
