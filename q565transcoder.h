#ifndef Q565TRANSCODER_H
#define Q565TRANSCODER_H

#include <QQuickItem>
#include <QUrl>
#include <QImage>

class Q565Transcoder : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int sizeIn  READ sizeIn NOTIFY sizeInChanged)
    Q_PROPERTY(int sizeOut READ sizeOut NOTIFY sizeOutChanged)
    Q_PROPERTY(QString textData READ textData NOTIFY textDataChanged)
    int sizeIn() const { return mSizeIn; }
    int sizeOut() const { return mSizeOut; }
    QString textData() const { return mTextData; }

signals:
    void sizeInChanged(int);
    void sizeOutChanged(int);
    void textDataChanged(QString);

public:
    Q565Transcoder(QQuickItem* parent = nullptr);
    Q_INVOKABLE QString decodeQRGB565(QUrl fileUrl);
    Q_INVOKABLE QString encodeQARGB32(QUrl fileUrl);
    Q_INVOKABLE QByteArray  loadFile(QUrl fileUrl);

protected:
    void generateStatements(QUrl);
    int mSizeIn;
    int mSizeOut;
    QString mTextData;
};

#endif // Q565TRANSCODER_H
