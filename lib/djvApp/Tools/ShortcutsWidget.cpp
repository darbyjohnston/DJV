// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Tools/SettingsToolPrivate.h>

#include <djvApp/App.h>

#include <feather-tk/ui/GroupBox.h>
#include <feather-tk/ui/DrawUtil.h>
#include <feather-tk/ui/FormLayout.h>
#include <feather-tk/ui/Label.h>
#include <feather-tk/ui/RowLayout.h>

namespace djv
{
    namespace app
    {
        struct ShortcutWidget::Private
        {
            Shortcut shortcut;
            bool collision = false;

            std::shared_ptr<feather_tk::Label> label;

            std::function<void(const Shortcut&)> callback;

            struct SizeData
            {
                std::optional<float> displayScale;
                int border = 0;
            };
            SizeData size;

            struct DrawData
            {
                feather_tk::Box2I g;
                feather_tk::Box2I g2;
                feather_tk::TriMesh2F border;
            };
            std::optional<DrawData> draw;
        };

        void ShortcutWidget::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<IWidget>& parent)
        {
            IWidget::_init(context, "djv::app::ShortcutWidget", parent);
            FEATHER_TK_P();
            
            setHStretch(feather_tk::Stretch::Expanding);
            setAcceptsKeyFocus(true);
            _setMouseHoverEnabled(true);
            _setMousePressEnabled(true);

            p.label = feather_tk::Label::create(context, shared_from_this());
            p.label->setMarginRole(feather_tk::SizeRole::MarginInside);

            _widgetUpdate();
        }

        ShortcutWidget::ShortcutWidget() :
            _p(new Private)
        {}

        ShortcutWidget::~ShortcutWidget()
        {}

        std::shared_ptr<ShortcutWidget> ShortcutWidget::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<ShortcutWidget>(new ShortcutWidget);
            out->_init(context, parent);
            return out;
        }

        void ShortcutWidget::setShortcut(const Shortcut& value)
        {
            FEATHER_TK_P();
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
            FEATHER_TK_P();
            if (value == p.collision)
                return;
            p.collision = value;
            _widgetUpdate();
        }

        void ShortcutWidget::setGeometry(const feather_tk::Box2I& value)
        {
            bool changed = value != getGeometry();
            IWidget::setGeometry(value);
            FEATHER_TK_P();

            const feather_tk::Box2I g = feather_tk::margin(value, -p.size.border);
            p.label->setGeometry(g);

            if (changed)
            {
                p.draw.reset();
            }
        }

        void ShortcutWidget::sizeHintEvent(const feather_tk::SizeHintEvent& event)
        {
            IWidget::sizeHintEvent(event);
            FEATHER_TK_P();

            if (!p.size.displayScale.has_value() ||
                (p.size.displayScale.has_value() && p.size.displayScale.value() != event.displayScale))
            {
                p.size.displayScale = event.displayScale;
                p.size.border = event.style->getSizeRole(feather_tk::SizeRole::Border, event.displayScale);
                p.draw.reset();
            }

            _setSizeHint(_p->label->getSizeHint() + p.size.border * 2);
        }

        void ShortcutWidget::drawEvent(const feather_tk::Box2I& drawRect, const feather_tk::DrawEvent& event)
        {
            IWidget::drawEvent(drawRect, event);
            FEATHER_TK_P();

            if (!p.draw.has_value())
            {
                p.draw = Private::DrawData();
                p.draw->g = getGeometry();
                p.draw->g2 = feather_tk::margin(p.draw->g, -p.size.border);
                p.draw->border = feather_tk::border(p.draw->g, p.size.border);
            }

            event.render->drawMesh(
                p.draw->border,
                event.style->getColorRole(hasKeyFocus() ? feather_tk::ColorRole::KeyFocus : feather_tk::ColorRole::Border));

            event.render->drawRect(
                p.draw->g2,
                event.style->getColorRole(p.collision ? feather_tk::ColorRole::Red : feather_tk::ColorRole::Base));

            if (_isMouseInside())
            {
                event.render->drawRect(
                    p.draw->g,
                    event.style->getColorRole(feather_tk::ColorRole::Hover));
            }
        }

        void ShortcutWidget::mouseEnterEvent(feather_tk::MouseEnterEvent& event)
        {
            IWidget::mouseEnterEvent(event);
            _setDrawUpdate();
        }

        void ShortcutWidget::mouseLeaveEvent()
        {
            IWidget::mouseLeaveEvent();
            _setDrawUpdate();
        }

        void ShortcutWidget::mousePressEvent(feather_tk::MouseClickEvent& event)
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

        void ShortcutWidget::keyPressEvent(feather_tk::KeyEvent& event)
        {
            IWidget::keyPressEvent(event);
            FEATHER_TK_P();
            switch (event.key)
            {
            case feather_tk::Key::Unknown: break;
            case feather_tk::Key::Escape:
                event.accept = true;
                releaseKeyFocus();
                break;
            case feather_tk::Key::Enter: break;
            case feather_tk::Key::Tab: break;
            case feather_tk::Key::CapsLock: break;
            case feather_tk::Key::ScrollLock: break;
            case feather_tk::Key::NumLock: break;
            case feather_tk::Key::LeftShift: break;
            case feather_tk::Key::LeftControl: break;
            case feather_tk::Key::LeftAlt: break;
            case feather_tk::Key::LeftSuper: break;
            case feather_tk::Key::RightShift: break;
            case feather_tk::Key::RightControl: break;
            case feather_tk::Key::RightAlt: break;
            case feather_tk::Key::RightSuper: break;
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

        void ShortcutWidget::keyReleaseEvent(feather_tk::KeyEvent& event)
        {
            IWidget::keyReleaseEvent(event);
            event.accept = true;
        }

        void ShortcutWidget::_widgetUpdate()
        {
            FEATHER_TK_P();
            p.label->setText(feather_tk::getShortcutLabel(p.shortcut.key, p.shortcut.modifiers));
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
            std::vector<std::shared_ptr<feather_tk::GroupBox> > groupBoxes;
            std::shared_ptr<feather_tk::VerticalLayout> layout;

            std::shared_ptr<feather_tk::ValueObserver<ShortcutsSettings> > settingsObserver;
        };

        void ShortcutsSettingsWidget::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            ISettingsWidget::_init(context, "djv::app::ShortcutsSettingsWidget", parent);
            FEATHER_TK_P();

            p.model = app->getSettingsModel();

            p.layout = feather_tk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(feather_tk::SizeRole::Spacing);

            p.settingsObserver = feather_tk::ValueObserver<ShortcutsSettings>::create(
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
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<ShortcutsSettingsWidget>(new ShortcutsSettingsWidget);
            out->_init(context, app, parent);
            return out;
        }

        void ShortcutsSettingsWidget::setMarginRole(feather_tk::SizeRole value)
        {
            _p->layout->setMarginRole(value);
        }

        void ShortcutsSettingsWidget::setGeometry(const feather_tk::Box2I& value)
        {
            ISettingsWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void ShortcutsSettingsWidget::sizeHintEvent(const feather_tk::SizeHintEvent& event)
        {
            ISettingsWidget::sizeHintEvent(event);
            _setSizeHint(_p->layout->getSizeHint());
        }

        void ShortcutsSettingsWidget::_widgetUpdate(const ShortcutsSettings& settings)
        {
            FEATHER_TK_P();

            // Create groups of shortcuts.
            std::vector<Private::Group> groups;
            for (const auto& shortcut : settings.shortcuts)
            {
                const auto s = feather_tk::split(shortcut.name, '/');
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
            std::map<std::pair<feather_tk::Key, int>, std::vector<std::string> > collisions;
            for (const auto& shortcut : settings.shortcuts)
            {
                if (shortcut.key != feather_tk::Key::Unknown)
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
                        auto groupBox = feather_tk::GroupBox::create(context, group.name, p.layout);
                        p.groupBoxes.push_back(groupBox);
                        auto formLayout = feather_tk::FormLayout::create(context, groupBox);
                        formLayout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
                        for (const auto& shortcut : group.shortcuts)
                        {
                            auto widget = ShortcutWidget::create(context);
                            widget->setShortcut(shortcut);
                            p.widgets[shortcut.name] = widget;
                            formLayout->addRow(shortcut.text + ":", widget);
                            widget->setCallback(
                                [this](const Shortcut& value)
                                {
                                    FEATHER_TK_P();
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
