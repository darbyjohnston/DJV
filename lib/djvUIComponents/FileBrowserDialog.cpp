//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvUIComponents/FileBrowserDialog.h>

#include <djvUIComponents/FileBrowser.h>

#include <djvUI/IDialog.h>
#include <djvUI/IWindowSystem.h>
#include <djvUI/Label.h>
#include <djvUI/Overlay.h>
#include <djvUI/PushButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/TextBlock.h>
#include <djvUI/ToolButton.h>
#include <djvUI/Window.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
		namespace FileBrowser
		{
			namespace
			{
				class Dialog : public IDialog
				{
					DJV_NON_COPYABLE(Dialog);

				protected:
					void _init(Context * context)
					{
						IDialog::_init(context);

						_widget = FileBrowser::Widget::create(context);
						_widget->setPath(FileSystem::Path("."));
						_widget->setBackgroundRole(ColorRole::Background);
						addWidget(_widget, RowStretch::Expand);

						auto weak = std::weak_ptr<Dialog>(std::dynamic_pointer_cast<Dialog>(shared_from_this()));
						_widget->setCallback(
							[weak](const FileSystem::FileInfo & value)
						{
							if (auto dialog = weak.lock())
							{
								if (dialog->_callback)
								{
									dialog->_callback(value);
								}
								dialog->_doCloseCallback();
							}
						});
					}

					Dialog()
					{}

				public:
					static std::shared_ptr<Dialog> create(Context * context)
					{
						auto out = std::shared_ptr<Dialog>(new Dialog);
						out->_init(context);
						return out;
					}

					void setCallback(const std::function<void(const FileSystem::FileInfo &)> & value)
					{
						_callback = value;
					}

				private:
					std::shared_ptr<FileBrowser::Widget> _widget;
					std::function<void(const FileSystem::FileInfo &)> _callback;
				};

			} // namespace

			struct DialogSystem::Private
			{
				std::shared_ptr<Dialog> dialog;
			};

			void DialogSystem::_init(Context * context)
			{
				ISystem::_init("djv::UI::FileBrowser::DialogSystem", context);
			}

			DialogSystem::DialogSystem() :
				_p(new Private)
			{}

			DialogSystem::~DialogSystem()
			{}

			std::shared_ptr<DialogSystem> DialogSystem::create(Context * context)
			{
				auto out = std::shared_ptr<DialogSystem>(new DialogSystem);
				out->_init(context);
				return out;
			}

			void DialogSystem::fileBrowser(
				const std::string & title,
				const std::function<void(const Core::FileSystem::FileInfo &)> & callback)
			{
				auto context = getContext();
				DJV_PRIVATE_PTR();
				if (!p.dialog)
				{
					p.dialog = Dialog::create(context);
				}
				if (auto windowSystem = context->getSystemT<UI::IWindowSystem>().lock())
				{
					if (auto window = windowSystem->observeCurrentWindow()->get())
					{
						p.dialog->setTitle(title);

						auto weak = std::weak_ptr<DialogSystem>(std::dynamic_pointer_cast<DialogSystem>(shared_from_this()));
						p.dialog->setCallback(
							[callback](const Core::FileSystem::FileInfo & value)
						{
							callback(value);
						});
						p.dialog->setCloseCallback(
							[weak, window]
						{
							if (auto system = weak.lock())
							{
								window->removeWidget(system->_p->dialog);
							}
						});

						window->addWidget(p.dialog);
						p.dialog->show();
					}
				}
			}

		} // namespace FileBrowser
    } // namespace UI
} // namespace djv
