#ifndef VIDEODISPLAY_H
#define VIDEODISPLAY_H

#include <QtWidgets>
#include <QtCore/QRect>
#include <QtGui/QImage>
#include <QtMultimedia/QAbstractVideoSurface>
#include <QtMultimedia/QVideoFrame>

template <class T> class GenericHolder;
class VideoDisplay;

class VideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    VideoSurface(QWidget *widget, QObject *parent = nullptr);

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;
    bool isFormatSupported(const QVideoSurfaceFormat &format, QVideoSurfaceFormat *similar) const;

    bool start(const QVideoSurfaceFormat &format);
    void stop();

    bool present(const QVideoFrame &frame);
    void paint(QPainter *painter);

    QRect videoRect() const { return targetRect; }
    void updateVideoRect();

    QSize checkSize();

signals:
    void newFrame(QVideoFrame);

private:
    QWidget *widget;
    QImage::Format imageFormat;
    QRect targetRect;
    QSize imageSize;
    QRect sourceRect;
    QVideoFrame currentFrame;
};


class FormatConverterThread : public QThread
{
    Q_OBJECT

public:
    FormatConverterThread(VideoSurface* surface, GenericHolder<QImage>* holder, VideoDisplay* parent);

    void run();
    void stop();

signals:
    void frameConverted(const QVideoFrame*);

private:
    bool isPlaying;
    VideoDisplay* parent;
    VideoSurface* videoSurface;
    QVideoFrame* currentFrame;
    GenericHolder<QImage>* holder;
};


class VideoDisplay : public QWidget
{
    Q_OBJECT

public:
    VideoDisplay(QWidget* parent = nullptr);
    void start(GenericHolder<QImage>* holder);
    void stop();
    bool isPainting();

signals:
    void fpsChanged(double);

protected:
    void paintEvent(QPaintEvent* event);
    QSize sizeHint() const;
    void resizeEvent(QResizeEvent* event);

private slots:
    void renderFrame(const QVideoFrame*);
    void calculateFps();

private:
    bool painting;
    std::chrono::steady_clock::time_point currentTime;
    double totalFps;
    long long fpsCounter;
    FormatConverterThread* conversionThread;
    VideoSurface* videoSurface;
    QAction* captureAct;
    QAction* detectAct;
    QAction* saveAct;
};

#endif // VIDEODISPLAY_H
