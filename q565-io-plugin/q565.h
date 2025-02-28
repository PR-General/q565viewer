#ifndef Q565_H
#define Q565_H
#include <QRgb>
#include <QHash>
#include <QDebug>
#include <QtEndian>
#include <QImage>
#include <QBitArray>
namespace Q565_Encoder {
    constexpr unsigned short MAX_COLORS = 64;
    constexpr unsigned short Q565_OP_RGB565 =  0xFE; // 0b11111110
    constexpr unsigned short Q565_OP_INDEX  = 0b00000000;
    constexpr unsigned short Q565_OP_DIFF   = 0b01000000;
    constexpr unsigned short Q565_OP_LUMA   = 0b10000000;
    constexpr unsigned short Q565_OP_RUN    = 0b11000000;
    constexpr unsigned short Q565_TYPE_MASK = 0b11000000;
    constexpr unsigned short Q565_DATA_MASK = 0b00111111;
    constexpr unsigned short Q565_OP_END = 0xFF;
    const int QRGB_ARRAY_SIZE = 1228809; // 1MB
    constexpr char Q565_FORMAT_TAG[] = "q565";

    static quint16 get565Pixel(QRgb rgb){ // assumed 8,8,8 bit rgb
        quint16 out{0};
        auto red = qRed(rgb);
        red /= 255;
        red *= 31;
        out = red << 11;
        auto green = qGreen(rgb) / 255 * 63;
        out |= green << 5;
        auto blue = qBlue(rgb) / 255  * 31;
        out |= blue;
        return  out;
    }
    static quint16 get565Pixel2(QRgb rgb){ // assumed 8,8,8 bit rgb
        auto red = (qRed(rgb) * 249 + 1014) >> 11 ;
        auto green = (qGreen(rgb) * 253 + 505) >> 10 ;
        auto blue = (qBlue(rgb) * 249 + 1014) >> 11;
        return  red << 11 | green << 5 | blue;
    }
    static quint8 wrapping_add(quint8 a, quint8 b) {
        quint16 c{a};
        c += b;
        if(c > 255) {
            c -= 255;
        }
        return (quint8)(c);
    }

    static qint8 wrapping_sub(quint8 a, quint8 b) {
        if( a >= b) {
            return a - b;
        }
        qint8 c(b - a);
        c *= -1;
        return  c;
    }

    static quint16 decodePixel(quint8 low, quint8 high)
    {
        return ((high << 8) | low);
    }

    static quint8 pixelHash(quint16 pixel) {
        unsigned short a = (pixel & 0b1111111100000000) >> 8;
        unsigned short b = pixel & 0b0000000011111111;
        return (wrapping_add(a,b) & 0b111111); // % 64
    }

    static quint16 encodeColorTable(quint16 pixel, QHash<quint8, quint16> & colorTable, bool& wasAdded) {
        // check if the colorTable already exists
        quint8 colorHash = MAX_COLORS + 1;
        wasAdded = false;
        if(colorTable.size() < MAX_COLORS) {
            colorHash = pixelHash(pixel);
            //qDebug() << "pixel: " << pixel << " hash: " << colorHash;
            if(!colorTable.contains(colorHash)) {
                qDebug() << "Added color hash" << colorHash << " for pixel " << QBitArray::fromBits((char*)&pixel, 16) << " - Colors: " << colorTable.size() + 1;
                wasAdded = true;
                colorTable.insert(colorHash, pixel);
            } else {
                quint16 existingColor = colorTable.value(colorHash);
                if(existingColor != pixel) {
                    colorHash = MAX_COLORS + 1; // invalidate it
                }
            }
        }
        return colorHash;
    }
    static void encodeFullPixel(QByteArray* encodeArray, quint16 currentPixel) {
        encodeArray->append(Q565_Encoder::Q565_OP_RGB565); // RGB565 (full pixel marker)
        encodeArray->append((currentPixel & Q565_Encoder::Q565_OP_END)); // 565 Low byte
        encodeArray->append((currentPixel >> 8) & Q565_Encoder::Q565_OP_END); // 565 high byte
    }
    static bool decodeByte(unsigned char byte, unsigned char& type, unsigned char& data) {
        if(byte == Q565_OP_END) {
            return false;
        }
        type = byte & Q565_TYPE_MASK;
        data = byte & Q565_DATA_MASK;
        return true;
    };

    static QByteArray encode(const QImage& image, QByteArray* encodeArray = nullptr) {
        if(encodeArray == nullptr) {
            // allocate an array for the size
            encodeArray = new QByteArray;
            encodeArray->reserve(QRGB_ARRAY_SIZE);
        } else {
            encodeArray->resize(0); // move to the front of the buffer (assumed preallocated memory)
        }
        QImage convertedImage = image;//convertToFormat(QImage::Format_RGB16);
        encodeArray->append(Q565_FORMAT_TAG); // write the magic header
        qDebug() << "Encoding image " << convertedImage.height() << "x" << convertedImage.width();
        quint16 w16(convertedImage.width());
        quint16 h16(convertedImage.height());
        // encode in little endian, 0xFF
        encodeArray->append(w16 & 0xFF);
        encodeArray->append(w16 >> 8);
        encodeArray->append(h16 & 0xFF);
        encodeArray->append(h16 >> 8);
        //encodeArray->append(image.height() & 0xFF);
        //encodeArray->append(image.height() & 0xFF00);
        quint16 previous16 = 0;
        qint8 r_diff, g_diff, b_diff;
        quint8 r_new, g_new, b_new;
        quint8 r_prev, g_prev, b_prev  = -1;
        quint16 currentPixel = 0;
        QHash<quint8, quint16> colorTable;
        qDebug() << "Colors in table: " << colorTable.size();
        //colorTable.clear();
        bool wasAddedToTable = false;
        quint8 colorIndex{0};
        int rg_diff, bg_diff;
        unsigned char run{0};
        QRgb rgb{0xFF};
        QRgb previous;
        QRgb* line = nullptr;
        quint32 pixelsWrittenOfOneColor = 0;
        for(int y = 0; y < convertedImage.height(); ++y) {
            line = (QRgb*)convertedImage.scanLine(y);
            for(int x = 0; x < convertedImage.width(); ++x) {
                rgb = line[x];
                /*** Encode entire image using next two lines ***/
                // currentPixel = get565Pixel2(rgb);
                currentPixel = get565Pixel2(rgb);
                //encodeFullPixel(encodeArray, currentPixel);
                if((previous16 == currentPixel) && ( y > 0 | x > 0) ) { // if diff is 0 for rgb!
                    if( run > 61) { // 62
                        pixelsWrittenOfOneColor += run;
                        run -= 1;
                        //qDebug() << "Encoding run of " << run << "of pixel " << QBitArray::fromBits((char*)&currentPixel, 16);
                        encodeArray->append(Q565_Encoder::Q565_OP_RUN | run); // End the current run
                        run = 1;
                    } else {
                        ++run;
                        //encodeArray->append(); // End the current run and increment
                        continue;
                    }
                } else {
                    if(run >= 1) { // existing run going, finish it up
                        pixelsWrittenOfOneColor += run;
                        run -= 1; // decrement by two as the initial value added 1
                        //qDebug() << "Switching colors and run of " << run << "of pixel " << QBitArray::fromBits((char*)&currentPixel, 16);
                        encodeArray->append(Q565_Encoder::Q565_OP_RUN | run); // End the current run
                        run = 0; // reset
                    }
                    r_new = (currentPixel >> 11) & 0b11111;
                    g_new = (currentPixel >> 5)  & 0b111111;
                    b_new = currentPixel         & 0b11111;

                    r_prev = (previous16 >> 11) & 0b11111;
                    g_prev = (previous16 >> 5)  & 0b111111;
                    b_prev = previous16         & 0b11111;

                    r_diff = wrapping_sub(r_new, r_prev) + 2;
                    g_diff = wrapping_sub(g_new, g_prev) + 2;
                    b_diff = wrapping_sub(b_new, b_prev) + 2;
                    // prefer diff over color table
                    if ((r_diff <= 3 && r_diff >= 0) && (g_diff <= 3 && g_diff >= 0) && (b_diff <= 3 && b_diff >= 0) ){ // within 2 bit difference, Create a OP_DIFF Byte
                        encodeArray->append(Q565_OP_DIFF | r_diff << 4 | g_diff << 2 | b_diff);
                        //encodeFullPixel(encodeArray, currentPixel);
                    } else {
                        //qDebug() << "Red: " << r_new << " Green: " << g_new << " Blue: " << b_new;
                        colorIndex = encodeColorTable(currentPixel, colorTable, wasAddedToTable);
                        if(colorIndex < MAX_COLORS) {
                            if(wasAddedToTable) {
                                encodeFullPixel(encodeArray, currentPixel);
                            } else {
                                encodeArray->append(colorIndex); // 565_OP_INDEX
                            }
                        }
                        else { // color table is full, time to use it
                            encodeFullPixel(encodeArray, currentPixel);
                            //encodeFullPixel(encodeArray, currentPixel);
                            // rg_diff = r_diff - g_diff;
                            // bg_diff = b_diff - g_diff;
                            // if((rg_diff <= 7 && r_diff >= -8)
                            //    &&(bg_diff <= 7 && bg_diff >= -8)
                            //    && (g_diff <= 15 && g_diff >= -16)) { // Create a OP_Luma value
                            //     quint8 luma1 = Q565_OP_LUMA | (g_diff & 0b11111);
                            //     encodeArray->append(luma1);
                            //     quint8 luma2 = (rg_diff & 0b1111) << 4 | (bg_diff & 0b1111);
                            //     encodeArray->append(luma2);
                            // } else { // else encode a full pixel
                            //   encodeFullPixel(encodeArray, currentPixel);
                            //}

                        }
                    }
                }
                previous16 = currentPixel;
                previous = rgb;
            }
        }
        if(run > 1) { // existing run going, finish it up
            --run; // decrement run
            //qDebug() << "Switching colors and run of " << run << "of pixel " << QBitArray::fromBits((char*)&currentPixel, 16);
            encodeArray->append(Q565_Encoder::Q565_OP_RUN | run); // End the current run
            run = 1; // reset
        }
        encodeArray->append(Q565_OP_END);
        return (*encodeArray);
    }
    static void writePixel(QImage& image, QRgb rgb, int& x, int& y) {
        // todo convert rgb16 to RGBA32
        if(x >= image.width()) {
            x = 0;
            ++y;
        }
        image.setPixel(x++,y, rgb);

    }
    static void writeRun(QImage& image, QRgb rgb, int& x, int& y, unsigned short count) {
        for(int i = 0; i < count; i++) {
            writePixel(image, rgb, x, y);
        }
    }
    static QRgb convertToQRgb(quint16 rgb16) {

        QRgb qrgb;
        quint8 r,g,b;
        r = (rgb16 & 0b1111100000000000) >> 11;
        g = (rgb16 & 0b0000011111100000) >> 5;
        b = (rgb16 & 0b0000000000011111);
        r = (r * 255)/ 31;
        g = (g * 255)/ 63;
        b = (b * 255)/ 31;
        return 0xFF000000 | r << 16 | g << 8 | b;
    }

    static quint8 adjustPixelByDiff(quint8 value, quint8 valueDiff)
    {
        if(valueDiff >= 2) {
            value += valueDiff - 2;
        } else {
            if( value > valueDiff) {
                value = value - (2 - valueDiff);
            }
        }
        return value;
    }

    static quint16 getPixelDiff(quint16 previous, quint8 difference)
    {
        // get previous color values
        quint8 r = (previous & 0b1111100000000000) >> 11;
        quint8 g = (previous & 0b0000011111100000) >> 5;
        quint8 b = (previous & 0b0000000000011111);

        // get difference data from 8 bits
        quint8 rDiff = (difference >> 4 ) & 0b11;
        quint8 gDiff = (difference >> 2 ) & 0b11;
        quint8 bDiff = difference & 0b11;
        r = adjustPixelByDiff(r, rDiff);
        g = adjustPixelByDiff(g, gDiff);
        b = adjustPixelByDiff(b, bDiff);
        quint16 pixel{r};
        pixel = pixel << 6;
        pixel |= g;
        return pixel << 5 | b;
    }

    static quint16 getPixelLuma(quint16 previous, quint8 luma1, quint8 luma2)
    {
        // get previous color values
        quint8 r = (previous & 0b1111100000000000) >> 11;
        quint8 g = (previous & 0b0000011111100000) >> 5;
        quint8 b = (previous & 0b0000000000011111);

        quint8 gDiff = (luma1 & 0b00011111) - 16;
        quint8 rDiff = ((luma2 >> 4) & 0x0F) - 8 + gDiff;
        quint8 bDiff = (luma2 & 0x0F) - 8 + gDiff;

        r = adjustPixelByDiff(r, rDiff);
        g = adjustPixelByDiff(g, gDiff);
        b = adjustPixelByDiff(b, bDiff);
        quint16 pixel{r};
        pixel = pixel << 6;
        pixel |= g;
        return pixel << 5 | b;
    }
    static quint8 getPixelHashIndex(quint8 indexed2)
    {
        return (indexed2 & 0b00111111);
    }

    static quint16 getPixelIndexedDiff(quint16 indexedColor, quint8 indexed1, quint8 indexed2)
    {
        quint8 r = (indexedColor & 0b1111100000000000) >> 11;
        quint8 g = (indexedColor & 0b0000011111100000) >> 5;
        quint8 b = (indexedColor & 0b0000000000011111);
        quint8 rDiff = (indexed1 & 0b00000011) - 2;
        quint8 gDiff = ((indexed1 & 0b00011100) >> 2) - 4;
        quint8 bDiff = (indexed2 >> 6) - 2;

        r = adjustPixelByDiff(r, rDiff);
        g = adjustPixelByDiff(g, gDiff);
        b = adjustPixelByDiff(b, bDiff);
        quint16 pixel{r};
        pixel = pixel << 6;
        pixel |= g;
        return pixel << 5 | b;
    }
};
#endif // Q565_H
