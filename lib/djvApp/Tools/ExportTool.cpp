// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Tools/ExportTool.h>

#include <djvApp/Models/ColorModel.h>
#include <djvApp/Models/FilesModel.h>
#include <djvApp/Models/SettingsModel.h>
#include <djvApp/Models/ViewportModel.h>
#include <djvApp/App.h>

#include <tlTimelineGL/Render.h>

#include <tlTimeline/IRender.h>
#include <tlTimeline/Util.h>

#include <tlIO/System.h>
#if defined(TLRENDER_FFMPEG)
#include <tlIO/FFmpeg.h>
#endif // TLRENDER_FFMPEG

#include <feather-tk/ui/ComboBox.h>
#include <feather-tk/ui/DialogSystem.h>
#include <feather-tk/ui/FileEdit.h>
#include <feather-tk/ui/FormLayout.h>
#include <feather-tk/ui/IntEdit.h>
#include <feather-tk/ui/LineEdit.h>
#include <feather-tk/ui/ProgressDialog.h>
#include <feather-tk/ui/PushButton.h>
#include <feather-tk/ui/RowLayout.h>
#include <feather-tk/ui/ScrollWidget.h>
#include <feather-tk/gl/GL.h>
#include <feather-tk/gl/OffscreenBuffer.h>
#include <feather-tk/gl/Util.h>
#include <feather-tk/core/Format.h>
#include <feather-tk/core/Timer.h>

namespace djv
{
    namespace app
    {
        struct ExportTool::Private
        {
            std::weak_ptr<App> app;
            std::shared_ptr<tl::timeline::Player> player;
            std::shared_ptr<SettingsModel> model;
            std::vector<std::string> imageExtensions;
            std::vector<std::string> movieExtensions;
            std::vector<std::string> movieCodecs;

            struct ExportData
            {
                OTIO_NS::TimeRange range;
                int64_t frame = 0;
                tl::file::Path path;
                feather_tk::ImageInfo info;
                std::shared_ptr<tl::io::IWrite> writer;
                tl::timeline::OCIOOptions ocioOptions;
                tl::timeline::LUTOptions lutOptions;
                feather_tk::ImageOptions imageOptions;
                tl::timeline::DisplayOptions displayOptions;
                feather_tk::ImageType colorBuffer = feather_tk::ImageType::RGBA_U8;
                std::shared_ptr<feather_tk::gl::OffscreenBuffer> buffer;
                std::shared_ptr<tl::timeline::IRender> render;
                GLenum glFormat = 0;
                GLenum glType = 0;
            };
            std::unique_ptr<ExportData> exportData;

            std::shared_ptr<feather_tk::FileEdit> directoryEdit;
            std::shared_ptr<feather_tk::ComboBox> renderSizeComboBox;
            std::shared_ptr<feather_tk::IntEdit> renderWidthEdit;
            std::shared_ptr<feather_tk::IntEdit> renderHeightEdit;
            std::shared_ptr<feather_tk::ComboBox> fileTypeComboBox;
            std::shared_ptr<feather_tk::LineEdit> imageBaseNameEdit;
            std::shared_ptr<feather_tk::IntEdit> imageZeroPadEdit;
            std::shared_ptr<feather_tk::ComboBox> imageExtensionComboBox;
            std::shared_ptr<feather_tk::LineEdit> movieBaseNameEdit;
            std::shared_ptr<feather_tk::ComboBox> movieExtensionComboBox;
            std::shared_ptr<feather_tk::ComboBox> movieCodecComboBox;
            std::shared_ptr<feather_tk::PushButton> exportButton;
            std::shared_ptr<feather_tk::HorizontalLayout> customSizeLayout;
            std::shared_ptr<feather_tk::FormLayout> formLayout;
            std::shared_ptr<feather_tk::VerticalLayout> layout;
            std::shared_ptr<feather_tk::ProgressDialog> progressDialog;

            std::shared_ptr<feather_tk::ValueObserver<std::shared_ptr<tl::timeline::Player> > > playerObserver;
            std::shared_ptr<feather_tk::ValueObserver<ExportSettings> > settingsObserver;

            std::shared_ptr<feather_tk::Timer> progressTimer;
        };

        void ExportTool::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IToolWidget::_init(
                context,
                app,
                Tool::Export,
                "djv::app::ExportTool",
                parent);
            FEATHER_TK_P();

            p.app = app;
            p.model = app->getSettingsModel();
            auto ioSystem = context->getSystem<tl::io::WriteSystem>();
            auto extensions = ioSystem->getExtensions(static_cast<int>(tl::io::FileType::Sequence));
            p.imageExtensions.insert(p.imageExtensions.end(), extensions.begin(), extensions.end());
            extensions = ioSystem->getExtensions(static_cast<int>(tl::io::FileType::Media));
            p.movieExtensions.insert(p.movieExtensions.end(), extensions.begin(), extensions.end());
#if defined(TLRENDER_FFMPEG)
            auto ffmpegPlugin = ioSystem->getPlugin<tl::ffmpeg::WritePlugin>();
            p.movieCodecs = ffmpegPlugin->getCodecs();
#endif // TLRENDER_FFMPEG

            p.directoryEdit = feather_tk::FileEdit::create(context, feather_tk::FileBrowserMode::Dir);

            p.renderSizeComboBox = feather_tk::ComboBox::create(context, getExportRenderSizeLabels());
            p.renderWidthEdit = feather_tk::IntEdit::create(context);
            p.renderWidthEdit->setRange(1, 16384);
            p.renderHeightEdit = feather_tk::IntEdit::create(context);
            p.renderHeightEdit->setRange(1, 16384);

            p.fileTypeComboBox = feather_tk::ComboBox::create(context, getExportFileTypeLabels());

            p.imageBaseNameEdit = feather_tk::LineEdit::create(context);
            p.imageZeroPadEdit = feather_tk::IntEdit::create(context);
            p.imageZeroPadEdit->setRange(0, 16);
            p.imageExtensionComboBox = feather_tk::ComboBox::create(context, p.imageExtensions);

            p.movieBaseNameEdit = feather_tk::LineEdit::create(context);
            p.movieExtensionComboBox = feather_tk::ComboBox::create(context, p.movieExtensions);
            p.movieCodecComboBox = feather_tk::ComboBox::create(context, p.movieCodecs);

            p.exportButton = feather_tk::PushButton::create(context, "Export");

            p.layout = feather_tk::VerticalLayout::create(context);
            p.layout->setMarginRole(feather_tk::SizeRole::MarginSmall);
            p.layout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            p.formLayout = feather_tk::FormLayout::create(context, p.layout);
            p.formLayout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            p.formLayout->addRow("Directory:", p.directoryEdit);
            p.formLayout->addRow("Render size:", p.renderSizeComboBox);
            p.customSizeLayout = feather_tk::HorizontalLayout::create(context);
            p.customSizeLayout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            p.renderWidthEdit->setParent(p.customSizeLayout);
            p.renderHeightEdit->setParent(p.customSizeLayout);
            p.formLayout->addRow("Custom size:", p.customSizeLayout);
            p.formLayout->addRow("File type:", p.fileTypeComboBox);
            p.formLayout->addRow("Base name:", p.imageBaseNameEdit);
            p.formLayout->addRow("Zero padding:", p.imageZeroPadEdit);
            p.formLayout->addRow("Extension:", p.imageExtensionComboBox);
            p.formLayout->addRow("Base name:", p.movieBaseNameEdit);
            p.formLayout->addRow("Extension:", p.movieExtensionComboBox);
            p.formLayout->addRow("Codec:", p.movieCodecComboBox);
            p.exportButton->setParent(p.layout);

            auto scrollWidget = feather_tk::ScrollWidget::create(context);
            scrollWidget->setBorder(false);
            scrollWidget->setWidget(p.layout);
            _setWidget(scrollWidget);

            p.playerObserver = feather_tk::ValueObserver<std::shared_ptr<tl::timeline::Player> >::create(
                app->observePlayer(),
                [this](const std::shared_ptr<tl::timeline::Player>& value)
                {
                    FEATHER_TK_P();
                    p.player = value;
                    p.exportButton->setEnabled(value.get());
                });

            p.settingsObserver = feather_tk::ValueObserver<ExportSettings>::create(
                p.model->observeExport(),
                [this](const ExportSettings& value)
                {
                    _widgetUpdate(value);
                });

            p.directoryEdit->setCallback(
                [this](const std::filesystem::path& value)
                {
                    FEATHER_TK_P();
                    auto options = p.model->getExport();
                    options.directory = value.u8string();
                    p.model->setExport(options);
                });

            p.renderSizeComboBox->setIndexCallback(
                [this](int value)
                {
                    FEATHER_TK_P();
                    auto options = p.model->getExport();
                    options.renderSize = static_cast<ExportRenderSize>(value);
                    p.model->setExport(options);
                });

            p.renderWidthEdit->setCallback(
                [this](int value)
                {
                    FEATHER_TK_P();
                    auto options = p.model->getExport();
                    options.customSize.w = value;
                    p.model->setExport(options);
                });

            p.renderHeightEdit->setCallback(
                [this](int value)
                {
                    FEATHER_TK_P();
                    auto options = p.model->getExport();
                    options.customSize.h = value;
                    p.model->setExport(options);
                });

            p.fileTypeComboBox->setIndexCallback(
                [this](int value)
                {
                    FEATHER_TK_P();
                    auto options = p.model->getExport();
                    options.fileType = static_cast<ExportFileType>(value);
                    p.model->setExport(options);
                });

            p.imageBaseNameEdit->setTextCallback(
                [this](const std::string& value)
                {
                    FEATHER_TK_P();
                    auto options = p.model->getExport();
                    options.imageBaseName = value;
                    p.model->setExport(options);
                });

            p.imageZeroPadEdit->setCallback(
                [this](int value)
                {
                    FEATHER_TK_P();
                    auto options = p.model->getExport();
                    options.imageZeroPad = value;
                    p.model->setExport(options);
                });

            p.imageExtensionComboBox->setIndexCallback(
                [this](int value)
                {
                    FEATHER_TK_P();
                    if (value >= 0 && value < p.imageExtensions.size())
                    {
                        auto options = p.model->getExport();
                        options.imageExtension = p.imageExtensions[value];
                        p.model->setExport(options);
                    }
                });

            p.movieBaseNameEdit->setTextCallback(
                [this](const std::string& value)
                {
                    FEATHER_TK_P();
                    auto options = p.model->getExport();
                    options.movieBaseName = value;
                    p.model->setExport(options);
                });

            p.movieExtensionComboBox->setIndexCallback(
                [this](int value)
                {
                    FEATHER_TK_P();
                    if (value >= 0 && value < p.movieExtensions.size())
                    {
                        auto options = p.model->getExport();
                        options.movieExtension = p.movieExtensions[value];
                        p.model->setExport(options);
                    }
                });

            p.movieCodecComboBox->setIndexCallback(
                [this](int value)
                {
                    FEATHER_TK_P();
                    if (value >= 0 && value < p.movieCodecs.size())
                    {
                        auto options = p.model->getExport();
                        options.movieCodec = p.movieCodecs[value];
                        p.model->setExport(options);
                    }
                });

            p.exportButton->setClickedCallback(
                [this]
                {
                    _export();
                });

            p.progressTimer = feather_tk::Timer::create(context);
            p.progressTimer->setRepeating(true);
        }

        ExportTool::ExportTool() :
            _p(new Private)
        {}

        ExportTool::~ExportTool()
        {}

        std::shared_ptr<ExportTool> ExportTool::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<ExportTool>(new ExportTool);
            out->_init(context, app, parent);
            return out;
        }

        void ExportTool::_widgetUpdate(const ExportSettings& settings)
        {
            FEATHER_TK_P();
            p.directoryEdit->setPath(std::filesystem::u8path(settings.directory));
            p.renderSizeComboBox->setCurrentIndex(static_cast<int>(settings.renderSize));
            p.renderWidthEdit->setValue(settings.customSize.w);
            p.renderHeightEdit->setValue(settings.customSize.h);
            p.fileTypeComboBox->setCurrentIndex(static_cast<int>(settings.fileType));
            
            p.imageBaseNameEdit->setText(settings.imageBaseName);
            auto i = std::find(p.imageExtensions.begin(), p.imageExtensions.end(), settings.imageExtension);
            p.imageExtensionComboBox->setCurrentIndex(i != p.imageExtensions.end() ? (i - p.imageExtensions.begin()) : -1);

            p.movieBaseNameEdit->setText(settings.movieBaseName);
            i = std::find(p.movieExtensions.begin(), p.movieExtensions.end(), settings.movieExtension);
            p.movieExtensionComboBox->setCurrentIndex(i != p.movieExtensions.end() ? (i - p.movieExtensions.begin()) : -1);
            i = std::find(p.movieCodecs.begin(), p.movieCodecs.end(), settings.movieCodec);
            p.movieCodecComboBox->setCurrentIndex(i != p.movieCodecs.end() ? (i - p.movieCodecs.begin()) : -1);

            p.formLayout->setRowVisible(p.customSizeLayout, ExportRenderSize::Custom == settings.renderSize);
            p.formLayout->setRowVisible(
                p.imageBaseNameEdit,
                ExportFileType::Image == settings.fileType ||
                ExportFileType::Sequence == settings.fileType);
            p.formLayout->setRowVisible(
                p.imageZeroPadEdit,
                ExportFileType::Image == settings.fileType ||
                ExportFileType::Sequence == settings.fileType);
            p.formLayout->setRowVisible(
                p.imageExtensionComboBox,
                ExportFileType::Image == settings.fileType ||
                ExportFileType::Sequence == settings.fileType);
            p.formLayout->setRowVisible(p.movieBaseNameEdit, ExportFileType::Movie == settings.fileType);
            p.formLayout->setRowVisible(p.movieExtensionComboBox, ExportFileType::Movie == settings.fileType);
            p.formLayout->setRowVisible(p.movieCodecComboBox, ExportFileType::Movie == settings.fileType);
        }

        void ExportTool::_export()
        {
            FEATHER_TK_P();
            auto context = getContext();
            auto app = p.app.lock();
            if (app && context && p.player)
            {
                try
                {
                    const tl::io::Info ioInfo = p.player->getIOInfo();
                    if (ioInfo.video.empty())
                    {
                        throw std::runtime_error("No video to render");
                    }
                    p.exportData.reset(new Private::ExportData);

                    // Get the time range.
                    const auto options = p.model->getExport();
                    switch (options.fileType)
                    {
                    case ExportFileType::Image:
                        p.exportData->range = OTIO_NS::TimeRange(
                            p.player->getCurrentTime(),
                            OTIO_NS::RationalTime(1.0, p.player->getTimeRange().duration().rate()));
                        break;
                    default:
                        p.exportData->range = p.player->getInOutRange();
                        break;
                    }
                    p.exportData->frame = p.exportData->range.start_time().value();

                    // Get the render size.
                    switch (options.renderSize)
                    {
                    case ExportRenderSize::Default:
                        p.exportData->info.size = ioInfo.video.front().size;
                        break;
                    case ExportRenderSize::Custom:
                        p.exportData->info.size = options.customSize;
                        break;
                    default:
                        p.exportData->info.size = getSize(options.renderSize);
                        break;
                    }

                    // Get the export path.
                    std::string fileName;
                    switch (options.fileType)
                    {
                    case ExportFileType::Image:
                    case ExportFileType::Sequence:
                    {
                        std::stringstream ss;
                        ss << options.imageBaseName;
                        ss << std::setfill('0') << std::setw(options.imageZeroPad) << p.exportData->range.start_time().value();
                        ss << options.imageExtension;
                        fileName = ss.str();
                        break;
                    }
                    case ExportFileType::Movie:
                    {
                        std::stringstream ss;
                        ss << options.movieBaseName << options.movieExtension;
                        fileName = ss.str();
                        break;
                    }
                    default: break;
                    }
                    p.exportData->path = tl::file::Path((std::filesystem::u8path(options.directory) /
                        std::filesystem::u8path(fileName)).u8string());

                    // Get the writer.
                    auto ioSystem = context->getSystem<tl::io::WriteSystem>();
                    auto plugin = ioSystem->getPlugin(p.exportData->path);
                    if (!plugin)
                    {
                        throw std::runtime_error(
                            feather_tk::Format("Cannot open: \"{0}\"").arg(p.exportData->path.get()));
                    }
                    p.exportData->info.type = ioInfo.video.front().type;
                    p.exportData->info = plugin->getInfo(p.exportData->info);
                    if (feather_tk::ImageType::None == p.exportData->info.type)
                    {
                        p.exportData->info.type = feather_tk::ImageType::RGBA_U8;
                    }
                    p.exportData->glFormat = feather_tk::gl::getReadPixelsFormat(p.exportData->info.type);
                    p.exportData->glType = feather_tk::gl::getReadPixelsType(p.exportData->info.type);
                    if (GL_NONE == p.exportData->glFormat || GL_NONE == p.exportData->glType)
                    {
                        throw std::runtime_error(
                            feather_tk::Format("Cannot open: \"{0}\"").arg(p.exportData->path.get()));
                    }
                    const double speed = p.player->getSpeed();
                    tl::io::Info outputInfo;
                    outputInfo.video.push_back(p.exportData->info);
                    outputInfo.videoTime = OTIO_NS::TimeRange(
                        OTIO_NS::RationalTime(0.0, speed),
                        p.exportData->range.duration().rescaled_to(speed));
                    tl::io::Options ioOptions;
                    ioOptions["FFmpeg/Codec"] = options.movieCodec;
                    p.exportData->writer = plugin->write(p.exportData->path, outputInfo, ioOptions);

                    // Create the renderer.
                    p.exportData->ocioOptions = app->getColorModel()->getOCIOOptions();
                    p.exportData->lutOptions = app->getColorModel()->getLUTOptions();
                    p.exportData->imageOptions = app->getViewportModel()->getImageOptions();
                    p.exportData->displayOptions = app->getViewportModel()->getDisplayOptions();
                    p.exportData->colorBuffer = app->getViewportModel()->getColorBuffer();
                    p.exportData->render = tl::timeline_gl::Render::create(context);
                    feather_tk::gl::OffscreenBufferOptions offscreenBufferOptions;
                    offscreenBufferOptions.color = p.exportData->colorBuffer;
                    p.exportData->buffer = feather_tk::gl::OffscreenBuffer::create(
                        p.exportData->info.size,
                        offscreenBufferOptions);

                    // Create the progress dialog.
                    p.progressDialog = feather_tk::ProgressDialog::create(
                        context,
                        "Export",
                        "Rendering:");
                    p.progressDialog->setRange(0.0, p.exportData->range.duration().value() - 1.0);
                    p.progressDialog->setMessage(feather_tk::Format("Frame: {0} / {1}").
                        arg(p.exportData->frame).
                        arg(p.exportData->range.end_time_inclusive().value()));
                    p.progressDialog->setCloseCallback(
                        [this]
                        {
                            FEATHER_TK_P();
                            p.progressTimer->stop();
                            p.exportData.reset();
                            p.progressDialog.reset();
                        });
                    p.progressDialog->open(getWindow());
                    p.progressTimer->start(
                        std::chrono::microseconds(500),
                        [this]
                        {
                            FEATHER_TK_P();
                            if (_exportFrame())
                            {
                                p.progressDialog->setValue(p.exportData->frame - p.exportData->range.start_time().value());
                                const int64_t end = p.exportData->range.end_time_inclusive().value();
                                if (p.exportData->frame <= end)
                                {
                                    p.progressDialog->setMessage(feather_tk::Format("Frame: {0} / {1}").
                                        arg(p.exportData->frame).
                                        arg(end));
                                }
                                else
                                {
                                    p.progressDialog->close();
                                }
                            }
                        });
                }
                catch (const std::exception& e)
                {
                    if (p.progressDialog)
                    {
                        p.progressDialog->close();
                    }
                    context->getSystem<feather_tk::DialogSystem>()->message(
                        "ERROR",
                        feather_tk::Format("Error: {0}").arg(e.what()),
                        getWindow());
                }
            }
        }

        bool ExportTool::_exportFrame()
        {
            FEATHER_TK_P();
            bool out = false;
            try
            {
                // Get the video.
                const OTIO_NS::RationalTime t(p.exportData->frame, p.exportData->range.duration().rate());
                auto ioOptions = p.player->getTimeline()->getOptions().ioOptions;
                ioOptions["Layer"] = feather_tk::Format("{0}").arg(p.player->getVideoLayer());
                auto video = p.player->getTimeline()->getVideo(t, ioOptions).future.get();

                // Render the video.
                feather_tk::gl::OffscreenBufferBinding binding(p.exportData->buffer);
                p.exportData->render->begin(p.exportData->info.size);
                p.exportData->render->setOCIOOptions(p.exportData->ocioOptions);
                p.exportData->render->setLUTOptions(p.exportData->lutOptions);
                p.exportData->render->drawVideo(
                    { video },
                    { feather_tk::Box2I(0, 0, p.exportData->info.size.w, p.exportData->info.size.h) },
                    { p.exportData->imageOptions },
                    { p.exportData->displayOptions },
                    tl::timeline::CompareOptions(),
                    p.exportData->colorBuffer);
                p.exportData->render->end();

                // Write the output image.
                auto image = feather_tk::Image::create(p.exportData->info);
                glPixelStorei(GL_PACK_ALIGNMENT, p.exportData->info.layout.alignment);
#if defined(dtk_API_GL_4_1)
                glPixelStorei(GL_PACK_SWAP_BYTES, p.exportData->info.layout.endian != feather_tk::getEndian());
#endif // dtk_API_GL_4_1
                glReadPixels(
                    0,
                    0,
                    p.exportData->info.size.w,
                    p.exportData->info.size.h,
                    p.exportData->glFormat,
                    p.exportData->glType,
                    image->getData());

                const double speed = p.player->getSpeed();
                const OTIO_NS::RationalTime t2 (p.exportData->frame, speed);
                p.exportData->writer->writeVideo(t2, image);

                ++p.exportData->frame;

                out = true;
            }
            catch (const std::exception& e)
            {
                if (p.progressDialog)
                {
                    p.progressDialog->close();
                }
                if (auto context = getContext())
                {
                    context->getSystem<feather_tk::DialogSystem>()->message(
                        "ERROR",
                        feather_tk::Format("Error: {0}").arg(e.what()),
                        getWindow());
                }
            }
            return out;
        }
    }
}
