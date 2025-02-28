#ifndef Q565IMAGEIOHANDLER_HPP
#define Q565IMAGEIOHANDLER_HPP

#include <QImageIOHandler>
#include <QIODevice>

class Q565ImageIOHandler : public QImageIOHandler {
public:
    bool canRead() const override;
    bool read(QImage* image) override;
    bool write(const QImage& image) override;
};

#endif // Q565IMAGEIOHANDLER_HPP
