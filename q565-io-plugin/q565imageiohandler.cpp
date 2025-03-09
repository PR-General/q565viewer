#include "q565imageiohandler.hpp"
#include "q565.h"
#include <QtEndian>
#include <QDebug>
bool Q565ImageIOHandler::canRead() const {
    // Implement logic to check if the device contains data that can be read by this handler
    qDebug() << "Inovking Q565ImageIOHandler::canRead()";
    return true;
}

bool Q565ImageIOHandler::read(QImage* image) {
    // Implement image reading logic
    qDebug() << "Invoking Q565ImageIOHandler::read()";
    if(image == nullptr) {
        qDebug() << "Q565ImageIOHandler::read - image is null";
    } else {
        auto iodevice = device();
        unsigned char bufferInt[4];
        unsigned char readBuffer;
        quint16 height,width;
        if(iodevice != nullptr) {
            if(iodevice->isOpen()) {
                // read in the header
                iodevice->read(reinterpret_cast<char*>(bufferInt), 4);
                //QString formatCheck(Q565_Encoder::Q565_FORMAT_TAG);
                //if(formatCheck.compare(bufferInt, Qt::CaseInsensitive) == 0) {
                    // read height and width
                bufferInt[2] = 0;
                bufferInt[3] = 0;
                if(iodevice->read(reinterpret_cast<char*>(bufferInt), 2) == 2) {
                    width = (bufferInt[0] & Q565_Encoder::Q565_OP_END);
                    width |= (quint16(bufferInt[1]) << 8);// sent as little endian
                }
                if(iodevice->read(reinterpret_cast<char*>(bufferInt), 2) == 2) {
                    height = (bufferInt[0] & Q565_Encoder::Q565_OP_END);
                    height |= (quint16(bufferInt[1]) << 8);// sent as little endian
                }
                //}
            }
        }
        QImage result(width, height, QImage::Format_ARGB32);
        qDebug() << "Loaded image with size" << result.sizeInBytes();
        // parse each byte and determine what they are
        const int bytesToRead = iodevice->size();
        if(bytesToRead > 1024000000) { // 10 mb
            qDebug() << "File is too large to process";
            return false;
        }
        unsigned char type{0};
        unsigned char data{0};
        int x{0},y{0};
        quint16 rgb = 0;
        quint8  allocatedColors{0};
        quint8 colorIndex{0};
        QMap<quint8, quint16> colorTable;
        colorTable.clear();
        colorTable.insert(0, 0); // white
        colorTable.insert(63, 0xFFFF); // black
        QRgb lastPixel;
        while(!(y > height) && iodevice->read(reinterpret_cast<char*>(&readBuffer), 1) == 1) {
            if(Q565_Encoder::decodeByte(readBuffer, type, data) ){
                switch(type) {
                    case Q565_Encoder::Q565_OP_INDEX: {
                        // data is an index for rgb (0..63)
                        if(colorTable.contains(data)) {
                            rgb = colorTable.value(data);
                            lastPixel = Q565_Encoder::convertToQRgb(rgb);
                            Q565_Encoder::writePixel(result, lastPixel, x, y);
                        } else {
                            qDebug() << "Color index requested not in the color table" << data << " X: " << x << " Y: " << y;
                            qDebug() << "Color Table has " << colorTable.size() << " colors";
                            Q565_Encoder::writePixel(result, lastPixel, x, y);
                        }
                        break;
                    }
                    case Q565_Encoder::Q565_OP_DIFF: {
                        auto newrgb = Q565_Encoder::getPixelDiff(rgb, data);
                        rgb = newrgb;
                        //quint8 colorHash = Q565_Encoder::encodeColorTable(rgb, colorTable, wasAddedToTable);
                        lastPixel = Q565_Encoder::convertToQRgb(rgb);
                        Q565_Encoder::writePixel(result, lastPixel, x, y);
                        continue;
                    }
                    case Q565_Encoder::Q565_OP_LUMA: {
                        if(iodevice->read(reinterpret_cast<char*>(&readBuffer), 1) == 1) {
                            if((data & 0b00100000) == 0) { // handle luma
                                rgb = Q565_Encoder::getPixelLuma(rgb, data, readBuffer);
                                lastPixel = Q565_Encoder::convertToQRgb(rgb);
                                quint8 colorHash = Q565_Encoder::encodeColorTable(rgb, colorTable);
                                //Q565_Encoder::printPixelData(rgb, colorHash, x, y);
                                Q565_Encoder::writePixel(result, lastPixel, x, y);
                            } else { // indexed diff
                                quint8 colorHash = readBuffer & 0b111111;
                                if(colorTable.contains(colorHash)) {
                                    quint16 color = colorTable.value(colorHash);
                                    rgb = Q565_Encoder::getPixelIndexedDiff(color, data, readBuffer);
                                    quint8 colorHash = Q565_Encoder::encodeColorTable(rgb, colorTable);
                                    //Q565_Encoder::printPixelData(rgb, colorHash, x, y);
                                    lastPixel = Q565_Encoder::convertToQRgb(rgb);
                                    Q565_Encoder::writePixel(result, lastPixel, x, y);
                                } else {
                                    qDebug() << "Requested invalid color hash from diff index: " << colorHash << " X: " << x << " Y: " << y;
                                    quint8 colorHash = data & 0b11111;
                                    qDebug() << "Requested invalid color hash from previous data index: " << colorHash << " X: " << x << " Y: " << y;
                                    Q565_Encoder::writePixel(result, lastPixel, x, y);
                                }
                            }
                        } else {
                            qDebug() << "ERROR: Failed to fetch luma second byte";
                            return false;
                        }
                        break;
                    }
                    case Q565_Encoder::Q565_OP_RUN: {
                        if(readBuffer == Q565_Encoder::Q565_OP_RGB565) {
                            bufferInt[0] = 0;
                            bufferInt[1] = 0;
                            bufferInt[2] = 0;
                            bufferInt[3] = 0;
                            if(iodevice->read(reinterpret_cast<char*>(bufferInt), 2) == 2) {
                                rgb = Q565_Encoder::decodePixel(bufferInt[0], bufferInt[1]); // sent little endian (low end first)
                                // write current pixel once,
                                // check if color array is full
                                quint8 colorHash = Q565_Encoder::encodeColorTable(rgb, colorTable);
                                //Q565_Encoder::printPixelData(rgb, colorHash, x, y);
                                lastPixel = Q565_Encoder::convertToQRgb(rgb);
                                Q565_Encoder::writePixel(result, lastPixel, x, y);
                            } else {
                                // error expected encoded RGB
                                qDebug() << "Failed to read pixel data ";
                                //return false;
                            }
                        } else if (data <=61) { // run found
                            data +=1; // run count has a -1 offset when sent
                            //qDebug() << "Decode: Found run of " << data << "of pixel ";
                            while(data > 0) {
                                Q565_Encoder::writePixel(result, lastPixel, x, y);
                                --data;
                            }
                        } else {
                            qDebug() << "Received OP_END " << readBuffer;
                        }
                        break;
                    }
                    default:{
                        qDebug() << "Found incorrect type" << type;
                        break;
                    }
                }
            }
        }
        (*image) = result;
    }
    return true;
}

bool Q565ImageIOHandler::write(const QImage & image) {
    auto iodevice = device();
    if(iodevice != nullptr) {
        QByteArray* byteArray = new QByteArray;
        qDebug() << "Image to write size: " << image.sizeInBytes();
        Q565_Encoder::encode(image, byteArray);
        if(byteArray->size() > 0) {
            qDebug() << "Bytes of data to write: " << byteArray->size();
            iodevice->write(*byteArray);
            delete byteArray;
            return true;
        }
    }
    return false;
}
