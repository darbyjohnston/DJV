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

#include <dtk/ui/ComboBox.h>
#include <dtk/ui/DialogSystem.h>
#include <dtk/ui/FileEdit.h>
#include <dtk/ui/FormLayout.h>
#include <dtk/ui/IntEdit.h>
#include <dtk/ui/LineEdit.h>
#include <dtk/ui/ProgressDialog.h>
#include <dtk/ui/PushButton.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/ScrollWidget.h>
#include <dtk/gl/GL.h>
#include <dtk/gl/OffscreenBuffer.h>
#include <dtk/gl/Util.h>
#include <dtk/core/Format.h>
#include <dtk/core/Timer.h>

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
                dtk::ImageInfo info;
                std::shared_ptr<tl::io::IWrite> writer;
                tl::timeline::OCIOOptions ocioOptions;
                tl::timeline::LUTOptions lutOptions;
                dtk::ImageOptions imageOptions;
                tl::timeline::DisplayOptions displayOptions;
                dtk::ImageType colorBuffer = dtk::ImageType::RGBA_U8;
                std::shared_ptr<dtk::gl::OffscreenBuffer> buffer;
                std::shared_ptr<tl::timeline::IRender> render;
                GLenum glFormat = 0;
                GLenum glType = 0;
            };
            std::unique_ptr<ExportData> exportData;

            std::shared_ptr<dtk::FileEdit> directoryEdit;
            std::shared_ptr<dtk::ComboBox> renderSizeComboBox;
            std::shared_ptr<dtk::IntEdit> renderWidthEdit;
            std::shared_ptr<dtk::IntEdit> renderHeightEdit;
            std::shared_ptr<dtk::ComboBox> fileTypeComboBox;
            std::shared_ptr<dtk::LineEdit> imageBaseNameEdit;
            std::shared_ptr<dtk::IntEdit> imageZeroPadEdit;
            std::shared_ptr<dtk::ComboBox> imageExtensionComboBox;
            std::shared_ptr<dtk::LineEdit> movieBaseNameEdit;
            std::shared_ptr<dtk::ComboBox> movieExtensionComboBox;
            std::shared_ptr<dtk::ComboBox> movieCodecComboBox;
            std::shared_ptr<dtk::PushButton> exportButton;
            std::shared_ptr<dtk::HorizontalLayout> customSizeLayout;
            std::shared_ptr<dtk::FormLayout> formLayout;
            std::shared_ptr<dtk::VerticalLayout> layout;
            std::shared_ptr<dtk::ProgressDialog> progressDialog;

            std::shared_ptr<dtk::ValueObserver<std::shared_ptr<tl::timeline::Player> > > playerObserver;
            std::shared_ptr<dtk::ValueObserver<ExportSettings> > settingsObserver;

            std::shared_ptr<dtk::Timer> progressTimer;
        };

        void ExportTool::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IToolWidget::_init(
                context,
                app,
                Tool::Export,
                "djv::app::ExportTool",
                parent);
            DTK_P();

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

            p.directoryEdit = dtk::FileEdit::create(context, dtk::FileBrowserMode::Dir);

            p.renderSizeComboBox = dtk::ComboBox::create(context, getExportRenderSizeLabels());
            p.renderWidthEdit = dtk::IntEdit::create(context);
            p.renderWidthEdit->setRange(dtk::RangeI(1, 16384));
            p.renderHeightEdit = dtk::IntEdit::create(context);
            p.renderHeightEdit->setRange(dtk::RangeI(1, 16384));

            p.fileTypeComboBox = dtk::ComboBox::create(context, getExportFileTypeLabels());

            p.imageBaseNameEdit = dtk::LineEdit::create(context);
            p.imageZeroPadEdit = dtk::IntEdit::create(context);
            p.imageZeroPadEdit->setRange(dtk::RangeI(0, 16));
            p.imageExtensionComboBox = dtk::ComboBox::create(context, p.imageExtensions);

            p.movieBaseNameEdit = dtk::LineEdit::create(context);
            p.movieExtensionComboBox = dtk::ComboBox::create(context, p.movieExtensions);
            p.movieCodecComboBox = dtk::ComboBox::create(context, p.movieCodecs);

            p.exportButton = dtk::PushButton::create(context, "Export");

            p.layout = dtk::VerticalLayout::create(context);
            p.layout->setMarginRole(dtk::SizeRole::MarginSmall);
            p.layout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            p.formLayout = dtk::FormLayout::create(context, p.layout);
            p.formLayout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            p.formLayout->addRow("Directory:", p.directoryEdit);
            p.formLayout->addRow("Render size:", p.renderSizeComboBox);
            p.customSizeLayout = dtk::HorizontalLayout::create(context);
            p.customSizeLayout->setSpacingRole(dtk::SizeRole::SpacingSmall);
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

            auto scrollWidget = dtk::ScrollWidget::create(context);
            scrollWidget->setBorder(false);
            scrollWidget->setWidget(p.layout);
            _setWidget(scrollWidget);

            p.playerObserver = dtk::ValueObserver<std::shared_ptr<tl::timeline::Player> >::create(
                app->observePlayer(),
                [this](const std::shared_ptr<tl::timeline::Player>& value)
                {
                    DTK_P();
                    p.player = value;
                    p.exportButton->setEnabled(value.get());
                });

            p.settingsObserver = dtk::ValueObserver<ExportSettings>::create(
                p.model->observeExport(),
                [this](const ExportSettings& value)
                {
                    _widgetUpdate(value);
                });

            p.directoryEdit->setCallback(
                [this](const std::filesystem::path& value)
                {
                    DTK_P();
                    auto options = p.model->getExport();
                    options.directory = value.u8string();
                    p.model->setExport(options);
                });

            p.renderSizeComboBox->setIndexCallback(
                [this](int value)
                {
                    DTK_P();
                    auto options = p.model->getExport();
                    options.renderSize = static_cast<ExportRenderSize>(value);
                    p.model->setExport(options);
                });

            p.renderWidthEdit->setCallback(
                [this](int value)
                {
                    DTK_P();
                    auto options = p.model->getExport();
                    options.customSize.w = value;
                    p.model->setExport(options);
                });

            p.renderHeightEdit->setCallback(
                [this](int value)
                {
                    DTK_P();
                    auto options = p.model->getExport();
                    options.customSize.h = value;
                    p.model->setExport(options);
                });

            p.fileTypeComboBox->setIndexCallback(
                [this](int value)
                {
                    DTK_P();
                    auto options = p.model->getExport();
                    options.fileType = static_cast<ExportFileType>(value);
                    p.model->setExport(options);
                });

            p.imageBaseNameEdit->setTextCallback(
                [this](const std::string& value)
                {
                    DTK_P();
                    auto options = p.model->getExport();
                    options.imageBaseName = value;
                    p.model->setExport(options);
                });

            p.imageZeroPadEdit->setCallback(
                [this](int value)
                {
                    DTK_P();
                    auto options = p.model->getExport();
                    options.imageZeroPad = value;
                    p.model->setExport(options);
                });

            p.imageExtensionComboBox->setIndexCallback(
                [this](int value)
                {
                    DTK_P();
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
                    DTK_P();
                    auto options = p.model->getExport();
                    options.movieBaseName = value;
                    p.model->setExport(options);
                });

            p.movieExtensionComboBox->setIndexCallback(
                [this](int value)
                {
                    DTK_P();
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
                    DTK_P();
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

            p.progressTimer = dtk::Timer::create(context);
            p.progressTimer->setRepeating(true);
        }

        ExportTool::ExportTool() :
            _p(new Private)
        {}

        ExportTool::~ExportTool()
        {}

        std::shared_ptr<ExportTool> ExportTool::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<ExportTool>(new ExportTool);
            out->_init(context, app, parent);
            return out;
        }

        void ExportTool::_widgetUpdate(const ExportSettings& settings)
        {
            DTK_P();
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
            DTK_P();
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
                            dtk::Format("Cannot open: \"{0}\"").arg(p.exportData->path.get()));
                    }
                    p.exportData->info.type = ioInfo.video.front().type;
                    p.exportData->info = plugin->getInfo(p.exportData->info);
                    if (dtk::ImageType::None == p.exportData->info.type)
                    {
                        p.exportData->info.type = dtk::ImageType::RGBA_U8;
                    }
                    p.exportData->glFormat = dtk::gl::getReadPixelsFormat(p.exportData->info.type);
                    p.exportData->glType = dtk::gl::getReadPixelsType(p.exportData->info.type);
                    if (GL_NONE == p.exportData->glFormat || GL_NONE == p.exportData->glType)
                    {
                        throw std::runtime_error(
                            dtk::Format("Cannot open: \"{0}\"").arg(p.exportData->path.get()));
                    }
                    tl::io::Info outputInfo;
                    outputInfo.video.push_back(p.exportData->info);
                    outputInfo.videoTime = OTIO_NS::TimeRange(
                        OTIO_NS::RationalTime(0.0, p.exportData->range.duration().rate()),
                        p.exportData->range.duration());
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
                    dtk::gl::OffscreenBufferOptions offscreenBufferOptions;
                    offscreenBufferOptions.color = p.exportData->colorBuffer;
                    p.exportData->buffer = dtk::gl::OffscreenBuffer::create(
                        p.exportData->info.size,
                        offscreenBufferOptions);

                    // Create the progress dialog.
                    p.progressDialog = dtk::ProgressDialog::create(
                        context,
                        "Export",
                        "Rendering:");
                    p.progressDialog->setRange(dtk::RangeD(0.0, p.exportData->range.duration().value() - 1.0));
                    p.progressDialog->setMessage(dtk::Format("Frame: {0} / {1}").
                        arg(p.exportData->frame).
                        arg(p.exportData->range.end_time_inclusive().value()));
                    p.progressDialog->setCloseCallback(
                        [this]
                        {
                            DTK_P();
                            p.exportData.reset();
                            p.progressTimer->stop();
                            p.progressDialog.reset();
                        });
                    p.progressDialog->open(getWindow());
                    p.progressTimer->start(
                        std::chrono::microseconds(500),
                        [this]
                        {
                            DTK_P();
                            _exportFrame();
                            p.progressDialog->setValue(p.exportData->frame - p.exportData->range.start_time().value());
                            const int64_t end = p.exportData->range.end_time_inclusive().value();
                            if (p.exportData->frame <= end)
                            {
                                p.progressDialog->setMessage(dtk::Format("Frame: {0} / {1}").
                                    arg(p.exportData->frame).
                                    arg(end));
                            }
                            else
                            {
                                p.progressDialog->close();
                            }
                        });
                }
                catch (const std::exception& e)
                {
                    if (p.progressDialog)
                    {
                        p.progressDialog->close();
                    }
                    p.exportData.reset();
                    context->getSystem<dtk::DialogSystem>()->message(
                        "ERROR",
                        dtk::Format("Error: {0}").arg(e.what()),
                        getWindow());
                }
            }
        }

        void ExportTool::_exportFrame()
        {
            DTK_P();
            try
            {
                // Get the video.
                OTIO_NS::RationalTime t(p.exportData->frame, p.exportData->range.duration().rate());
                auto video = p.player->getTimeline()->getVideo(t).future.get();

                // Render the video.
                dtk::gl::OffscreenBufferBinding binding(p.exportData->buffer);
                p.exportData->render->begin(p.exportData->info.size);
                p.exportData->render->setOCIOOptions(p.exportData->ocioOptions);
                p.exportData->render->setLUTOptions(p.exportData->lutOptions);
                p.exportData->render->drawVideo(
                    { video },
                    { dtk::Box2I(0, 0, p.exportData->info.size.w, p.exportData->info.size.h) },
                    { p.exportData->imageOptions },
                    { p.exportData->displayOptions },
                    tl::timeline::CompareOptions(),
                    p.exportData->colorBuffer);
                p.exportData->render->end();

                // Write the output image.
                auto image = dtk::Image::create(p.exportData->info);
                glPixelStorei(GL_PACK_ALIGNMENT, p.exportData->info.layout.alignment);
#if defined(dtk_API_GL_4_1)
                glPixelStorei(GL_PACK_SWAP_BYTES, p.exportData->info.layout.endian != dtk::getEndian());
#endif // dtk_API_GL_4_1
                glReadPixels(
                    0,
                    0,
                    p.exportData->info.size.w,
                    p.exportData->info.size.h,
                    p.exportData->glFormat,
                    p.exportData->glType,
                    image->getData());
                p.exportData->writer->writeVideo(t, image);

                ++p.exportData->frame;
            }
            catch (const std::exception& e)
            {
                if (p.progressDialog)
                {
                    p.progressDialog->close();
                }
                p.exportData.reset();
                if (auto context = getContext())
                {
                    context->getSystem<dtk::DialogSystem>()->message(
                        "ERROR",
                        dtk::Format("Error: {0}").arg(e.what()),
                        getWindow());
                }
            }
        }
    }
}
