//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#pragma once

#include <djvGraphics/ImageTags.h>
#include <djvGraphics/PixelData.h>

#include <djvCore/FileInfo.h>
#include <djvCore/Plugin.h>
#include <djvCore/Sequence.h>
#include <djvCore/System.h>

#include <QMetaType>
#include <QObject>
#include <QStringList>

#include <memory>

#if defined DJV_WINDOWS
#undef ERROR
#endif // DJV_WINDOWS

namespace djv
{
    namespace Core
    {
        class CoreContext;

    } // namespace Core

    namespace Graphics
    {
        class Image;

        //! This class provides image I/O information.
        class ImageIOInfo : public PixelDataInfo
        {
        public:
            ImageIOInfo();
            ImageIOInfo(const PixelDataInfo &);

            //! Add a layer.
            void addLayer(const PixelDataInfo &);

            //! Get the number of layers.
            int layerCount() const;

            //! Set the number of layers.
            void setLayerCount(int);

            //! Remove all the layers.
            void clearLayers();

            //! The image tags.
            ImageTags tags;

            //! The frame sequence.
            Core::Sequence sequence;

            PixelDataInfo & operator [] (int);

            const PixelDataInfo & operator [] (int) const;

        private:
            QVector<PixelDataInfo> _info;
        };

        //! This struct provides image I/O frame information.
        struct ImageIOFrameInfo
        {
            ImageIOFrameInfo();
            ImageIOFrameInfo(
                qint64               frame,
                int                  layer = 0,
                PixelDataInfo::PROXY proxy = PixelDataInfo::PROXY_NONE);

            //! The frame number.
            qint64 frame = -1;

            //! The image layer.
            int layer = 0;

            //! The proxy scale.
            PixelDataInfo::PROXY proxy = PixelDataInfo::PROXY_NONE;
        };

        //! This class provides the base functionality for image loading.
        //!
        //! Note that image loaders may be run in a separate thread.
        class ImageLoad
        {
        public:
            //! Throws:
            //! - Core::Error
            explicit ImageLoad(const Core::FileInfo &, const QPointer<Core::CoreContext> &);
            virtual ~ImageLoad() = 0;

            //! Get the file information.
            const Core::FileInfo & fileInfo() const { _fileInfo; }

            //! Get the image I/O information.
            const ImageIOInfo & imageIOInfo() const { return _imageIOInfo; }

            //! Load an image.
            //!
            //! Throws:
            //! - Core::Error
            virtual void read(Image &, const ImageIOFrameInfo & = ImageIOFrameInfo()) = 0;

            //! Get the context.
            const QPointer<Core::CoreContext> & context() const;

        protected:
            Core::FileInfo _fileInfo;
            ImageIOInfo _imageIOInfo;

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

        //! This class provides the base functionality for image saving.
        class ImageSave
        {
        public:
            //! Throws:
            //! - Core::Error
            explicit ImageSave(const Core::FileInfo &, const ImageIOInfo &, const QPointer<Core::CoreContext> &);
            virtual ~ImageSave() = 0;

            //! Get the file information.
            const Core::FileInfo & fileInfo() const { _fileInfo; }

            //! Get the image I/O information.
            const ImageIOInfo & imageIOInfo() const { return _imageIOInfo; }

            //! Save an image.
            //!
            //! Throws:
            //! - Core::Error
            virtual void write(const Image &, const ImageIOFrameInfo & = ImageIOFrameInfo()) = 0;

            //! Close the file.
            //!
            //! Throws:
            //! - Core::Error
            virtual void close();

            //! Get the context.
            const QPointer<Core::CoreContext> & context() const;

        protected:
            Core::FileInfo _fileInfo;
            ImageIOInfo _imageIOInfo;

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

        //! This class provides the base functionality for image I/O plugins.
        class ImageIO : public QObject, public Core::Plugin
        {
            Q_OBJECT

        public:
            explicit ImageIO(const QPointer<Core::CoreContext> &);
            virtual ~ImageIO() = 0;

            //! Get the list of supported file extensions.
            virtual QStringList extensions() const;

            //! Does the plugin use file sequences?
            virtual bool isSequence() const;

            //! Get an option.
            virtual QStringList option(const QString &) const;

            //! Set an option.
            virtual bool setOption(const QString &, QStringList &);

            //! Get the list of options.
            virtual QStringList options() const;

            //! Parse the command line.
            //!
            //! Throws:
            //! - QString
            virtual void commandLine(QStringList &);

            //! Get the command line help.
            virtual QString commandLineHelp() const;

            //! Get an image loader.
            virtual std::unique_ptr<ImageLoad> createLoad(const Core::FileInfo &) const;

            //! Get an image saver.
            virtual std::unique_ptr<ImageSave> createSave(const Core::FileInfo &, const ImageIOInfo &) const;

            //! This enumeration provides error codes.
            enum ERROR
            {
                ERROR_UNRECOGNIZED,
                ERROR_UNSUPPORTED,
                ERROR_OPEN,
                ERROR_READ,
                ERROR_WRITE,

                ERROR_COUNT
            };

            //! Get the error code labels.
            static const QStringList & errorLabels();

        Q_SIGNALS:
            //! This signal is emitted when an option is changed.
            void optionChanged(const QString &);
        };

        //! This class provides a factory for image I/O plugins.
        class ImageIOFactory : public Core::PluginFactory
        {
            Q_OBJECT

        public:
            explicit ImageIOFactory(
                const QPointer<Core::CoreContext> &,
                const QStringList & searchPath = Core::System::searchPath(),
                QObject * parent = nullptr);
            ~ImageIOFactory() override;

            //! Get a plugin option.
            QStringList option(const QString & name, const QString &) const;

            //! Set a plugin option.
            bool setOption(const QString & name, const QString &, QStringList &);

            //! Open an image for loading.
            //!
            //! Throws:
            //! - Core::Error
            std::unique_ptr<ImageLoad> load(const Core::FileInfo &, ImageIOInfo &) const;

            //! Open an image for saving.
            //!
            //! Throws:
            //! - Core::Error
            std::unique_ptr<ImageSave> save(const Core::FileInfo &, const ImageIOInfo &) const;

            //! This enumeration provides error codes.
            enum ERROR
            {
                ERROR_UNRECOGNIZED,

                ERROR_COUNT
            };

            //! Get the error code labels.
            static const QStringList & errorLabels();

            void addPlugin(Core::Plugin *) override;

        Q_SIGNALS:
            //! This signal is emitted when a plugin option is changed.
            void optionChanged();

        private Q_SLOTS:
            void pluginOptionCallback(const QString &);

        private:
            DJV_PRIVATE_COPY(ImageIOFactory);

            void _addPlugin(ImageIO *);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace Graphics

    DJV_COMPARISON_OPERATOR(Graphics::ImageIOInfo);
    DJV_COMPARISON_OPERATOR(Graphics::ImageIOFrameInfo);

    DJV_DEBUG_OPERATOR(Graphics::ImageIOInfo);
    DJV_DEBUG_OPERATOR(Graphics::ImageIOFrameInfo);

} // namespace djv

Q_DECLARE_METATYPE(djv::Graphics::ImageIOInfo)
Q_DECLARE_METATYPE(djv::Graphics::ImageIOFrameInfo)
