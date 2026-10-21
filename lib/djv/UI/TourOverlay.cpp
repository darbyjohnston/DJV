// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2026 Darby Johnston
// All rights reserved.

#include <djv/UI/TourOverlay.h>

#include <ftk/UI/DrawUtil.h>
#include <ftk/UI/Icon.h>
#include <ftk/UI/IWindow.h>
#include <ftk/UI/Label.h>
#include <ftk/UI/PushButton.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/UI/ScreenshotTag.h>

#include <algorithm>
#include <list>
#include <optional>

namespace djv
{
    namespace ui
    {
        namespace
        {
            //! The width a callout's text asks for, in characters: a few
            //! short lines beside the control, not a column.
            const int textWrap = 44;

            //! How many ticks a step's action is given to bring its control
            //! on screen before the step is skipped: a tool opens on the
            //! next layout pass, a tick or two away.
            const int enterTicks = 6;

            //! How many ticks a control brought on screen by a step's action
            //! has to hold still before the callout is placed beside it: a
            //! section expands on one layout pass and its pane scrolls to
            //! it two ticks later, and a callout placed at each of those
            //! jumps.
            const int settleTicks = 3;

            //! And how long the callout waits for that at the most.
            const int settleTicksMax = 30;

            //! How many ticks a callout with new contents is kept off
            //! screen before it is placed: its labels wrap to the width
            //! they are given, and ask for their height only once they
            //! have, so a callout placed on the tick its text changed is
            //! placed again a tick later at another size, and jumps.
            const int holdTicks = 2;

            //! Whether a widget and every widget above it is shown. Asked
            //! of the widgets themselves rather than of isVisible(true),
            //! whose record of the parents is brought up to date by the
            //! tick: a tool widget just created under a hidden column
            //! would read as visible until then.
            bool shown(const std::shared_ptr<ftk::IWidget>& widget)
            {
                for (auto i = widget; i; i = i->getParent())
                {
                    if (!i->isVisible(false))
                        return false;
                }
                return true;
            }

            std::shared_ptr<ftk::IWidget> find(
                const std::shared_ptr<ftk::IWidget>& widget,
                const std::string& tag,
                const ftk::IWidget* skip)
            {
                if (!widget || widget.get() == skip)
                    return nullptr;
                // A geometry too: a widget not yet laid out still has the
                // default, which is no place to point at.
                if (ftk::hasScreenshotTag(widget) &&
                    ftk::getScreenshotTag(widget) == tag &&
                    widget->getGeometry() != ftk::Box2I() &&
                    !widget->isClipped() &&
                    shown(widget))
                {
                    return widget;
                }
                for (const auto& child : widget->getChildren())
                {
                    if (auto found = find(child, tag, skip))
                        return found;
                }
                return nullptr;
            }

            bool inside(const ftk::Box2I& box, const ftk::Box2I& in)
            {
                return
                    box.min.x >= in.min.x && box.max.x <= in.max.x &&
                    box.min.y >= in.min.y && box.max.y <= in.max.y;
            }
        }

        TourStep& TourStep::withItems(const std::vector<TourItem>& value)
        {
            items = value;
            return *this;
        }

        TourStep& TourStep::withNotes(const std::vector<std::string>& value)
        {
            notes = value;
            return *this;
        }

        TourStep& TourStep::withPlacement(TourPlacement value)
        {
            placement = value;
            return *this;
        }

        struct TourOverlay::Private
        {
            std::vector<TourStep> steps;
            int step = -1;

            //! A step whose action is waiting for its control to appear
            //! and hold still: the direction to skip in if it does not
            //! appear, the ticks so far, and where the control was last
            //! seen with how long it has been there. Zero when nothing is
            //! pending.
            int pending = 0;
            int pendingTicks = 0;
            std::optional<ftk::Box2I> pendingTarget;
            int pendingStill = 0;

            //! Ticks for which the callout is kept off screen: laid out,
            //! so that its labels wrap and its size settles, but below the
            //! window and not drawn.
            int hold = 0;

            //! The key focus before the tour, to give back.
            std::weak_ptr<IWidget> restoreFocus;

            std::shared_ptr<ftk::VerticalLayout> callout;
            std::shared_ptr<ftk::Label> titleLabel;
            std::shared_ptr<ftk::Label> textLabel;
            std::shared_ptr<ftk::VerticalLayout> itemsLayout;
            std::shared_ptr<ftk::VerticalLayout> notesLayout;
            std::shared_ptr<ftk::PushButton> prevButton;
            std::shared_ptr<ftk::PushButton> nextButton;
            std::shared_ptr<ftk::PushButton> closeButton;

            //! Where the current step's control is, or nothing when it is
            //! not on screen; the cutout in the dimming is this, and the
            //! ring is drawn just inside its edge.
            std::optional<ftk::Box2I> target;
            std::optional<ftk::Box2I> cutout;

            std::function<void(void)> closeCallback;

            struct SizeData
            {
                bool init = true;
                int margin = 0;
                int spacing = 0;
                int border = 0;
                int shadow = 0;
                int cornerRadius = 0;
                int ring = 0;

                //! The callout's width: the text's wrap width with the
                //! margins. A label that wraps to fit asks only for the
                //! width of its lines as last wrapped, so a callout sized
                //! from its contents would keep the width of the step
                //! before it, and a step after a narrow one would come out
                //! narrow and tall.
                int width = 0;
            };
            SizeData size;

            struct DrawData
            {
                std::vector<ftk::Box2I> dim;
                ftk::TriMesh2F ring;
                ftk::TriMesh2F shadow;
                ftk::TriMesh2F border;
                ftk::TriMesh2F bg;
            };
            std::optional<DrawData> draw;
        };

        void TourOverlay::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::vector<TourStep>& steps,
            const std::shared_ptr<IWidget>& parent)
        {
            IMouseWidget::_init(context, "djv::ui::TourOverlay", parent);
            FTK_P();

            p.steps = steps;

            // Only the callout takes the mouse (see mousePressEvent()); a
            // press anywhere else goes on to the window underneath, so the
            // controls can be tried.
            _setMouseHoverEnabled(false);
            _setMousePressEnabled(false);

            p.callout = ftk::VerticalLayout::create(context, shared_from_this());
            p.callout->setMarginRole(ftk::SizeRole::Margin);
            p.callout->setSpacingRole(ftk::SizeRole::Spacing);
            p.titleLabel = ftk::Label::create(context, p.callout);
            p.titleLabel->setFont(ftk::FontType::Bold);
            ftk::setScreenshotTag(p.titleLabel, "Tour.Title");
            p.textLabel = ftk::Label::create(context, p.callout);
            p.textLabel->setWrap(textWrap);
            p.textLabel->setWrapToFit(true);
            ftk::setScreenshotTag(p.textLabel, "Tour.Text");
            p.itemsLayout = ftk::VerticalLayout::create(context, p.callout);
            p.itemsLayout->setSpacingRole(ftk::SizeRole::SpacingSmall);
            p.notesLayout = ftk::VerticalLayout::create(context, p.callout);
            p.notesLayout->setSpacingRole(ftk::SizeRole::SpacingSmall);
            auto buttons = ftk::HorizontalLayout::create(context, p.callout);
            buttons->setSpacingRole(ftk::SizeRole::SpacingSmall);
            p.prevButton = ftk::PushButton::create(context, "Back", buttons);
            ftk::setScreenshotTag(p.prevButton, "Tour.Prev");
            p.nextButton = ftk::PushButton::create(context, "Next", buttons);
            ftk::setScreenshotTag(p.nextButton, "Tour.Next");
            buttons->addSpacer(ftk::SizeRole::Spacing, ftk::Stretch::Expanding);
            p.closeButton = ftk::PushButton::create(context, "Close", buttons);
            ftk::setScreenshotTag(p.closeButton, "Tour.Close");

            p.prevButton->setClickedCallback(
                [this]
                {
                    _setStep(_p->step - 1, -1);
                });
            p.nextButton->setClickedCallback(
                [this]
                {
                    _setStep(_p->step + 1, 1);
                });
            p.closeButton->setClickedCallback(
                [this]
                {
                    close();
                });
        }

        TourOverlay::TourOverlay() :
            _p(new Private)
        {}

        TourOverlay::~TourOverlay()
        {}

        std::shared_ptr<TourOverlay> TourOverlay::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::vector<TourStep>& steps,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<TourOverlay>(new TourOverlay);
            out->_init(context, steps, parent);
            return out;
        }

        void TourOverlay::open(const std::shared_ptr<ftk::IWindow>& window)
        {
            FTK_P();
            // Next has the key focus, so Return goes on; a push button takes
            // no other key, so Space still plays and the arrows still step,
            // which is what a step says to try. The focus is given back when
            // the tour ends.
            p.restoreFocus = window->getKeyFocus();
            setParent(window);
            window->setKeyFocusVisible(true);
            _setStep(0, 1);
        }

        void TourOverlay::close()
        {
            FTK_P();
            std::shared_ptr<IWidget> restoreFocus;
            if (containsKeyFocus())
            {
                restoreFocus = p.restoreFocus.lock();
            }
            p.restoreFocus.reset();
            setParent(nullptr);
            if (restoreFocus)
            {
                restoreFocus->takeKeyFocus();
            }
            if (p.closeCallback)
            {
                p.closeCallback();
            }
        }

        void TourOverlay::setCloseCallback(const std::function<void(void)>& value)
        {
            _p->closeCallback = value;
        }

        int TourOverlay::getStep() const
        {
            return _p->step;
        }

        void TourOverlay::setGeometry(const ftk::Box2I& value)
        {
            IMouseWidget::setGeometry(value);
            // Every layout pass, whether or not this geometry changed: the
            // control a step points at may have moved, or appeared.
            _layout();
        }

        void TourOverlay::tickEvent(
            bool parentsVisible,
            bool parentsEnabled,
            const ftk::TickEvent& event)
        {
            IMouseWidget::tickEvent(parentsVisible, parentsEnabled, event);
            FTK_P();
            // A control that appears or moves without a layout pass of the
            // window -- or that is clipped in and out -- is followed here.
            if (p.step >= 0 && p.step < static_cast<int>(p.steps.size()))
            {
                std::optional<ftk::Box2I> target;
                if (auto widget = _findTarget(p.steps[p.step].tag))
                {
                    target = widget->getGeometry();
                }
                if (p.pending)
                {
                    // The step's action is given a few ticks to bring its
                    // control on screen -- a tool opening -- and the step
                    // is skipped when it cannot, as when the tools are
                    // hidden in presentation mode. The callout waits with
                    // it, so there is no callout pointing at nothing, and
                    // waits again until the control holds still, so there
                    // is no callout following it around as it is laid out
                    // and scrolled to.
                    ++p.pendingTicks;
                    if (target.has_value())
                    {
                        if (target == p.pendingTarget)
                        {
                            ++p.pendingStill;
                        }
                        else
                        {
                            p.pendingTarget = target;
                            p.pendingStill = 0;
                        }
                        if (p.pendingStill >= settleTicks ||
                            p.pendingTicks > settleTicksMax)
                        {
                            p.pending = 0;
                            _layout();
                            return;
                        }
                    }
                    else if (!p.pendingTarget.has_value() &&
                        p.pendingTicks > enterTicks)
                    {
                        const int direction = p.pending;
                        p.pending = 0;
                        _setStep(p.step + direction, direction);
                        return;
                    }
                }
                else if (p.hold > 0 && 0 == --p.hold)
                {
                    // Settled: placed where it belongs.
                    _layout();
                    return;
                }
                if (target != p.target)
                {
                    _layout();
                }
            }
        }

        void TourOverlay::sizeHintEvent(const ftk::SizeHintEvent& event)
        {
            IMouseWidget::sizeHintEvent(event);
            FTK_P();
            if (p.size.init)
            {
                p.size.init = false;
                p.size.margin = event.style->getSizeRole(ftk::SizeRole::Margin, event.displayScale);
                p.size.spacing = event.style->getSizeRole(ftk::SizeRole::Spacing, event.displayScale);
                p.size.border = event.style->getSizeRole(ftk::SizeRole::Border, event.displayScale);
                p.size.shadow = event.style->getSizeRole(ftk::SizeRole::Shadow, event.displayScale);
                p.size.cornerRadius = event.style->getSizeRole(ftk::SizeRole::CornerRadius, event.displayScale);
                p.size.ring = event.style->getSizeRole(ftk::SizeRole::KeyFocus, event.displayScale);
                p.size.width =
                    ftk::getCharsWidth(
                        event.fontSystem,
                        event.style->getFont(ftk::FontType::Regular, event.displayScale),
                        textWrap) +
                    p.size.margin * 2;
                p.draw.reset();
            }
        }

        void TourOverlay::drawEvent(
            const ftk::Box2I& drawRect,
            const ftk::DrawEvent& event)
        {
            IMouseWidget::drawEvent(drawRect, event);
            FTK_P();

            const ftk::Box2I& g = getGeometry();
            if (!p.draw.has_value())
            {
                p.draw = Private::DrawData();

                // The dimming: the window less the cutout, in four pieces
                // around it, or all of it when the step has nothing to
                // point at.
                if (p.cutout.has_value())
                {
                    // (The max of an integer box is inclusive.)
                    const ftk::Box2I& c = p.cutout.value();
                    p.draw->dim.push_back(ftk::Box2I(
                        g.min.x, g.min.y, g.w(), c.min.y - g.min.y));
                    p.draw->dim.push_back(ftk::Box2I(
                        g.min.x, c.max.y + 1, g.w(), g.max.y - c.max.y));
                    p.draw->dim.push_back(ftk::Box2I(
                        g.min.x, c.min.y, c.min.x - g.min.x, c.h()));
                    p.draw->dim.push_back(ftk::Box2I(
                        c.max.x + 1, c.min.y, g.max.x - c.max.x, c.h()));
                    // Inside the control's edge: a ring outside it is
                    // clipped where the control runs to the window's
                    // edge, as the viewport does.
                    p.draw->ring = ftk::border(
                        c,
                        p.size.ring,
                        p.size.cornerRadius);
                }
                else
                {
                    p.draw->dim.push_back(g);
                }

                const ftk::Box2I& b = p.callout->getGeometry();
                p.draw->shadow = ftk::shadow(
                    ftk::Box2I(
                        b.min.x - p.size.shadow,
                        b.min.y,
                        b.w() + p.size.shadow * 2,
                        b.h() + p.size.shadow),
                    p.size.shadow);
                p.draw->border = ftk::border(
                    ftk::margin(b, p.size.border),
                    p.size.border,
                    p.size.cornerRadius + p.size.border);
                p.draw->bg = ftk::rect(b, p.size.cornerRadius);
            }

            const ftk::Color4F dim = event.style->getColorRole(ftk::ColorRole::Overlay);
            for (const auto& box : p.draw->dim)
            {
                if (box.w() > 0 && box.h() > 0)
                {
                    event.render->drawRect(box, dim);
                }
            }
            if (p.cutout.has_value())
            {
                event.render->drawMesh(
                    p.draw->ring,
                    event.style->getColorRole(ftk::ColorRole::KeyFocus));
            }
            if (!p.pending && 0 == p.hold)
            {
                event.render->drawColorMesh(p.draw->shadow);
                event.render->drawMesh(
                    p.draw->border,
                    event.style->getColorRole(ftk::ColorRole::Border));
                event.render->drawMesh(
                    p.draw->bg,
                    event.style->getColorRole(ftk::ColorRole::Window));
            }
        }

        void TourOverlay::mouseMoveEvent(ftk::MouseMoveEvent& event)
        {
            IMouseWidget::mouseMoveEvent(event);
            // The callout's own ground, between its widgets, is the
            // callout's; anywhere else the window underneath hovers.
            event.accept = ftk::contains(_p->callout->getGeometry(), event.pos);
        }

        void TourOverlay::mousePressEvent(ftk::MouseClickEvent& event)
        {
            IMouseWidget::mousePressEvent(event);
            event.accept = ftk::contains(_p->callout->getGeometry(), event.pos);
        }

        void TourOverlay::mouseReleaseEvent(ftk::MouseClickEvent& event)
        {
            IMouseWidget::mouseReleaseEvent(event);
            event.accept = ftk::contains(_p->callout->getGeometry(), event.pos);
        }

        void TourOverlay::keyPressEvent(ftk::KeyEvent& event)
        {
            // Escape ends the tour; every other key is the window's, so
            // the shortcuts a step mentions can be tried.
            if (ftk::Key::Escape == event.key && 0 == event.modifiers)
            {
                event.accept = true;
                close();
            }
        }

        void TourOverlay::keyReleaseEvent(ftk::KeyEvent& event)
        {}

        void TourOverlay::_setStep(int index, int direction)
        {
            FTK_P();
            const int count = static_cast<int>(p.steps.size());
            while (index >= 0 && index < count)
            {
                const auto& step = p.steps[index];
                // The last step is shown whatever is on screen, so the
                // tour ends on its closing word even in presentation mode,
                // where only the viewport is left; so is a step that points
                // at nothing, which is a word to the user.
                if (index == count - 1 || step.tag.empty() || step.enter || _findTarget(step.tag))
                    break;
                index += direction;
            }
            if (index >= count)
            {
                // Next on the last step, or every later step off screen.
                close();
                return;
            }
            if (index < 0)
            {
                // Back with nothing to go back to: stay.
                return;
            }
            p.step = index;
            const auto& step = p.steps[index];
            p.pending = 0;
            p.hold = holdTicks;
            if (step.enter && index != count - 1)
            {
                step.enter();
                // Whether or not the control is there already: what the
                // action did has still to be laid out.
                p.pending = direction;
                p.pendingTicks = 0;
                p.pendingTarget.reset();
                p.pendingStill = 0;
            }
            p.titleLabel->setText(step.title);
            p.textLabel->setText(step.text);
            p.textLabel->setVisible(!step.text.empty());

            // The controls, each as its icon and its name, and the notes
            // as bullets. Made afresh for each step: the layouts hold
            // nothing between steps.
            auto context = getContext();
            for (const auto& child : std::list<std::shared_ptr<IWidget> >(p.itemsLayout->getChildren()))
            {
                child->setParent(nullptr);
            }
            for (const auto& item : step.items)
            {
                auto row = ftk::HorizontalLayout::create(context, p.itemsLayout);
                row->setSpacingRole(ftk::SizeRole::SpacingSmall);
                ftk::Icon::create(context, item.icon, row);
                ftk::Label::create(context, item.text, row);
            }
            p.itemsLayout->setVisible(!step.items.empty());
            for (const auto& child : std::list<std::shared_ptr<IWidget> >(p.notesLayout->getChildren()))
            {
                child->setParent(nullptr);
            }
            for (const auto& note : step.notes)
            {
                // The bullet in its own column, so a note that wraps hangs
                // under its first word rather than under the bullet.
                auto row = ftk::HorizontalLayout::create(context, p.notesLayout);
                row->setSpacingRole(ftk::SizeRole::SpacingSmall);
                auto bullet = ftk::Label::create(context, "\u2022", row);
                bullet->setVAlign(ftk::VAlign::Top);
                auto label = ftk::Label::create(context, note, row);
                label->setWrap(textWrap - 2);
                label->setWrapToFit(true);
            }
            p.notesLayout->setVisible(!step.notes.empty());
            p.prevButton->setEnabled(index > 0);
            const bool last = index == count - 1;
            p.nextButton->setText(last ? "Done" : "Next");
            p.closeButton->setVisible(!last);
            _layout();
        }

        std::shared_ptr<ftk::IWidget> TourOverlay::_findTarget(const std::string& tag)
        {
            std::shared_ptr<IWidget> out;
            if (auto window = getWindow())
            {
                out = find(window, tag, this);
            }
            return out;
        }

        void TourOverlay::_layout()
        {
            FTK_P();
            const ftk::Box2I& g = getGeometry();
            if (!g.isValid())
                return;

            p.target.reset();
            p.cutout.reset();
            if (p.step >= 0 && p.step < static_cast<int>(p.steps.size()))
            {
                if (auto widget = _findTarget(p.steps[p.step].tag))
                {
                    p.target = widget->getGeometry();
                    // What shows of it: a section of a tool taller than
                    // the tool's pane is ringed where it is seen, not down
                    // through whatever is under the pane.
                    ftk::Box2I cutout = ftk::intersect(p.target.value(), g);
                    for (auto parent = widget->getParent(); parent; parent = parent->getParent())
                    {
                        if (parent->doesClipChildren())
                        {
                            cutout = ftk::intersect(cutout, parent->getChildrenClipRect());
                        }
                    }
                    if (cutout.isValid())
                    {
                        p.cutout = cutout;
                    }
                }
            }

            // The callout: below the control, or above, to the right or
            // the left, the first with room for it; for a control too big
            // to have room beside it -- the viewport -- its lower right
            // corner; otherwise wherever the most of it fits. Without a
            // control, the middle.
            const ftk::Size2I hint = p.callout->getSizeHint();
            const int w = std::min(std::max(hint.w, p.size.width), g.w() - p.size.margin * 2);
            const int h = std::min(hint.h, g.h() - p.size.margin * 2);
            ftk::Box2I box;
            if (p.cutout.has_value())
            {
                const ftk::Box2I& c = p.cutout.value();
                const int gap = p.size.spacing + p.size.shadow;
                const int cx = c.min.x + c.w() / 2 - w / 2;
                const int cy = c.min.y + c.h() / 2 - h / 2;
                std::vector<ftk::Box2I> boxes =
                {
                    ftk::Box2I(cx, c.max.y + 1 + gap, w, h),
                    ftk::Box2I(cx, c.min.y - gap - h, w, h),
                    ftk::Box2I(c.max.x + 1 + gap, cy, w, h),
                    ftk::Box2I(c.min.x - gap - w, cy, w, h)
                };
                // The step's own choice of side first.
                const TourPlacement placement =
                    p.step >= 0 && p.step < static_cast<int>(p.steps.size()) ?
                    p.steps[p.step].placement :
                    TourPlacement::Auto;
                if (placement != TourPlacement::Auto)
                {
                    const size_t i = static_cast<size_t>(placement) - 1;
                    std::rotate(boxes.begin(), boxes.begin() + i, boxes.begin() + i + 1);
                }
                bool found = false;
                for (const auto& i : boxes)
                {
                    if (inside(i, g))
                    {
                        box = i;
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    const ftk::Box2I corner(
                        c.max.x - gap - w,
                        c.max.y - gap - h,
                        w,
                        h);
                    if (inside(corner, c))
                    {
                        box = corner;
                        found = true;
                    }
                }
                if (!found)
                {
                    float best = -1.F;
                    for (const auto& i : boxes)
                    {
                        const float a = ftk::area(ftk::intersect(i, g));
                        if (a > best)
                        {
                            best = a;
                            box = i;
                        }
                    }
                }
            }
            else
            {
                box = ftk::Box2I(
                    g.min.x + g.w() / 2 - w / 2,
                    g.min.y + g.h() / 2 - h / 2,
                    w,
                    h);
            }
            if (box.max.x > g.max.x - p.size.margin)
            {
                const int d = box.max.x - (g.max.x - p.size.margin);
                box.min.x -= d;
                box.max.x -= d;
            }
            if (box.min.x < g.min.x + p.size.margin)
            {
                const int d = g.min.x + p.size.margin - box.min.x;
                box.min.x += d;
                box.max.x += d;
            }
            if (box.max.y > g.max.y - p.size.margin)
            {
                const int d = box.max.y - (g.max.y - p.size.margin);
                box.min.y -= d;
                box.max.y -= d;
            }
            if (box.min.y < g.min.y + p.size.margin)
            {
                const int d = g.min.y + p.size.margin - box.min.y;
                box.min.y += d;
                box.max.y += d;
            }
            // Off screen while the step's control is still moving or the
            // callout's own size is still settling: laid out, so its labels
            // wrap and ask for their height, but neither seen nor jumping.
            if (p.pending || p.hold > 0)
            {
                box = ftk::move(box, ftk::V2I(0, g.h()));
            }
            p.callout->setGeometry(box);
            if (!p.pending && 0 == p.hold)
            {
                // Next has the key focus once the callout is on screen,
                // with the ring shown: a widget gives the focus up when it
                // is clipped, as the callout is while it waits off screen,
                // and a mouse press hides the ring.
                p.nextButton->takeKeyFocus();
                if (auto window = getWindow())
                {
                    window->setKeyFocusVisible(true);
                }
            }

            p.draw.reset();
            setDrawUpdate();
        }
    }
}
