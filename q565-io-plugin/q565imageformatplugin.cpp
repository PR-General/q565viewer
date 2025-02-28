#include "q565imageformatplugin.hpp"
#include "q565imageiohandler.hpp"
#include "q565.h"
QImageIOPlugin::Capabilities Q565ImageFormatPlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
    qDebug() << "Plugin capabilities called";

    if (format == Q565_Encoder::Q565_FORMAT_TAG || format.isEmpty()) {

        qDebug() << "Found q565 Format: " << format;
        if(device != nullptr) {
            if (!device->isOpen()) {
                return (QImageIOPlugin::Capabilities)0x0;
            }
            if(device->isWritable()) {
                if(device->isReadable()) {
                    return (CanRead | CanWrite);
                } else {
                    return CanWrite;
                }
            }else {
                if(device->isReadable()) {
                    return CanRead;
                }
            }
        }
    }
    return (QImageIOPlugin::Capabilities)0x0;
}

QImageIOHandler* Q565ImageFormatPlugin::create(QIODevice *device, const QByteArray &format) const
{
    if (format == Q565_Encoder::Q565_FORMAT_TAG || format.isEmpty()) {
        qDebug() << "Invoking create for Q565 image io plugin and handler";
        Q565ImageIOHandler* handler = new Q565ImageIOHandler();
        handler->setDevice(device);
        handler->setFormat(format);
        return handler;
    }
    return nullptr;
}
