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

#include <djvUIQt/System.h>

#include <djvUIQt/ProxyStyle.h>
#include <djvUIQt/Util.h>

#include <djvUICore/FontSettings.h>
#include <djvUICore/StyleSettings.h>
#include <djvUICore/System.h>

#include <djvAV/FontSystem.h>

#include <djvCore/Context.h>

#include <QApplication>
#include <QFontDatabase>

using namespace djv::Core;

namespace djv
{
    namespace UIQt
    {
        struct System::Private
        {
            QPointer<ProxyStyle> qStyle;
            std::map<std::string, std::string> fontFileNames;
            std::shared_ptr<ValueObserver<UICore::Palette> > paletteObserver;
            std::shared_ptr<MapObserver<std::string, UICore::FontMap> > fontsObserver;
        };

        void System::_init(const std::shared_ptr<Context> & context)
        {
            ISystem::_init("djv::UIQt::System", context);
            UICore::System::create(context);
            _p->qStyle = new ProxyStyle(context);
            qApp->setStyle(_p->qStyle);
            
            _p->fontFileNames = context->getSystemT<AV::FontSystem>()->getFileNames().get();
            for (const auto & i : _p->fontFileNames)
            {
                QFontDatabase::addApplicationFont(QString::fromStdString(i.second));
            }

            auto weak = std::weak_ptr<System>(std::dynamic_pointer_cast<System>(shared_from_this()));
            auto system = context->getSystemT<UICore::System>();
            _p->paletteObserver = ValueObserver<UICore::Palette>::create(
                system->getStyleSettings()->getCurrentPalette(),
                [](const UICore::Palette & value)
            {
                QPalette palette;
                palette.setColor(QPalette::Window, toQt(value.getColor(UICore::ColorRole::Background)));
                palette.setColor(QPalette::WindowText, toQt(value.getColor(UICore::ColorRole::Foreground)));
                palette.setColor(QPalette::Base, toQt(value.getColor(UICore::ColorRole::Trough)));
                palette.setColor(QPalette::AlternateBase, toQt(value.getColor(UICore::ColorRole::Trough)));
                palette.setColor(QPalette::Text, toQt(value.getColor(UICore::ColorRole::Foreground)));
                palette.setColor(QPalette::Button, toQt(value.getColor(UICore::ColorRole::Button)));
                palette.setColor(QPalette::ButtonText, toQt(value.getColor(UICore::ColorRole::Foreground)));
                palette.setColor(QPalette::Highlight, toQt(value.getColor(UICore::ColorRole::Checked)));
                qApp->setPalette(palette);
            });
            
            _p->fontsObserver = MapObserver<std::string, UICore::FontMap>::create(
                system->getFontSettings()->getFonts(),
                [weak](const std::map<std::string, UICore::FontMap> & value)
            {
                if (auto system = weak.lock())
                {
                    const auto i = value.find("Default");
                    if (i != value.end())
                    {
                        const auto j = i->second.find(UICore::FontFace::Regular);
                        if (j != i->second.end())
                        {
                            qApp->setFont(QString::fromStdString(j->second));
                        }
                    }
                }
            });
        }

        System::System() :
            _p(new Private)
        {}

        System::~System()
        {}

        std::shared_ptr<System> System::create(const std::shared_ptr<Context> & context)
        {
            auto out = std::shared_ptr<System>(new System);
            out->_init(context);
            return out;
        }

        QPointer<QStyle> System::getQStyle() const
        {
            return _p->qStyle.data();
        }

    } // namespace UIQt
} // namespace djv

