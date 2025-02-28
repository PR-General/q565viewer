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
        quint16 rgb;
        quint8  allocatedColors{0};
        quint8 colorIndex{0};
        bool wasAddedToTable{false};
        QHash<quint8, quint16> colorTable;
        colorTable.clear();
        while(!(y > height) && iodevice->read(reinterpret_cast<char*>(&readBuffer), 1) == 1) {
            if(Q565_Encoder::decodeByte(readBuffer, type, data) ){
                switch(type) {
                    case Q565_Encoder::Q565_OP_INDEX: {
                        // data is an index for rgb (0..63)
                        if(colorTable.contains(data)) {
                            rgb = colorTable.value(data);
                            //qDebug() << "Fetched color hash" << data << " with pixel " << QBitArray::fromBits((char*)&rgb, 16);
                            Q565_Encoder::writePixel(result, Q565_Encoder::convertToQRgb(rgb), x, y);
                        } else {
                            qDebug() << "Color index requested not in the color table" << data;
                            qDebug() << "Color Table has " << colorTable.size() << " colors";
                        }
                        break;
                    }
                    case Q565_Encoder::Q565_OP_DIFF: {
                        auto newrgb = Q565_Encoder::getPixelDiff(rgb, data);
                        rgb = newrgb;
                        Q565_Encoder::writePixel(result, Q565_Encoder::convertToQRgb(rgb), x, y);
                        break;
                    }
                    case Q565_Encoder::Q565_OP_LUMA: {
                        if(iodevice->read(reinterpret_cast<char*>(&readBuffer), 1) == 1) {
                            quint8 luma2;
                            Q565_Encoder::decodeByte(readBuffer, type, luma2);
                            if((data & 0b00100000) == 0) { // handle luma
                                auto lumaRgb = Q565_Encoder::getPixelLuma(rgb, data, luma2);
                                rgb = lumaRgb;
                                Q565_Encoder::writePixel(result, Q565_Encoder::convertToQRgb(rgb), x, y);
                            } else { // indexed diff
                                quint8 colorHash = Q565_Encoder::getPixelHashIndex(luma2);
                                if(colorTable.contains(colorHash)) {
                                    quint16 color = colorTable.value(colorHash);
                                    auto diffRgb = Q565_Encoder::getPixelIndexedDiff(color, data, luma2);
                                    rgb = diffRgb;
                                    Q565_Encoder::writePixel(result, Q565_Encoder::convertToQRgb(rgb), x, y);
                                } else {
                                    qDebug() << "Requested invalid color hash index: " << colorHash;
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
                                quint8 colorHash = Q565_Encoder::encodeColorTable(rgb, colorTable, wasAddedToTable);
                                Q565_Encoder::writePixel(result, Q565_Encoder::convertToQRgb(rgb), x, y);
                            } else {
                                // error expected encoded RGB
                                return false;
                            }
                        } else if (data <=61) { // run found
                            //data +=1; // run count has a -1 offset when sent
                            //qDebug() << "Decode: Found run of " << data << "of pixel ";
                            while(data > 0) {
                                Q565_Encoder::writePixel(result, Q565_Encoder::convertToQRgb(rgb), x, y);
                                --data;
                            }
                            Q565_Encoder::writePixel(result, Q565_Encoder::convertToQRgb(rgb), x, y);
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
            // process read buffer
            // decode type
            // decode data based on  type
            // switch(type)
            // if pixel, set current pixel, draw pixel
            // if run, draw current pixel
            // if from color palette, set current pixel color from color palette, draw pixel
            // if
        }
        qDebug() << "processed " << bytesToRead - 8;

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
