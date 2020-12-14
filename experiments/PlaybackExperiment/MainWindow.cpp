// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "MainWindow.h"

#include "Enum.h"
#include "IO.h"
#include "ImageWidget.h"
#include "Media.h"
#include "DrawerWidget.h"
#include "TimelineWidget.h"

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Label.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuBar.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ShortcutData.h>
#include <djvUI/ToolBar.h>

#include <djvAV/TimeFunc.h>

#include <djvImage/Data.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv;

struct MainWindow::Private
{
    std::shared_ptr<Media> media;
    Playback playback = Playback::Forward;
    IO::FrameInfo currentFrame;
    float speed = 0.F;

    std::map<std::string, std::shared_ptr<djv::UI::Action> > actions;
    std::shared_ptr<UI::ActionGroup> playbackActionGroup;
    std::shared_ptr<ImageWidget> imageWidget;
    std::shared_ptr<UI::Text::Label> speedLabel;
    std::shared_ptr<UI::Text::Label> currentTimeLabel;
    std::shared_ptr<TimelineWidget> timelineWidget;
    std::shared_ptr<UI::Drawer> drawer;

    std::shared_ptr<Core::Observer::Value<std::shared_ptr<Image::Data> > > imageObserver;
    std::shared_ptr<Core::Observer::Value<Playback> > playbackObserver;
    std::shared_ptr<Core::Observer::Value<IO::FrameInfo> > frameInfoObserver;
    std::shared_ptr<Core::Observer::Value<float> > speedObserver;
};

void MainWindow::_init(
    const std::shared_ptr<Media>& media,
    const std::shared_ptr<System::Context>& context)
{
    UI::Window::_init(context);
    DJV_PRIVATE_PTR();

    p.media = media;

    p.actions["Playback/Forward"] = UI::Action::create();
    p.actions["Playback/Forward"]->setButtonType(UI::ButtonType::Exclusive);
    p.actions["Playback/Forward"]->setIcon("djvIconPlaybackForward");
    p.actions["Playback/Forward"]->setCheckedIcon("djvIconPlaybackStop");
    p.actions["Playback/Forward"]->setShortcuts({
        UI::ShortcutData(GLFW_KEY_UP),
        UI::ShortcutData(GLFW_KEY_L) } );
    p.actions["Playback/Reverse"] = UI::Action::create();
    p.actions["Playback/Reverse"]->setButtonType(UI::ButtonType::Exclusive);
    p.actions["Playback/Reverse"]->setIcon("djvIconPlaybackReverse");
    p.actions["Playback/Reverse"]->setCheckedIcon("djvIconPlaybackStop");
    p.actions["Playback/Reverse"]->setShortcuts({
        UI::ShortcutData(GLFW_KEY_DOWN),
        UI::ShortcutData(GLFW_KEY_J) });
    p.actions["Playback/Playback"] = UI::Action::create();
    p.actions["Playback/Playback"]->setShortcuts({
        UI::ShortcutData(GLFW_KEY_SPACE),
        UI::ShortcutData(GLFW_KEY_K) });
    p.actions["Playback/StartFrame"] = UI::Action::create();
    p.actions["Playback/StartFrame"]->setIcon("djvIconFrameStart");
    p.actions["Playback/StartFrame"]->setShortcuts({
        UI::ShortcutData(GLFW_KEY_HOME) });
    p.actions["Playback/EndFrame"] = UI::Action::create();
    p.actions["Playback/EndFrame"]->setIcon("djvIconFrameEnd");
    p.actions["Playback/EndFrame"]->setShortcuts({
        UI::ShortcutData(GLFW_KEY_END) });
    p.actions["Playback/NextFrame"] = UI::Action::create();
    p.actions["Playback/NextFrame"]->setIcon("djvIconFrameNext");
    p.actions["Playback/NextFrame"]->setShortcuts({
        UI::ShortcutData(GLFW_KEY_RIGHT) });
    p.actions["Playback/PrevFrame"] = UI::Action::create();
    p.actions["Playback/PrevFrame"]->setIcon("djvIconFramePrev");
    p.actions["Playback/PrevFrame"]->setShortcuts({
        UI::ShortcutData(GLFW_KEY_LEFT) });

    p.actions["Tools/Drawer"] = UI::Action::create();
    p.actions["Tools/Drawer"]->setButtonType(UI::ButtonType::Toggle);
    p.actions["Tools/Drawer"]->setIcon("djvIconDrawerRight");
    p.actions["Tools/Drawer"]->setShortcut(GLFW_KEY_D, GLFW_MOD_CONTROL);

    for (const auto& i : p.actions)
    {
        addAction(i.second);
    }

    p.playbackActionGroup = UI::ActionGroup::create(UI::ButtonType::Exclusive);
    p.playbackActionGroup->setActions({
        p.actions["Playback/Forward"],
        p.actions["Playback/Reverse"] });

    auto playbackMenu = UI::Menu::create(context);
    playbackMenu->setText("Playback");
    playbackMenu->addAction(p.actions["Playback/Forward"]);
    playbackMenu->addAction(p.actions["Playback/Reverse"]);
    playbackMenu->addSeparator();
    playbackMenu->addAction(p.actions["Playback/StartFrame"]);
    playbackMenu->addAction(p.actions["Playback/EndFrame"]);
    playbackMenu->addAction(p.actions["Playback/NextFrame"]);
    playbackMenu->addAction(p.actions["Playback/PrevFrame"]);

    auto toolsMenu = UI::Menu::create(context);
    toolsMenu->setText("Tools");
    toolsMenu->addAction(p.actions["Tools/Drawer"]);

    auto menuBar = UI::MenuBar::create(context);;
    menuBar->addChild(playbackMenu);
    menuBar->addChild(toolsMenu);

    auto fileInfoLabel = UI::Text::Label::create(context);
    fileInfoLabel->setText(media->getFileInfo().getFileName(Math::Frame::invalid, false));
    fileInfoLabel->setTextHAlign(UI::TextHAlign::Left);
    fileInfoLabel->setTextElide(40);
    fileInfoLabel->setMargin(UI::Layout::Margin(UI::MetricsRole::Margin, UI::MetricsRole::Margin, UI::MetricsRole::MarginSmall, UI::MetricsRole::MarginSmall));

    auto toolBar = UI::ToolBar::create(context);
    toolBar->setBackgroundRole(UI::ColorRole::Background);
    toolBar->addChild(fileInfoLabel);
    toolBar->setStretch(fileInfoLabel, UI::RowStretch::Expand);
    toolBar->addSeparator();
    toolBar->addAction(p.actions["Tools/Drawer"]);

    p.imageWidget = ImageWidget::create(context);

    auto playbackToolBar = UI::ToolBar::create(context);
    playbackToolBar->setBackgroundRole(UI::ColorRole::Background);
    playbackToolBar->addAction(p.actions["Playback/StartFrame"]);
    playbackToolBar->addAction(p.actions["Playback/PrevFrame"]);
    playbackToolBar->addAction(p.actions["Playback/Reverse"]);
    playbackToolBar->addAction(p.actions["Playback/Forward"]);
    playbackToolBar->addAction(p.actions["Playback/NextFrame"]);
    playbackToolBar->addAction(p.actions["Playback/EndFrame"]);

    p.speedLabel = UI::Text::Label::create(context);
    p.speedLabel->setSizeString("00.00");

    p.currentTimeLabel = UI::Text::Label::create(context);
    p.currentTimeLabel->setSizeString("000000");

    p.timelineWidget = TimelineWidget::create(context);

    p.drawer = UI::Drawer::create(UI::Side::Right, context);

    auto layout = UI::VerticalLayout::create(context);
    layout->setSpacing(UI::MetricsRole::None);
    auto hLayout = UI::HorizontalLayout::create(context);
    hLayout->setSpacing(UI::MetricsRole::None);
    hLayout->addChild(menuBar);
    hLayout->addSeparator();
    hLayout->addChild(toolBar);
    hLayout->setStretch(toolBar, UI::RowStretch::Expand);
    layout->addChild(hLayout);
    layout->addSeparator();
    hLayout = UI::HorizontalLayout::create(context);
    hLayout->setSpacing(UI::MetricsRole::None);
    hLayout->addChild(p.imageWidget);
    hLayout->setStretch(p.imageWidget, UI::RowStretch::Expand);
    hLayout->addChild(p.drawer);
    layout->addChild(hLayout);
    layout->setStretch(hLayout, UI::RowStretch::Expand);
    layout->addSeparator();
    hLayout = UI::HorizontalLayout::create(context);
    hLayout->setSpacing(UI::MetricsRole::SpacingSmall);
    hLayout->addChild(playbackToolBar);
    hLayout->addChild(p.speedLabel);
    hLayout->addChild(p.currentTimeLabel);
    hLayout->addChild(p.timelineWidget);
    hLayout->setStretch(p.timelineWidget, UI::RowStretch::Expand);
    layout->addChild(hLayout);
    addChild(layout);

    _widgetUpdate();

    auto weak = std::weak_ptr<MainWindow>(std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
    p.actions["Playback/Playback"]->setClickedCallback(
        [weak]
        {
            if (auto widget = weak.lock())
            {
                if (auto media = widget->_p->media)
                {
                    media->setPlayback(
                        Playback::Stop == media->observePlayback()->get() ?
                        (Playback::Stop == widget->_p->playback ? Playback::Forward : widget->_p->playback) :
                        Playback::Stop);
                }
            }
        });

    p.actions["Playback/StartFrame"]->setClickedCallback(
        [weak]
        {
            if (auto widget = weak.lock())
            {
                if (auto media = widget->_p->media)
                {
                    media->frame(Frame::Start);
                }
            }
        });
    p.actions["Playback/EndFrame"]->setClickedCallback(
        [weak]
        {
            if (auto widget = weak.lock())
            {
                if (auto media = widget->_p->media)
                {
                    media->frame(Frame::End);
                }
            }
        });
    p.actions["Playback/NextFrame"]->setClickedCallback(
        [weak]
        {
            if (auto widget = weak.lock())
            {
                if (auto media = widget->_p->media)
                {
                    media->frame(Frame::Next);
                }
            }
        });
    p.actions["Playback/PrevFrame"]->setClickedCallback(
        [weak]
        {
            if (auto widget = weak.lock())
            {
                if (auto media = widget->_p->media)
                {
                    media->frame(Frame::Prev);
                }
            }
        });

    p.actions["Tools/Drawer"]->setCheckedCallback(
        [weak](bool value)
        {
            if (auto widget = weak.lock())
            {
                widget->_p->drawer->setOpen(value);
            }
        });

    p.playbackActionGroup->setExclusiveCallback(
        [weak](int value)
        {
            if (auto widget = weak.lock())
            {
                Playback playback = Playback::Stop;
                switch (value)
                {
                case 0: playback = widget->_p->playback = Playback::Forward; break;
                case 1: playback = widget->_p->playback = Playback::Reverse; break;
                }
                widget->_p->media->setPlayback(playback);
            }
        });

    p.timelineWidget->setCallback(
        [weak](const IO::FrameInfo& value)
        {
            if (auto widget = weak.lock())
            {
                widget->_p->media->seek(value.timestamp);
            }
        });

    auto contextWeak = std::weak_ptr<System::Context>(context);
    p.drawer->setOpenCallback(
        [weak, contextWeak]()->std::shared_ptr<Widget>
        {
            std::shared_ptr<Widget> out;
            if (auto context = contextWeak.lock())
            {
                if (auto widget = weak.lock())
                {
                    out = DrawerWidget::create(widget->_p->media, context);
                }
            }
            return out;
        });

    p.imageObserver = Core::Observer::Value<std::shared_ptr<Image::Data> >::create(
        media->observeImage(),
        [weak](const std::shared_ptr<Image::Data>& value)
        {
            if (auto widget = weak.lock())
            {
                widget->_p->imageWidget->setImage(value);
            }
        });

    p.playbackObserver = Core::Observer::Value<Playback>::create(
        media->observePlayback(),
        [weak](Playback value)
        {
            if (auto widget = weak.lock())
            {
                int index = -1;
                switch (value)
                {
                case Playback::Forward: index = 0; break;
                case Playback::Reverse: index = 1; break;
                }
                widget->_p->playbackActionGroup->setChecked(index);
            }
        });

    p.frameInfoObserver = Core::Observer::Value<IO::FrameInfo>::create(
        media->observeCurrentFrame(),
        [weak](const IO::FrameInfo& value)
        {
            if (auto widget = weak.lock())
            {
                widget->_p->currentFrame = value;
                widget->_widgetUpdate();
            }
        });

    p.speedObserver = Core::Observer::Value<float>::create(
        media->observeSpeed(),
        [weak](float value)
        {
            if (auto widget = weak.lock())
            {
                widget->_p->speed = value;
                widget->_widgetUpdate();
            }
        });
}

MainWindow::MainWindow() :
    _p(new Private)
{}

MainWindow::~MainWindow()
{}

std::shared_ptr<MainWindow> MainWindow::create(
    const std::shared_ptr<Media>& media,
    const std::shared_ptr<System::Context>& context)
{
    auto out = std::shared_ptr<MainWindow>(new MainWindow);
    out->_init(media, context);
    return out;
}

void MainWindow::_initEvent(System::Event::Init& event)
{
    DJV_PRIVATE_PTR();
    if (event.getData().text)
    {
        p.actions["Playback/Forward"]->setText("Forward");
        p.actions["Playback/Reverse"]->setText("Reverse");

        p.actions["Tools/Drawer"]->setText("Drawer");
    }
}

void MainWindow::_widgetUpdate()
{
    DJV_PRIVATE_PTR();

    const auto& info = p.media->getIOInfo();
    Math::Rational speed;
    std::vector<IO::FrameInfo> frameInfo;
    if (info->video.size())
    {
        speed = info->videoSpeed;
        frameInfo = info->videoFrameInfo;
    }
    else if (info->audio.isValid())
    {
        speed = Math::Rational(info->audio.sampleRate, 1);
        frameInfo = info->audioFrameInfo;
    }

    {
        std::stringstream ss;
        ss.precision(2);
        ss << std::fixed << p.speed;
        p.speedLabel->setText(ss.str());
    }
    {
        std::stringstream ss;
        ss << p.currentFrame.timestamp;
        p.currentTimeLabel->setText(ss.str());
    }

    p.timelineWidget->setSpeed(speed);
    p.timelineWidget->setFrameInfo(frameInfo);
    p.timelineWidget->setCurrentFrame(p.currentFrame);
}
