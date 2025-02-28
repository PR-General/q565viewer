#ifndef Q565TRANSCODER_H
#define Q565TRANSCODER_H

#include <QQuickItem>
#include <QUrl>
#include <QImage>

class Q565Transcoder : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT
public:
    Q565Transcoder(QQuickItem* parent = nullptr);
    Q_INVOKABLE static QString decodeQRGB565(QUrl fileUrl);
    Q_INVOKABLE static QString encodeQARGB32(QUrl fileUrl);

};

#endif // Q565TRANSCODER_H
