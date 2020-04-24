#include "DetectionPainter.h"
#include "DarknetDispatcher.h"
#include "GenericHolder.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;

DetectionPainter::DetectionPainter(GenericHolder<QImage>* inputImage,
                                   GenericHolder<std::vector<detection_with_class>>* dets) :
    isRunning(false),
    holderIn(inputImage),
    darknetHolder(dets)
{
    holderOut = new GenericHolder<QImage>;
}

void DetectionPainter::run()
{
    isRunning = true;
    QImage inputImage;
    std::vector<detection_with_class> dets = {};

    QTimer timer;
    timer.setSingleShot(true);
    QEventLoop loop;
    holderIn->connectSet(&loop, SLOT(quit()), Qt::DirectConnection);
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));

    while (isRunning) {
        timer.start(1000);
        loop.exec();

        if (timer.isActive()) {
            dets = darknetHolder->get();
            inputImage = holderIn->get();
            int width = inputImage.width();
            int height = inputImage.height();
            char** names = darknetInformation.names;

            void* buf = inputImage.scanLine(0);
            Mat mat(inputImage.height(), inputImage.width(), CV_8UC3, buf);

            for (ulong i = 0; i < dets.size(); ++i) {
                box b = dets[i].det.bbox;

                int left  = int((double(b.x) - double(b.w) / 2.) * width);
                int right = int((double(b.x) + double(b.w) / 2.) * width);
                int top   = int((double(b.y) - double(b.h) / 2.) * height);
                int bot   = int((double(b.y) + double(b.h) / 2.) * height);

                if(left < 0) left = 0;
                if(right > width - 1) right = width - 1;
                if(top < 0) top = 0;
                if(bot > height - 1) bot = height - 1;

                std::string str = std::string(names[dets[i].best_class]) + " " + std::to_string(int(std::round(dets[i].det.prob[dets[i].best_class]*100)));

                Scalar color(100, 100, 100, 0);
                if (modelInformation.subpartMap.contains(QString::number(dets[i].best_class))) {
                    SubpartModel subpartModel = *modelInformation.subpartMap[QString::number(dets[i].best_class)];
                    color = Scalar(subpartModel.color.midRef(1, 2).toUInt(nullptr, 16),
                                   subpartModel.color.midRef(3, 2).toUInt(nullptr, 16),
                                   subpartModel.color.midRef(5, 2).toUInt(nullptr, 16), 0.0);
                }

                Point p1(left, top);
                Point p2(right, bot);

                rectangle(mat, p1, p2, color, 5);
                putText(mat, str, p1, FONT_HERSHEY_SIMPLEX, 5, color, 5);
            }
            holderOut->set(inputImage);
        }
    }
}

void DetectionPainter::stop()
{
    isRunning = false;
}

void DetectionPainter::setModelInformation(ModelInformation info)
{
    modelInformation = info;
}

void DetectionPainter::setDarknetInformation(DarknetInformation info)
{
    darknetInformation = info;
}

GenericHolder<QImage>* DetectionPainter::getImageHolder()
{
    return holderOut;
}
