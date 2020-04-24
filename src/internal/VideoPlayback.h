#ifndef VIDEOPLAYBACK_H
#define VIDEOPLAYBACK_H

#include <QThread>
#include <QAbstractVideoSurface>
#include <QVideoFrame>
#include <QMediaPlayer>
#include <pylon/PylonIncludes.h>

template <class T> class GenericHolder;


class VideoFrameConverter : public QThread
{
public:
    VideoFrameConverter(GenericHolder<QVideoFrame>* in, GenericHolder<QImage>* out);

    void run();

private:
    bool isPlaying;
    GenericHolder<QVideoFrame>* videoHolder;
    GenericHolder<QImage>* imageHolder;
};

class VideoPlaybackSurface : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;
    bool present(const QVideoFrame &frame);

signals:
    void newFrame(QVideoFrame);

private:
    QVideoFrame currentFrame;
};

class VideoPlayback : public QThread
{
    Q_OBJECT

public:
    VideoPlayback(GenericHolder<Pylon::CPylonImage*>* pylonHolder, GenericHolder<QImage>* imageHolder);
    ~VideoPlayback();

    void run();
    void startRecording(QString fileName);
    void stopRecording();
    void playVideo(QString fileName);
    void stopPlaying();

private:
    bool isRecording;
    VideoFrameConverter* frameConverter;
    QMediaPlayer* player;
    VideoPlaybackSurface* surface;
    GenericHolder<Pylon::CPylonImage*>* pylonHolder;
    GenericHolder<QImage>* imageHolder;
    GenericHolder<QVideoFrame>* videoHolder;
    Pylon::CVideoWriter* videoWriter;
    Pylon::CPylonImage* playbackFrame;
    QImage currentFrame;

private slots:
    void acceptVideoFrame(QVideoFrame frame);
    void repeatVideo(QMediaPlayer::MediaStatus status);
};

#endif // VIDEOPLAYBACK_H
