#include "VideoDisplay.h"
#include "../internal/GenericHolder.h"
#include "../internal/GlobalVariables.h"
#include <QWidget>
#include <QVideoSurfaceFormat>

VideoSurface::VideoSurface(QWidget *widget, QObject *parent)
    : QAbstractVideoSurface(parent)
    , widget(widget)
    , imageFormat(QImage::Format_Invalid)
{

}

QList<QVideoFrame::PixelFormat> VideoSurface::supportedPixelFormats(
        QAbstractVideoBuffer::HandleType handleType) const
{
    if (handleType == QAbstractVideoBuffer::NoHandle) {
        return QList<QVideoFrame::PixelFormat>()
                << QVideoFrame::Format_RGB24
                << QVideoFrame::Format_ARGB32
                << QVideoFrame::Format_ARGB32_Premultiplied
                << QVideoFrame::Format_RGB565
                << QVideoFrame::Format_RGB555;
    } else {
        return QList<QVideoFrame::PixelFormat>();
    }
}

bool VideoSurface::isFormatSupported(
        const QVideoSurfaceFormat &format, QVideoSurfaceFormat *similar) const
{
    Q_UNUSED(similar)

    const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
    const QSize size = format.frameSize();

    return imageFormat != QImage::Format_Invalid
            && !size.isEmpty()
            && format.handleType() == QAbstractVideoBuffer::NoHandle;
}

bool VideoSurface::start(const QVideoSurfaceFormat &format)
{
    const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
    const QSize size = format.frameSize();

    if (imageFormat != QImage::Format_Invalid && !size.isEmpty()) {
        this->imageFormat = imageFormat;
        imageSize = size;
        sourceRect = format.viewport();

        QAbstractVideoSurface::start(format);

        widget->updateGeometry();
        updateVideoRect();

        return true;
    } else {
        qDebug() << "Invalid image format error: " << imageFormat;
        return false;
    }
}

void VideoSurface::stop()
{
    currentFrame = QVideoFrame();
    targetRect = QRect();

    QAbstractVideoSurface::stop();

    widget->update();
}

bool VideoSurface::present(const QVideoFrame &frame)
{
    emit newFrame(frame); // add some flag maybe
    if (surfaceFormat().pixelFormat() != frame.pixelFormat()) {
        setError(IncorrectFormatError);
        stop();
        qDebug() << "Invalid video format error: " << frame.pixelFormat();
        return false;
    } else {
        currentFrame = frame;
        widget->repaint(targetRect);

        return true;
    }
}

void VideoSurface::updateVideoRect()
{
    QSize size = surfaceFormat().sizeHint();
    size.scale(widget->size().boundedTo(size), Qt::KeepAspectRatio);

    targetRect = QRect(QPoint(0, 0), size);
    targetRect.moveCenter(widget->rect().center());
}

void VideoSurface::paint(QPainter *painter)
{
    if (currentFrame.isValid() && currentFrame.map(QAbstractVideoBuffer::ReadOnly)) {
        const QTransform oldTransform = painter->transform();

        if (surfaceFormat().scanLineDirection() == QVideoSurfaceFormat::BottomToTop) {
           painter->scale(1, -1);
           painter->translate(0, -widget->height());
        }

        QImage image(
                currentFrame.bits(),
                currentFrame.width(),
                currentFrame.height(),
                currentFrame.bytesPerLine(),
                imageFormat);

        QRect imageRect(QRect(0, 0, image.width(), image.height()));
        imageRect.moveCenter(widget->rect().center());
        painter->drawImage(imageRect, image, QRect(0, 0, image.width(), image.height()));

        currentFrame.unmap();
    }
}

QSize VideoSurface::checkSize()
{
    return widget->size();
}


FormatConverterThread::FormatConverterThread(VideoSurface* surface, GenericHolder<QImage>* imageHolder, VideoDisplay* par) :
    parent(par),
    videoSurface(surface),
    holder(imageHolder)
{
    currentFrame = new QVideoFrame;
}

void FormatConverterThread::run()
{
    isPlaying = true;

    QImage qimage;
    QImage scaledImg;
    QEventLoop preLoop;
    holder->connectSet(&preLoop, SLOT(quit()));

    preLoop.exec();
    qimage = holder->get();

    int width = qimage.width();
    int height = qimage.height();

    QVideoFrame::PixelFormat f = QVideoFrame::pixelFormatFromImageFormat(QImage::Format_RGB888);
    QSize frameSize(width, height);
    QVideoSurfaceFormat format(frameSize, f);

    if (!videoSurface->isActive()){
        videoSurface->start(format);
    }

    QTimer timer;
    timer.setSingleShot(true);
    QEventLoop loop;
    holder->connectSet(&loop, SLOT(quit()), Qt::DirectConnection);
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));

    while (isPlaying) {
        timer.start(1000);
        loop.exec();

        if (timer.isActive()) {
            qimage = holder->get();
            scaledImg = qimage;
            scaledImg = (double(parent->width())/parent->height() > double(qimage.width())/qimage.height()) ? qimage.scaledToHeight(parent->height()) : qimage.scaledToWidth(parent->width());
            if (scaledImg.width() && scaledImg.height()) {
                *currentFrame = QVideoFrame(scaledImg);
                emit frameConverted(currentFrame);
            }
        }
    }
}

void FormatConverterThread::stop()
{
    isPlaying = false;
}


VideoDisplay::VideoDisplay(QWidget* parent) :
    QWidget(parent),
    painting(false),
    totalFps(0),
    fpsCounter(0)
{
    videoSurface = new VideoSurface(this, parent);
    QTimer* fpsTimer = new QTimer(this);
    connect(fpsTimer, SIGNAL(timeout()), this, SLOT(calculateFps()));
    fpsTimer->start(1000);
}

void VideoDisplay::start(GenericHolder<QImage>* holder)
{
    conversionThread = new FormatConverterThread(videoSurface, holder, this);
    connect(conversionThread, SIGNAL(frameConverted(const QVideoFrame*)), this, SLOT(renderFrame(const QVideoFrame*)));
    conversionThread->start();
}

void VideoDisplay::stop()
{
    disconnect(conversionThread, SIGNAL(frameConverted(const QVideoFrame*)), this, SLOT(renderFrame(const QVideoFrame*)));

    conversionThread->stop();
    if (!conversionThread->wait(5000)) {
        conversionThread->terminate();
        conversionThread->wait();
    }
    conversionThread->deleteLater();
}

bool VideoDisplay::isPainting()
{
    return painting;
}

void VideoDisplay::paintEvent(QPaintEvent *event)
{
    painting = true;
    QPainter painter(this);

    if (videoSurface->isActive()) {
        const QRect videoRect = videoSurface->videoRect();
        if (!videoRect.contains(event->rect())) {
            QRegion region = event->region();
            region.subtracted(videoRect);
            QBrush brush = palette().background();
            for (const QRect* i = region.begin(); i != region.end(); ++i) {
                painter.fillRect(*i, brush);
            }
        }
        videoSurface->paint(&painter);
    } else {
        painter.fillRect(event->rect(), palette().background());
    }
    painting = false;
}

QSize VideoDisplay::sizeHint() const
{
    QSize s = size();
    s.setWidth(videoSurface->surfaceFormat().sizeHint().width());
    s.setHeight(videoSurface->surfaceFormat().sizeHint().height());
    return s;
}

void VideoDisplay::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    videoSurface->updateVideoRect();
    updateGeometry();
}

void VideoDisplay::renderFrame(const QVideoFrame* frame)
{
    videoSurface->present(*frame);
    totalFps += 1000./(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - currentTime).count());
    fpsCounter++;
    currentTime = std::chrono::high_resolution_clock::now();
}

void VideoDisplay::calculateFps()
{
    emit fpsChanged(totalFps / fpsCounter);
    totalFps = 0;
    fpsCounter = 0;
}

