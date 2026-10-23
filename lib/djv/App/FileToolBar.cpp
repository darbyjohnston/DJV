// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the DJV project.

#include <djv/App/FileToolBar.h>

#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif // __APPLE__

namespace djv
{
    namespace app
    {
        void FileToolBar::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::map<std::string, std::shared_ptr<ftk::Action> >& actions,
            const std::shared_ptr<IWidget>& parent)
        {
            ToolBar::_init(context, ftk::Orientation::Horizontal, parent);

            setMarginRole(ftk::SizeRole::MarginInside);

            auto tmp = actions;
            addAction(tmp["Open"]);
#if defined(__APPLE__) && TARGET_OS_IPHONE
            // The system's picker opens one file at a time; a folder, and
            // the browser inside it, is how an image sequence is opened.
            addAction(tmp["OpenFolder"]);
#endif // TARGET_OS_IPHONE
            addAction(tmp["OpenAudio"]);
            addAction(tmp["Close"]);
            addAction(tmp["CloseAll"]);
            addAction(tmp["Reload"]);
        }

        FileToolBar::~FileToolBar()
        {}

        std::shared_ptr<FileToolBar> FileToolBar::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::map<std::string, std::shared_ptr<ftk::Action> >& actions,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<FileToolBar>(new FileToolBar);
            out->_init(context, actions, parent);
            return out;
        }
    }
}
