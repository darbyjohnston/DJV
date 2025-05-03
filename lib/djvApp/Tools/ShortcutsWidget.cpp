// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Tools/SettingsToolPrivate.h>

#include <djvApp/App.h>

#include <dtk/ui/GroupBox.h>
#include <dtk/ui/DrawUtil.h>
#include <dtk/ui/FormLayout.h>
#include <dtk/ui/Label.h>
#include <dtk/ui/RowLayout.h>

namespace djv
{
    namespace app
    {
        struct ShortcutWidget::Private
        {
            Shortcut shortcut;
            bool collision = false;

            std::shared_ptr<dtk::Label> label;

            std::function<void(const Shortcut&)> callback;

            struct SizeData
            {
                std::optional<float> displayScale;
                int border = 0;
            };
            SizeData size;

            struct DrawData
            {
                dtk::Box2I g;
                dtk::Box2I g2;
                dtk::TriMesh2F border;
            };
            std::optional<DrawData> draw;
        };

        void ShortcutWidget::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<IWidget>& parent)
        {
            IWidget::_init(context, "tl::play_app::ShortcutWidget", parent);
            DTK_P();
            
            setHStretch(dtk::Stretch::Expanding);
            setAcceptsKeyFocus(true);
            _setMouseHoverEnabled(true);
            _setMousePressEnabled(true);

            p.label = dtk::Label::create(context, shared_from_this());
            p.label->setMarginRole(dtk::SizeRole::MarginInside);

            _widgetUpdate();
        }

        ShortcutWidget::ShortcutWidget() :
            _p(new Private)
        {}

        ShortcutWidget::~ShortcutWidget()
        {}

        std::shared_ptr<ShortcutWidget> ShortcutWidget::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<ShortcutWidget>(new ShortcutWidget);
            out->_init(context, parent);
            return out;
        }

        void ShortcutWidget::setShortcut(const Shortcut& value)
        {
            DTK_P();
            if (value == p.shortcut)
                return;
            p.shortcut = value;
            _widgetUpdate();
        }

        void ShortcutWidget::setCallback(const std::function<void(const Shortcut&)>& value)
        {
            _p->callback = value;
        }

        void ShortcutWidget::setCollision(bool value)
        {
            DTK_P();
            if (value == p.collision)
                return;
            p.collision = value;
            _widgetUpdate();
        }

        void ShortcutWidget::setGeometry(const dtk::Box2I& value)
        {
            bool changed = value != getGeometry();
            IWidget::setGeometry(value);
            DTK_P();

            const dtk::Box2I g = dtk::margin(value, -p.size.border);
            p.label->setGeometry(g);

            if (changed)
            {
                p.draw.reset();
            }
        }

        void ShortcutWidget::sizeHintEvent(const dtk::SizeHintEvent& event)
        {
            IWidget::sizeHintEvent(event);
            DTK_P();

            if (!p.size.displayScale.has_value() ||
                (p.size.displayScale.has_value() && p.size.displayScale.value() != event.displayScale))
            {
                p.size.displayScale = event.displayScale;
                p.size.border = event.style->getSizeRole(dtk::SizeRole::Border, event.displayScale);
                p.draw.reset();
            }

            _setSizeHint(_p->label->getSizeHint() + p.size.border * 2);
        }

        void ShortcutWidget::drawEvent(const dtk::Box2I& drawRect, const dtk::DrawEvent& event)
        {
            IWidget::drawEvent(drawRect, event);
            DTK_P();

            if (!p.draw.has_value())
            {
                p.draw = Private::DrawData();
                p.draw->g = getGeometry();
                p.draw->g2 = dtk::margin(p.draw->g, -p.size.border);
                p.draw->border = dtk::border(p.draw->g, p.size.border);
            }

            event.render->drawMesh(
                p.draw->border,
                event.style->getColorRole(hasKeyFocus() ? dtk::ColorRole::KeyFocus : dtk::ColorRole::Border));

            event.render->drawRect(
                p.draw->g2,
                event.style->getColorRole(p.collision ? dtk::ColorRole::Red : dtk::ColorRole::Base));

            if (_isMouseInside())
            {
                event.render->drawRect(
                    p.draw->g,
                    event.style->getColorRole(dtk::ColorRole::Hover));
            }
        }

        void ShortcutWidget::mouseEnterEvent(dtk::MouseEnterEvent& event)
        {
            IWidget::mouseEnterEvent(event);
            _setDrawUpdate();
        }

        void ShortcutWidget::mouseLeaveEvent()
        {
            IWidget::mouseLeaveEvent();
            _setDrawUpdate();
        }

        void ShortcutWidget::mousePressEvent(dtk::MouseClickEvent& event)
        {
            IWidget::mousePressEvent(event);
            takeKeyFocus();
            _setDrawUpdate();
        }

        void ShortcutWidget::keyFocusEvent(bool value)
        {
            IWidget::keyFocusEvent(value);
            _setDrawUpdate();
        }

        void ShortcutWidget::keyPressEvent(dtk::KeyEvent& event)
        {
            IWidget::keyPressEvent(event);
            DTK_P();
            switch (event.key)
            {
            case dtk::Key::Unknown: break;
            case dtk::Key::Escape:
                event.accept = true;
                releaseKeyFocus();
                break;
            case dtk::Key::Enter: break;
            case dtk::Key::Tab: break;
            case dtk::Key::CapsLock: break;
            case dtk::Key::ScrollLock: break;
            case dtk::Key::NumLock: break;
            case dtk::Key::LeftShift: break;
            case dtk::Key::LeftControl: break;
            case dtk::Key::LeftAlt: break;
            case dtk::Key::LeftSuper: break;
            case dtk::Key::RightShift: break;
            case dtk::Key::RightControl: break;
            case dtk::Key::RightAlt: break;
            case dtk::Key::RightSuper: break;
            default:
                if (hasKeyFocus())
                {
                    event.accept = true;
                    p.shortcut.key = event.key;
                    p.shortcut.modifiers = event.modifiers;
                    if (p.callback)
                    {
                        p.callback(p.shortcut);
                    }
                    _widgetUpdate();
                }
                break;
            }
        }

        void ShortcutWidget::keyReleaseEvent(dtk::KeyEvent& event)
        {
            IWidget::keyReleaseEvent(event);
            event.accept = true;
        }

        void ShortcutWidget::_widgetUpdate()
        {
            DTK_P();
            p.label->setText(dtk::getShortcutLabel(p.shortcut.key, p.shortcut.modifiers));
        }

        struct ShortcutsSettingsWidget::Private
        {
            std::shared_ptr<SettingsModel> model;
            struct Group
            {
                std::string name;
                std::vector<Shortcut> shortcuts;

                bool operator == (const Group& other) const
                {
                    bool out =
                        name == other.name &&
                        shortcuts.size() == other.shortcuts.size();
                    for (size_t i = 0; out && i < shortcuts.size(); ++i)
                    {
                        out &= shortcuts[i].name == other.shortcuts[i].name;
                    }
                    return out;
                }

                bool operator != (const Group& other) const
                {
                    return !(*this == other);
                }
            };
            std::vector<Group> groups;

            std::map<std::string, std::shared_ptr<ShortcutWidget> > widgets;
            std::vector<std::shared_ptr<dtk::GroupBox> > groupBoxes;
            std::shared_ptr<dtk::VerticalLayout> layout;

            std::shared_ptr<dtk::ValueObserver<ShortcutsSettings> > settingsObserver;
        };

        void ShortcutsSettingsWidget::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IWidget::_init(context, "tl::play_app::ShortcutsSettingsWidget", parent);
            DTK_P();

            p.model = app->getSettingsModel();

            p.layout = dtk::VerticalLayout::create(context, shared_from_this());
            p.layout->setMarginRole(dtk::SizeRole::MarginSmall);
            p.layout->setSpacingRole(dtk::SizeRole::Spacing);

            p.settingsObserver = dtk::ValueObserver<ShortcutsSettings>::create(
                p.model->observeShortcuts(),
                [this](const ShortcutsSettings& value)
                {
                    _widgetUpdate(value);
                });
        }

        ShortcutsSettingsWidget::ShortcutsSettingsWidget() :
            _p(new Private)
        {}

        ShortcutsSettingsWidget::~ShortcutsSettingsWidget()
        {}

        std::shared_ptr<ShortcutsSettingsWidget> ShortcutsSettingsWidget::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<ShortcutsSettingsWidget>(new ShortcutsSettingsWidget);
            out->_init(context, app, parent);
            return out;
        }

        void ShortcutsSettingsWidget::setGeometry(const dtk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void ShortcutsSettingsWidget::sizeHintEvent(const dtk::SizeHintEvent& event)
        {
            IWidget::sizeHintEvent(event);
            _setSizeHint(_p->layout->getSizeHint());
        }

        void ShortcutsSettingsWidget::_widgetUpdate(const ShortcutsSettings& settings)
        {
            DTK_P();

            // Create groups of shortcuts.
            std::vector<Private::Group> groups;
            for (const auto& shortcut : settings.shortcuts)
            {
                const auto s = dtk::split(shortcut.name, '/');
                if ((!s.empty() && !groups.empty() && s.front() != groups.back().name) ||
                    (!s.empty() && groups.empty()))
                {
                    Private::Group group;
                    group.name = s.front();
                    groups.push_back(group);
                }
                if (s.size() > 1 && !groups.empty())
                {
                    groups.back().shortcuts.push_back(shortcut);
                }
            }

            // Find collisions.
            std::map<std::pair<dtk::Key, int>, std::vector<std::string> > collisions;
            for (const auto& shortcut : settings.shortcuts)
            {
                if (shortcut.key != dtk::Key::Unknown)
                {
                    collisions[std::make_pair(shortcut.key, shortcut.modifiers)].push_back(shortcut.name);
                }
            }

            if (groups != p.groups)
            {
                p.groups = groups;

                // Delete the old widgets.
                p.widgets.clear();
                for (auto groupBox : p.groupBoxes)
                {
                    groupBox->setParent(nullptr);
                }
                p.groupBoxes.clear();

                // Create the new widgets.
                if (auto context = getContext())
                {
                    for (const auto& group : p.groups)
                    {
                        auto groupBox = dtk::GroupBox::create(context, group.name, p.layout);
                        p.groupBoxes.push_back(groupBox);
                        auto formLayout = dtk::FormLayout::create(context, groupBox);
                        formLayout->setSpacingRole(dtk::SizeRole::SpacingSmall);
                        for (const auto& shortcut : group.shortcuts)
                        {
                            auto widget = ShortcutWidget::create(context);
                            widget->setShortcut(shortcut);
                            p.widgets[shortcut.name] = widget;
                            formLayout->addRow(shortcut.text + ":", widget);
                            widget->setCallback(
                                [this](const Shortcut& value)
                                {
                                    DTK_P();
                                    auto settings = p.model->getShortcuts();
                                    const auto shortcut = value;
                                    const auto i = std::find_if(
                                        settings.shortcuts.begin(),
                                        settings.shortcuts.end(),
                                        [shortcut](const Shortcut& other)
                                        {
                                            return shortcut.name == other.name;
                                        });
                                    if (i != settings.shortcuts.end())
                                    {
                                        *i = value;
                                        p.model->setShortcuts(settings);
                                    }
                                });
                        }
                    }
                }
            }

            // Update the values.
            for (const auto& group : p.groups)
            {
                for (const auto& shortcut : group.shortcuts)
                {
                    const auto i = p.widgets.find(shortcut.name);
                    const auto j = std::find_if(
                        settings.shortcuts.begin(),
                        settings.shortcuts.end(),
                        [shortcut](const Shortcut& value)
                        {
                            return shortcut.name == value.name;
                        });
                    if (i != p.widgets.end() && j != settings.shortcuts.end())
                    {
                        i->second->setShortcut(*j);
                        bool collision = false;
                        const auto k = collisions.find(std::make_pair(j->key, j->modifiers));
                        if (k != collisions.end())
                        {
                            collision = k->second.size() > 1;
                        }
                        i->second->setCollision(collision);
                    }
                }
            }
        }
    }
}
