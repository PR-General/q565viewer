#ifndef Q565IMAGEFORMATPLUGIN_H
#define Q565IMAGEFORMATPLUGIN_H
#include <QObject>
#include <QtPlugin>
#include <QImageIOPlugin>

class Q565ImageFormatPlugin : public QImageIOPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID  "org.qt-project.Qt.QImageIOHandlerFactoryInterface" FILE "q565imageformat.json")

public:
    QImageIOPlugin::Capabilities capabilities(QIODevice *device, const QByteArray &format) const override;
    QImageIOHandler *create(QIODevice *device, const QByteArray &format) const override;
};
#endif
