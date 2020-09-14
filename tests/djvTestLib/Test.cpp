// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>
#include <djvCore/PathFunc.h>
#include <djvCore/TextSystem.h>

#include <iostream>

using namespace djv::Core;

namespace djv
{
    namespace Test
    {
        struct ITest::Private
        {
            std::weak_ptr<Context> context;
            std::shared_ptr<TextSystem> textSystem;
            std::string name;
            FileSystem::Path tempPath;
        };
        
        ITest::ITest(
            const std::string& name,
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            _p(new Private)
        {
            _p->context = context;
            _p->textSystem = context->getSystemT<TextSystem>();
            _p->name = name;
            _p->tempPath = tempPath;
            
            if (!FileSystem::FileInfo(tempPath).doesExist())
            {
                FileSystem::mkdir(tempPath);
            }
        }
        
        ITest::~ITest()
        {}
        
        const std::weak_ptr<Context>& ITest::getContext() const
        {
            return _p->context;
        }

        const std::string& ITest::getName() const
        {
            return _p->name;
        }

        const FileSystem::Path& ITest::getTempPath() const
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
