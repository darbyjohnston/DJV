// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2026 Darby Johnston
// All rights reserved.

#pragma once

#include <djv/UI/Export.h>

#include <ftk/UI/IMouseWidget.h>

#include <functional>
#include <string>
#include <vector>

namespace djv
{
    namespace ui
    {
        //! A control listed in a callout: its icon and its name, as the
        //! action it stands for has them.
        struct TourItem
        {
            std::string icon;
            std::string text;
        };

        //! Where a step's callout goes, relative to its control. Auto takes
        //! the first side with room, below first; a step whose control is
        //! wide, such as a section of a tool panel, says which side.
        enum class TourPlacement
        {
            Auto,
            Below,
            Above,
            Right,
            Left
        };

        //! A step of the tour: the control it points at, by its screenshot
        //! tag (see ftk::setScreenshotTag()), and what the callout says: a
        //! line of description, then the controls as a list of icons and
        //! names, then notes on the mouse and the keys as bullets.
        struct TourStep
        {
            //! A step may leave the action out, which an aggregate of the
            //! fields would warn about at every step without one.
            TourStep(
                const std::string& tag,
                const std::string& title,
                const std::string& text,
                const std::function<void(void)>& enter = nullptr) :
                tag(tag),
                title(title),
                text(text),
                enter(enter)
            {}

            //! The controls, and the notes, which a step is given after it
            //! is made.
            TourStep& withItems(const std::vector<TourItem>&);
            TourStep& withNotes(const std::vector<std::string>&);
            TourStep& withPlacement(TourPlacement);

            std::string tag;
            std::string title;
            std::string text;
            std::vector<TourItem> items;
            std::vector<std::string> notes;
            TourPlacement placement = TourPlacement::Auto;

            //! What the step does to bring its control on screen -- open a
            //! tool, say -- before the callout is placed. A step without
            //! one whose control is not visible is skipped, as is a step
            //! with one whose control has not appeared a few ticks later.
            //! The last step is always shown, so the tour ends on its
            //! closing word whatever is on screen, as is a step with no
            //! tag, whose callout sits in the middle: a welcome.
            std::function<void(void)> enter;
        };

        //! A walk through the window, in the window: everything dims except
        //! the control of the current step, which is ringed, and a callout
        //! beside it says what it is, with Back and Next. The controls are
        //! found by their screenshot tags, so the ring follows the layout
        //! as it is rather than a picture of it. The window stays live
        //! underneath: the control a step describes can be tried while the
        //! callout is up, and the ring follows whatever that changes.
        class DJV_UI_API_TYPE TourOverlay : public ftk::IMouseWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::vector<TourStep>&,
                const std::shared_ptr<IWidget>& parent);

            TourOverlay();

        public:
            DJV_UI_API virtual ~TourOverlay();

            //! Create a new overlay.
            DJV_UI_API static std::shared_ptr<TourOverlay> create(
                const std::shared_ptr<ftk::Context>&,
                const std::vector<TourStep>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            //! Open the overlay over a window, on the first step.
            DJV_UI_API void open(const std::shared_ptr<ftk::IWindow>&);

            //! Close the overlay.
            DJV_UI_API void close();

            //! Set the close callback.
            DJV_UI_API void setCloseCallback(const std::function<void(void)>&);

            //! Get the current step.
            DJV_UI_API int getStep() const;

            DJV_UI_API void setGeometry(const ftk::Box2I&) override;
            DJV_UI_API void tickEvent(bool, bool, const ftk::TickEvent&) override;
            DJV_UI_API void sizeHintEvent(const ftk::SizeHintEvent&) override;
            DJV_UI_API void drawEvent(const ftk::Box2I&, const ftk::DrawEvent&) override;
            DJV_UI_API void mouseMoveEvent(ftk::MouseMoveEvent&) override;
            DJV_UI_API void mousePressEvent(ftk::MouseClickEvent&) override;
            DJV_UI_API void mouseReleaseEvent(ftk::MouseClickEvent&) override;
            DJV_UI_API void keyPressEvent(ftk::KeyEvent&) override;
            DJV_UI_API void keyReleaseEvent(ftk::KeyEvent&) override;

        private:
            //! Go to a step, skipping in the given direction over steps
            //! whose control is not on screen; past the end, the tour is
            //! done.
            void _setStep(int, int direction);
            std::shared_ptr<IWidget> _findTarget(const std::string& tag);
            void _layout();

            FTK_PRIVATE();
        };
    }
}
