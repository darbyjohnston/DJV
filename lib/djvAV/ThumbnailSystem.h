// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvImage/Type.h>

#include <djvSystem/ISystem.h>

#include <djvCore/UID.h>

#include <future>

namespace djv
{
    namespace System
    {
        namespace File
        {
            class Info;

        } // namespace File
    } // namespace System

    namespace Image
    {
        class Data;
        class Info;
        class Size;
        
    } // namespace Image

    namespace GL
    {
        class ImageConvert;
        
    } // namespace GL

    namespace AV
    {
        namespace IO
        {
            class Info;

        } // namespace IO
            
        //! Thumbnail error.
        class ThumbnailError : public std::runtime_error
        {
        public:
            explicit ThumbnailError(const std::string&);
        };
        
        //! Thumbnail system.
        class ThumbnailSystem : public System::ISystem
        {
            DJV_NON_COPYABLE(ThumbnailSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            ThumbnailSystem();

        public:
            ~ThumbnailSystem() override;

            //! Create a new thumbnail system.
            //! Throws:
            //! - ThumbnailError
            static std::shared_ptr<ThumbnailSystem> create(const std::shared_ptr<System::Context>&);

            //! Thumbnail information future.
            struct InfoFuture
            {
                InfoFuture();
                InfoFuture(std::future<IO::Info>&, Core::UID);
                std::future<IO::Info> future;
                Core::UID uid = 0;
            };
            
            //! Get information about a file.
            InfoFuture getInfo(const System::File::Info&);

            //! Cancel information about a file.
            void cancelInfo(Core::UID);

            //! Thumbnail image future.
            struct ImageFuture
            {
                ImageFuture();
                ImageFuture(std::future<std::shared_ptr<Image::Data> >&, Core::UID);
                std::future<std::shared_ptr<Image::Data> > future;
                Core::UID uid = 0;
            };

            //! Get a thumbnail image.
            ImageFuture getImage(
                const System::File::Info& path,
                const Image::Size&        size,
                Image::Type               type = Image::Type::None);

            //! Cancel a thumbnail image.
            void cancelImage(Core::UID);

            //! Get the infromation cache percentage used.
            float getInfoCachePercentage() const;

            //! Get the image cache percentage used.
            float getImageCachePercentage() const;

            //! Clear the cache.
            void clearCache();

        private:
            void _handleInfoRequests();
            void _handleImageRequests(const std::shared_ptr<GL::ImageConvert>&);

            DJV_PRIVATE();
        };

    } // namespace AV
} // namespace djv
