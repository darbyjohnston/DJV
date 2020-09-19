// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

#include <djvSystem/Context.h>
#include <djvSystem/FileInfo.h>
#include <djvSystem/PathFunc.h>
#include <djvSystem/TextSystem.h>

#include <iostream>

using namespace djv::Core;

namespace djv
{
    namespace Test
    {
        struct ITest::Private
        {
            std::weak_ptr<System::Context> context;
            std::shared_ptr<System::TextSystem> textSystem;
            std::string name;
            System::File::Path tempPath;
        };
        
        ITest::ITest(
            const std::string& name,
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            _p(new Private)
        {
            _p->context = context;
            _p->textSystem = context->getSystemT<System::TextSystem>();
            _p->name = name;
            _p->tempPath = tempPath;
            
            if (!System::File::Info(tempPath).doesExist())
            {
                System::File::mkdir(tempPath);
            }
        }
        
        ITest::~ITest()
        {}
        
        const std::weak_ptr<System::Context>& ITest::getContext() const
        {
            return _p->context;
        }

        const std::string& ITest::getName() const
        {
            return _p->name;
        }

        const System::File::Path& ITest::getTempPath() const
        {
            return _p->tempPath;
        }

        std::string ITest::_getText(const std::string& id) const
        {
            DJV_PRIVATE_PTR();
            return p.textSystem ? p.textSystem->getText(id) : id;
        }

        void ITest::_print(const std::string& value)
        {
            std::cout << _p->name << ": " << value << std::endl;
        }
        
    } // namespace Test
} // namespace djv
