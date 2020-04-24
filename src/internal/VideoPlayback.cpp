#include "VideoPlayback.h"
#include "GenericHolder.h"
#include <QtWidgets>
#include <QDebug>

using namespace Pylon;

VideoFrameConverter::VideoFrameConverter(GenericHolder<QVideoFrame>* in, GenericHolder<QImage>* out) :
    isPlaying(false),
    videoHolder(in),
    imageHolder(out)
{

}

void VideoFrameConverter::run()
{
    isPlaying = true;

    QVideoFrame frame;
    QImage qimage;
    QEventLoop preLoop;

    QTimer timer;
    timer.setSingleShot(true);
    QEventLoop loop;
    videoHolder->connectSet(&loop, SLOT(quit()));
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));

    while (isPlaying) {
        timer.start(1000);
        loop.exec();

        if (timer.isActive()) {
            frame = videoHolder->get();
            frame.map(QAbstractVideoBuffer::ReadOnly);
            QImage img(frame.bits(),
                       frame.width(),
                       frame.height(),
                       frame.bytesPerLine(),
                       frame.imageFormatFromPixelFormat(frame.pixelFormat()));
            frame.unmap();
            qimage = img.convertToFormat(QImage::Format_RGB888);
            imageHolder->set(qimage);
        }
    }
}

QList<QVideoFrame::PixelFormat> VideoPlaybackSurface::supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType) const
{
    Q_UNUSED(handleType)

    return QList<QVideoFrame::PixelFormat>()
            << QVideoFrame::Format_RGB24
            << QVideoFrame::Format_RGB32
            << QVideoFrame::Format_ARGB32
            << QVideoFrame::Format_ARGB32_Premultiplied
            << QVideoFrame::Format_RGB565
            << QVideoFrame::Format_RGB555;
}

bool VideoPlaybackSurface::present(const QVideoFrame &frame)
{
    currentFrame = frame;
    emit newFrame(currentFrame);
    return true;
}

VideoPlayback::VideoPlayback(GenericHolder<CPylonImage*>* pHolder, GenericHolder<QImage>* iHolder) :
    isRecording(false),
    pylonHolder(pHolder),
    imageHolder(iHolder)
{
    player = new QMediaPlayer(this);
    videoHolder = new GenericHolder<QVideoFrame>;
    frameConverter = new VideoFrameConverter(videoHolder, imageHolder);
    videoWriter = new CVideoWriter;
    playbackFrame = new CPylonImage;
    surface = new VideoPlaybackSurface;
    connect(surface, SIGNAL(newFrame(QVideoFrame)), this, SLOT(acceptVideoFrame(QVideoFrame)));
    videoWriter->SetParameter(1920, 1200, PixelType_RGB8planar, 40, 100);

    frameConverter->start();
}

VideoPlayback::~VideoPlayback()
{
    surface->deleteLater();
    delete playbackFrame;
    delete videoWriter;
    player->deleteLater();
}

void VideoPlayback::run()
{
    try {
        QTimer timer;
        timer.setSingleShot(true);
        QEventLoop loop;

        QEventLoop preLoop;
        pylonHolder->connectSet(&preLoop, SLOT(quit()));
        preLoop.exec();

        CPylonImage pylonImage = *pylonHolder->get();

        pylonHolder->connectSet(&loop, SLOT(quit()));
        connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));

        while (isRecording) {
            timer.start(1000);
            loop.exec();

            if (timer.isActive()) {
                pylonImage = *pylonHolder->get();
                if (pylonImage.IsValid()){
                    videoWriter->Add(pylonImage);
                }
            }
        }
        videoWriter->Close();

    } catch (GenericException &e) {
        qDebug() << e.GetDescription();
    }
}

void VideoPlayback::startRecording(QString fileName)
{
    videoWriter->Open(fileName.toStdString().c_str());
    isRecording = true;
    start();
}

void VideoPlayback::stopRecording()
{
    isRecording = false;
}

void VideoPlayback::playVideo(QString fileName)
{
    connect(player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(repeatVideo(QMediaPlayer::MediaStatus)));
    player->setVideoOutput(surface);
    player->setMedia(QUrl::fromLocalFile(fileName));
    player->play();
}

void VideoPlayback::acceptVideoFrame(QVideoFrame f)
{
    videoHolder->set(f);
}

void VideoPlayback::repeatVideo(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::MediaStatus::EndOfMedia)
        player->play();
}

void VideoPlayback::stopPlaying()
{
    disconnect(player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(repeatVideo(QMediaPlayer::MediaStatus)));
}
