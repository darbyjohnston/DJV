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

#include <djvViewLib/MousePrefsWidget.h>

#include <djvViewLib/MousePrefsModel.h>
#include <djvViewLib/ViewContext.h>

#include <djvUI/IconLibrary.h>
#include <djvUI/PrefsGroupBox.h>
#include <djvUI/QuestionDialog.h>
#include <djvUI/ToolButton.h>

#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QFormLayout>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QVBoxLayout>

namespace djv
{
    namespace ViewLib
    {
        struct MousePrefsWidget::Private
        {
            QScopedPointer<MouseButtonActionsModel> mouseButtonActionsModel;
            QPointer<QSortFilterProxyModel> mouseButtonActionsProxyModel;
            QScopedPointer<MouseWheelActionsModel> mouseWheelActionsModel;
            QPointer<QSortFilterProxyModel> mouseWheelActionsProxyModel;

            QScopedPointer<MouseButtonActionDelegate> mouseButtonActionsDelegate;
            QScopedPointer<MouseWheelActionDelegate> mouseWheelActionsDelegate;

            QPointer<QTableView> mouseButtonActionsWidget;
            QPointer<UI::ToolButton> mouseButtonActionAddButton;
            QPointer<UI::ToolButton> mouseButtonActionRemoveButton;
            QPointer<QTableView> mouseWheelActionsWidget;
            QPointer<UI::ToolButton> mouseWheelActionAddButton;
            QPointer<UI::ToolButton> mouseWheelActionRemoveButton;
        };

        MousePrefsWidget::MousePrefsWidget(const QPointer<ViewContext> & context) :
            AbstractPrefsWidget(qApp->translate("djv::ViewLib::MousePrefsWidget", "Mouse"), context),
            _p(new Private)
        {
            // Create the models.
            _p->mouseButtonActionsModel.reset(new MouseButtonActionsModel);
            _p->mouseButtonActionsProxyModel = new QSortFilterProxyModel(this);
            _p->mouseButtonActionsProxyModel->setSourceModel(_p->mouseButtonActionsModel.data());
            _p->mouseButtonActionsProxyModel->setSortRole(Qt::EditRole);
            _p->mouseButtonActionsProxyModel->sort(0, Qt::AscendingOrder);

            _p->mouseWheelActionsModel.reset(new MouseWheelActionsModel);
            _p->mouseWheelActionsProxyModel = new QSortFilterProxyModel(this);
            _p->mouseWheelActionsProxyModel->setSourceModel(_p->mouseWheelActionsModel.data());
            _p->mouseWheelActionsProxyModel->setSortRole(Qt::EditRole);
            _p->mouseWheelActionsProxyModel->sort(0, Qt::AscendingOrder);

            // Create the delegates.
            _p->mouseButtonActionsDelegate.reset(new MouseButtonActionDelegate);
            _p->mouseWheelActionsDelegate.reset(new MouseWheelActionDelegate);

            // Create the widgets.
            _p->mouseButtonActionsWidget = new QTableView;
            _p->mouseButtonActionsWidget->setSortingEnabled(true);
            _p->mouseButtonActionsWidget->setSelectionMode(QAbstractItemView::SingleSelection);
            _p->mouseButtonActionsWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
            _p->mouseButtonActionsWidget->setItemDelegate(_p->mouseButtonActionsDelegate.data());
            _p->mouseButtonActionsWidget->setModel(_p->mouseButtonActionsProxyModel.data());
            _p->mouseButtonActionsWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
            _p->mouseButtonActionsWidget->horizontalHeader()->setStretchLastSection(true);

            _p->mouseButtonActionAddButton = new UI::ToolButton(context.data());
            _p->mouseButtonActionAddButton->setToolTip(
                qApp->translate("djv::ViewLib::MousePrefsWidget", "Add a new mouse button action"));
            _p->mouseButtonActionRemoveButton = new UI::ToolButton(context.data());
            _p->mouseButtonActionRemoveButton->setToolTip(
                qApp->translate("djv::ViewLib::MousePrefsWidget", "Remove the selected mouse button action"));

            _p->mouseWheelActionsWidget = new QTableView;
            _p->mouseWheelActionsWidget->setSortingEnabled(true);
            _p->mouseWheelActionsWidget->setSelectionMode(QAbstractItemView::SingleSelection);
            _p->mouseWheelActionsWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
            _p->mouseWheelActionsWidget->setItemDelegate(_p->mouseWheelActionsDelegate.data());
            _p->mouseWheelActionsWidget->setModel(_p->mouseWheelActionsProxyModel.data());
            _p->mouseWheelActionsWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
            _p->mouseWheelActionsWidget->horizontalHeader()->setStretchLastSection(true);

            _p->mouseWheelActionAddButton = new UI::ToolButton(context.data());
            _p->mouseWheelActionAddButton->setToolTip(
                qApp->translate("djv::ViewLib::MousePrefsWidget", "Add a new mouse wheel action"));
            _p->mouseWheelActionRemoveButton = new UI::ToolButton(context.data());
            _p->mouseWheelActionRemoveButton->setToolTip(
                qApp->translate("djv::ViewLib::MousePrefsWidget", "Remove the selected mouse wheel action"));

            // Layout the widgets.
            auto layout = new QVBoxLayout(this);

            auto prefsGroupBox = new UI::PrefsGroupBox(
                qApp->translate("djv::ViewLib::MousePrefsWidget", "Mouse Button Actions"), context.data());
            auto formLayout = prefsGroupBox->createLayout();
            auto vLayout = new QVBoxLayout;
            vLayout->addWidget(_p->mouseButtonActionsWidget);
            auto hLayout = new QHBoxLayout;
            hLayout->addStretch();
            hLayout->addWidget(_p->mouseButtonActionAddButton);
            hLayout->addWidget(_p->mouseButtonActionRemoveButton);
            vLayout->addLayout(hLayout);
            formLayout->addRow(vLayout);
            layout->addWidget(prefsGroupBox);

            prefsGroupBox = new UI::PrefsGroupBox(
                qApp->translate("djv::ViewLib::MousePrefsWidget", "Mouse Wheel Actions"), context.data());
            formLayout = prefsGroupBox->createLayout();
            vLayout = new QVBoxLayout;
            vLayout->addWidget(_p->mouseWheelActionsWidget);
            hLayout = new QHBoxLayout;
            hLayout->addStretch();
            hLayout->addWidget(_p->mouseWheelActionAddButton);
            hLayout->addWidget(_p->mouseWheelActionRemoveButton);
            vLayout->addLayout(hLayout);
            formLayout->addRow(vLayout);
            layout->addWidget(prefsGroupBox);

            layout->addStretch();

            // Initialize.
            modelUpdate();
            styleUpdate();
            widgetUpdate();

            // Setup the callbacks.
            connect(
                _p->mouseButtonActionsModel.data(),
                SIGNAL(actionsChanged(const QVector<djv::ViewLib::MouseButtonAction> &)),
                SLOT(mouseButtonActionsCallback(const QVector<djv::ViewLib::MouseButtonAction> &)));
            connect(
                _p->mouseButtonActionsWidget->selectionModel(),
                SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
                SLOT(widgetUpdate()));
            connect(
                _p->mouseButtonActionAddButton,
                SIGNAL(clicked()),
                SLOT(addMouseButtonActionCallback()));
            connect(
                _p->mouseButtonActionRemoveButton,
                SIGNAL(clicked()),
                SLOT(removeMouseButtonActionCallback()));
            connect(
                _p->mouseWheelActionsModel.data(),
                SIGNAL(actionsChanged(const QVector<djv::ViewLib::MouseWheelAction> &)),
                SLOT(mouseWheelActionsCallback(const QVector<djv::ViewLib::MouseWheelAction> &)));
            connect(
                _p->mouseWheelActionsWidget->selectionModel(),
                SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
                SLOT(widgetUpdate()));
            connect(
                _p->mouseWheelActionAddButton,
                SIGNAL(clicked()),
                SLOT(addMouseWheelActionCallback()));
            connect(
                _p->mouseWheelActionRemoveButton,
                SIGNAL(clicked()),
                SLOT(removeMouseWheelActionCallback()));
            connect(
                context->mousePrefs(),
                SIGNAL(prefChanged()),
                SLOT(modelUpdate()));
        }

        MousePrefsWidget::~MousePrefsWidget()
        {}

        void MousePrefsWidget::resetPreferences()
        {
            context()->mousePrefs()->setMouseButtonActions(MousePrefs::mouseButtonActionsDefault());
            context()->mousePrefs()->setMouseWheelActions(MousePrefs::mouseWheelActionsDefault());
        }

        bool MousePrefsWidget::event(QEvent * event)
        {
            if (QEvent::StyleChange == event->type())
            {
                styleUpdate();
            }
            return AbstractPrefsWidget::event(event);
        }

        void MousePrefsWidget::mouseButtonActionsCallback(const QVector<djv::ViewLib::MouseButtonAction> & value)
        {
            context()->mousePrefs()->setMouseButtonActions(value);
            widgetUpdate();
        }

        void MousePrefsWidget::addMouseButtonActionCallback()
        {
            auto actions = context()->mousePrefs()->mouseButtonActions();
            actions.append(MouseButtonAction());
            context()->mousePrefs()->setMouseButtonActions(actions);
        }

        void MousePrefsWidget::removeMouseButtonActionCallback()
        {
            int index = _p->mouseButtonActionsWidget->selectionModel()->currentIndex().row();
            auto actions = context()->mousePrefs()->mouseButtonActions();
            if (-1 == index)
                index = actions.count() - 1;
            if (index != -1)
            {
                UI::QuestionDialog dialog(
                    qApp->translate("djv::ViewLib::MousePrefsWidget", \
                        "Are you sure you want to remove the selected mouse button action?"));
                if (QDialog::Accepted == dialog.exec())
                {
                    actions.remove(index);
                    context()->mousePrefs()->setMouseButtonActions(actions);
                    _p->mouseButtonActionsWidget->selectionModel()->select(
                        QItemSelection(
                            _p->mouseButtonActionsModel->index(index >= actions.count() ? (index - 1) : index, 0),
                            _p->mouseButtonActionsModel->index(index >= actions.count() ? (index - 1) : index, 1)),
                        QItemSelectionModel::Select);
                }
            }
        }

        void MousePrefsWidget::mouseWheelActionsCallback(const QVector<djv::ViewLib::MouseWheelAction> & value)
        {
            context()->mousePrefs()->setMouseWheelActions(value);
            widgetUpdate();
        }

        void MousePrefsWidget::addMouseWheelActionCallback()
        {
            auto actions = context()->mousePrefs()->mouseWheelActions();
            actions.append(MouseWheelAction());
            context()->mousePrefs()->setMouseWheelActions(actions);
        }

        void MousePrefsWidget::removeMouseWheelActionCallback()
        {
            int index = _p->mouseWheelActionsWidget->selectionModel()->currentIndex().row();
            auto actions = context()->mousePrefs()->mouseWheelActions();
            if (-1 == index)
                index = actions.count() - 1;
            if (index != -1)
            {
                UI::QuestionDialog dialog(
                    qApp->translate("djv::ViewLib::MousePrefsWidget", \
                        "Are you sure you want to remove the selected mouse wheel action?"));
                if (QDialog::Accepted == dialog.exec())
                {
                    actions.remove(index);
                    context()->mousePrefs()->setMouseWheelActions(actions);
                    _p->mouseWheelActionsWidget->selectionModel()->select(
                        QItemSelection(
                            _p->mouseWheelActionsModel->index(index >= actions.count() ? (index - 1) : index, 0),
                            _p->mouseWheelActionsModel->index(index >= actions.count() ? (index - 1) : index, 1)),
                        QItemSelectionModel::Select);
                }
            }
        }

        void MousePrefsWidget::modelUpdate()
        {
            _p->mouseButtonActionsModel->setActions(context()->mousePrefs()->mouseButtonActions());
            _p->mouseWheelActionsModel->setActions(context()->mousePrefs()->mouseWheelActions());
        }

        void MousePrefsWidget::styleUpdate()
        {
            _p->mouseButtonActionAddButton->setIcon(context()->iconLibrary()->icon("djv/UI/AddIcon"));
            _p->mouseButtonActionRemoveButton->setIcon(context()->iconLibrary()->icon("djv/UI/RemoveIcon"));
            _p->mouseWheelActionAddButton->setIcon(context()->iconLibrary()->icon("djv/UI/AddIcon"));
            _p->mouseWheelActionRemoveButton->setIcon(context()->iconLibrary()->icon("djv/UI/RemoveIcon"));
            updateGeometry();
        }

        void MousePrefsWidget::widgetUpdate()
        {
            Core::SignalBlocker signalBlocker(QObjectList() <<
                _p->mouseButtonActionRemoveButton <<
                _p->mouseWheelActionRemoveButton);
            _p->mouseButtonActionRemoveButton->setEnabled(
                _p->mouseButtonActionsModel->actions().size() &&
                _p->mouseButtonActionsWidget->selectionModel()->hasSelection());
            _p->mouseWheelActionRemoveButton->setEnabled(
                _p->mouseWheelActionsModel->actions().size() &&
                _p->mouseWheelActionsWidget->selectionModel()->hasSelection());
        }

    } // namespace ViewLib
} // namespace djv
