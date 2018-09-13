//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

#include <djvCore/Plugin.h>
#include <djvCore/Sequence.h>
#include <djvCore/System.h>

#include <QMetaType>
#include <QObject>
#include <QStringList>

#include <memory>

class djvCoreContext;
class djvFileInfo;

#if defined DJV_WINDOWS
#undef ERROR
#endif // DJV_WINDOWS

namespace djv
{
    namespace Graphics
    {
        class Image;

        //! \class ImageIOInfo
        //!
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
            djvSequence sequence;

            PixelDataInfo & operator [] (int);

            const PixelDataInfo & operator [] (int) const;

        private:
            QVector<PixelDataInfo> _info;
        };

        //! \class ImageIOFrameInfo
        //!
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

        //! \class ImageLoad
        //!
        //! This class provides the base functionality for image loading.
        //!
        //! \todo Add comments about ensuring loaders are thread safe (ie. don't use
        //! OpenGL functionality, debug logging, etc.).
        class ImageLoad
        {
        public:
            explicit ImageLoad(djvCoreContext *);

            virtual ~ImageLoad() = 0;

            //! Open an image.
            virtual void open(
                const djvFileInfo & fileInfo,
                ImageIOInfo &       imageIOInfo)
                throw (djvError) = 0;

            //! Load an image.
            virtual void read(
                Image &                  image,
                const ImageIOFrameInfo & frameInfo = ImageIOFrameInfo())
                throw (djvError) = 0;

            //! Close the image.
            virtual void close() throw (djvError);

            //! Get the context.
            djvCoreContext * context() const;

        private:
            djvCoreContext * _context = nullptr;
        };

        //! \class ImageSave
        //!
        //! This class provides the base functionality for image saving.
        class ImageSave
        {
        public:
            explicit ImageSave(djvCoreContext *);

            virtual ~ImageSave() = 0;

            //! Open an image.
            virtual void open(
                const djvFileInfo & fileInfo,
                const ImageIOInfo & imageIOInfo)
                throw (djvError) = 0;

            //! Save an image.
            virtual void write(
                const Image &            image,
                const ImageIOFrameInfo & frameInfo = ImageIOFrameInfo())
                throw (djvError) = 0;

            //! Close the image.
            virtual void close() throw (djvError);

            //! Get the context.
            djvCoreContext * context() const;

        private:
            djvCoreContext * _context = nullptr;
        };

        //! \class ImageIO
        //!
        //! This class provides the base functionality for image I/O plugins.
        class ImageIO : public QObject, public djvPlugin
        {
            Q_OBJECT

        public:
            explicit ImageIO(djvCoreContext *);

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
            virtual void commandLine(QStringList &) throw (QString);

            //! Get the command line help.
            virtual QString commandLineHelp() const;

            //! Get an image loader.
            virtual ImageLoad * createLoad() const;

            //! Get an image saver.
            virtual ImageSave * createSave() const;

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

        //! \class ImageIOFactory
        //!
        //! This class provides a factory for image I/O plugins.
        class ImageIOFactory : public djvPluginFactory
        {
            Q_OBJECT

        public:
            explicit ImageIOFactory(
                djvCoreContext *    context,
                const QStringList & searchPath = djvSystem::searchPath(),
                QObject *           parent = nullptr);

            virtual ~ImageIOFactory();

            //! Get a plugin option.
            QStringList option(const QString & name, const QString &) const;

            //! Set a plugin option.
            bool setOption(const QString & name, const QString &, QStringList &);

            //! Open an image for loading.
            ImageLoad * load(
                const djvFileInfo & fileInfo,
                ImageIOInfo &       imageIoInfo) const throw (djvError);

            //! Open an image for saving.
            ImageSave * save(
                const djvFileInfo & fileInfo,
                const ImageIOInfo & imageIoInfo) const throw (djvError);

            //! This enumeration provides error codes.
            enum ERROR
            {
                ERROR_UNRECOGNIZED,

                ERROR_COUNT
            };

            //! Get the error code labels.
            static const QStringList & errorLabels();

            virtual void addPlugin(djvPlugin *);

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

        DJV_COMPARISON_OPERATOR(ImageIOInfo);
        DJV_COMPARISON_OPERATOR(ImageIOFrameInfo);

    } // namespace Graphics
} // namespace djv

Q_DECLARE_METATYPE(djv::Graphics::ImageIOInfo)
Q_DECLARE_METATYPE(djv::Graphics::ImageIOFrameInfo)

DJV_DEBUG_OPERATOR(djv::Graphics::ImageIOInfo);
DJV_DEBUG_OPERATOR(djv::Graphics::ImageIOFrameInfo);
