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

#include <djvAV/AudioData.h>
#include <djvAV/PixelData.h>
#include <djvAV/Tags.h>

#include <djvCore/FileInfo.h>
#include <djvCore/Plugin.h>
#include <djvCore/Sequence.h>
#include <djvCore/System.h>

#include <QMetaType>
#include <QObject>
#include <QStringList>

#include <memory>
#include <vector>

#if defined DJV_WINDOWS
#undef ERROR
#endif // DJV_WINDOWS

namespace djv
{
    namespace Core
    {
        class CoreContext;

    } // namespace Core

    namespace AV
    {
        class Image;

        //! This struct provides I/O information.
        struct IOInfo
        {
            IOInfo();
            IOInfo(const PixelDataInfo &);

            std::vector<PixelDataInfo> layers;
            AudioInfo                  audio;
            Tags                       tags;
            Core::Sequence             sequence;

            bool operator == (const IOInfo &) const;
            bool operator != (const IOInfo &) const;
        };

        //! This struct provides image I/O information.
        struct ImageIOInfo
        {
            ImageIOInfo();
            ImageIOInfo(
                qint64               frame,
                size_t               layer = 0,
                PixelDataInfo::PROXY proxy = PixelDataInfo::PROXY_NONE);

            qint64 frame = -1;
            size_t layer = 0;
            PixelDataInfo::PROXY proxy = PixelDataInfo::PROXY_NONE;

            bool operator == (const ImageIOInfo &) const;
            bool operator != (const ImageIOInfo &) const;
        };

        //! This struct provides audio I/O information.
        struct AudioIOInfo
        {
            AudioIOInfo();
            AudioIOInfo(uint64_t samplesOffset, uint64_t samplesSize);

            uint64_t samplesOffset = 0;
            uint64_t samplesSize   = 0;

            bool operator == (const AudioIOInfo &) const;
            bool operator != (const AudioIOInfo &) const;
        };

        //! This class provides the base functionality for loading media.
        //!
        //! Note that loaders may be run in a separate thread.
        class Load
        {
        public:
            //! Throws:
            //! - Core::Error
            explicit Load(const Core::FileInfo &, const QPointer<Core::CoreContext> &);
            virtual ~Load() = 0;

            //! Get the file information.
            const Core::FileInfo & fileInfo() const { return _fileInfo; }

            //! Get the I/O information.
            const IOInfo & ioInfo() const { return _ioInfo; }

            //! Read an image.
            //!
            //! Throws:
            //! - Core::Error
            virtual void read(Image &, const ImageIOInfo & = ImageIOInfo());

            //! Read audio.
            //!
            //! Throws:
            //! - Core::Error
            virtual void read(AudioData &, const AudioIOInfo & = AudioIOInfo());

            //! Get the context.
            const QPointer<Core::CoreContext> & context() const;

        protected:
            Core::FileInfo _fileInfo;
            IOInfo _ioInfo;

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

        //! This class provides the base functionality for saving media.
        class Save
        {
        public:
            //! Throws:
            //! - Core::Error
            explicit Save(const Core::FileInfo &, const IOInfo &, const QPointer<Core::CoreContext> &);
            virtual ~Save() = 0;

            //! Get the file information.
            const Core::FileInfo & fileInfo() const { return _fileInfo; }

            //! Get the I/O information.
            const IOInfo & ioInfo() const { return _ioInfo; }

            //! Write an image.
            //!
            //! Throws:
            //! - Core::Error
            virtual void write(const Image &, const ImageIOInfo & = ImageIOInfo());

            //! Close the file.
            //!
            //! Throws:
            //! - Core::Error
            virtual void close();

            //! Get the context.
            const QPointer<Core::CoreContext> & context() const;

        protected:
            Core::FileInfo _fileInfo;
            IOInfo _ioInfo;

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

        //! This class provides the base functionality for I/O plugins.
        class IOPlugin : public QObject, public Core::Plugin
        {
            Q_OBJECT

        public:
            explicit IOPlugin(const QPointer<Core::CoreContext> &);
            virtual ~IOPlugin() = 0;

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

            //! Get a loader.
            virtual std::unique_ptr<Load> createLoad(const Core::FileInfo &) const;

            //! Get a saver.
            virtual std::unique_ptr<Save> createSave(const Core::FileInfo &, const IOInfo &) const;

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

        //! This class provides a factory for I/O plugins.
        class IOFactory : public Core::PluginFactory
        {
            Q_OBJECT

        public:
            explicit IOFactory(
                const QPointer<Core::CoreContext> &,
                const QStringList & searchPath = Core::System::searchPath(),
                QObject * parent = nullptr);
            ~IOFactory() override;

            //! Get a plugin option.
            QStringList option(const QString & name, const QString &) const;

            //! Set a plugin option.
            bool setOption(const QString & name, const QString &, QStringList &);

            //! Load media.
            //!
            //! Throws:
            //! - Core::Error
            std::unique_ptr<Load> load(const Core::FileInfo &, IOInfo &) const;

            //! Save media.
            //!
            //! Throws:
            //! - Core::Error
            std::unique_ptr<Save> save(const Core::FileInfo &, const IOInfo &) const;

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
            DJV_PRIVATE_COPY(IOFactory);

            void _addPlugin(IOPlugin *);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace AV

    DJV_DEBUG_OPERATOR(AV::IOInfo);
    DJV_DEBUG_OPERATOR(AV::ImageIOInfo);
    DJV_DEBUG_OPERATOR(AV::ImageIOInfo);

} // namespace djv

Q_DECLARE_METATYPE(djv::AV::IOInfo)
Q_DECLARE_METATYPE(djv::AV::ImageIOInfo)
Q_DECLARE_METATYPE(djv::AV::AudioIOInfo)
