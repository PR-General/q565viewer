#include "q565transcoder.h"
#include <QImage>

Q565Transcoder::Q565Transcoder(QQuickItem* parent)
    : QQuickItem(parent)
{

}

QString Q565Transcoder::decodeQRGB565(QUrl fileUrl)
{
    QImage image;
    QString outputPath;
    if(image.load(fileUrl.path(), "q565")) {
        qDebug() << "Successfully loaded image";
        outputPath = fileUrl.path().replace(".q565", ".png");
        image.save(outputPath, "png");
    } else {
        qDebug() << "failed to load q565 image";
    }
    return "file://" + outputPath;
}

QString Q565Transcoder::encodeQARGB32(QUrl fileUrl)
{
    QImage image;
    QString outputPath;
    if(image.load(fileUrl.path())) {
        outputPath = fileUrl.path();
        QString type = outputPath.mid(outputPath.lastIndexOf("."));
        outputPath = fileUrl.path().replace(type, ".q565");
        qDebug() << "Successfully loaded image";
        image.save(outputPath, "q565");
    } else {
        qDebug() << "failed to load q565 image";
    }
    return "file://" + outputPath;
}
