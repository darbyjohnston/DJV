// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Tools/IToolWidget.h>

#include <djvApp/Models/FilesModel.h>

namespace djv
{
    namespace app
    {
        class App;

        //! Files tool.
        class FilesTool : public IToolWidget
        {
            FEATHER_TK_NON_COPYABLE(FilesTool);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            FilesTool();

        public:
            virtual ~FilesTool();

            static std::shared_ptr<FilesTool> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

        private:
            void _filesUpdate(const std::vector<std::shared_ptr<FilesModelItem> >&);
            void _aUpdate(const std::shared_ptr<FilesModelItem>&);
            void _bUpdate(const std::vector<std::shared_ptr<FilesModelItem> >&);
            void _layersUpdate(const std::vector<int>&);
            void _compareUpdate(const tl::timeline::CompareOptions&);

            FEATHER_TK_PRIVATE();
        };
    }
}
