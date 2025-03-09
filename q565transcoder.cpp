#include "q565transcoder.h"
#include <QImage>
#include <QFileInfo>
#include "q565-io-plugin/q565.h"
Q565Transcoder::Q565Transcoder(QQuickItem* parent)
    : QQuickItem(parent)
{

}

void Q565Transcoder::generateStatements(QUrl fileUrl)
{
    const auto statements = Q565_Encoder::generateStatementListModel(fileUrl.path());

    QString statementFilePath = fileUrl.path().replace(".q565", ".txt");
    QFile statementFile(statementFilePath);
    mTextData.clear();
    if(statementFile.open(QIODevice::WriteOnly)) {
        for(const Q565_Encoder::Q565Statement& statement: statements) {
            switch(statement.type) {
            case Q565_Encoder::HEADER_STATEMENT: {
                QString str("Header: q565. Height: " );
                str.append(QString::number(statement.data));
                str.append(" Width: ");
                str.append(QString::number(statement.q565Pixel));
                str.append("\n");
                mTextData.append(str);
                break;
            }
            case Q565_Encoder::PIXEL_STATEMENT: {
                QString str("Pixel Statement -" );
                str.append(" RGB: ");
                str.append(QString::number(statement.q565Pixel,2));
                str.append(" Red: ");
                str.append(QString::number(statement.q565Pixel >> 11));
                str.append(" Green: ");
                str.append(QString::number((statement.q565Pixel & 0b0000011111100000) >> 5));
                str.append(" Blue: ");
                str.append(QString::number(statement.q565Pixel & 0b0000000000011111));
                str.append( " Pixel: ");
                str.append(QString::number(statement.fullPixel,16));
                str.append("\n");
                mTextData.append(str);
                break;
            }
            case Q565_Encoder::INDEX_STATEMENT: {
                QString str("Index Statement [" );
                str.append(QString::number(statement.data));
                str.append("] RGB: ");
                str.append(QString::number(statement.q565Pixel,2));
                str.append(" Red: ");
                str.append(QString::number(statement.q565Pixel >> 11));
                str.append(" Green: ");
                str.append(QString::number((statement.q565Pixel & 0b0000011111100000) >> 5));
                str.append(" Blue: ");
                str.append(QString::number(statement.q565Pixel & 0b0000000000011111));
                str.append( " Pixel: ");
                str.append(QString::number(statement.fullPixel,16));
                str.append("\n");
                mTextData.append(str);
                break;
            }
            case Q565_Encoder::DIFF_STATEMENT: {
                QString str("Diff Statement [" );
                str.append(QString::number(statement.data, 2).leftJustified(6,'0'));
                str.append("] RGB: ");
                str.append(QString::number(statement.q565Pixel,2));
                str.append(" Red: ");
                str.append(QString::number(statement.q565Pixel >> 11));
                str.append(" Green: ");
                str.append(QString::number((statement.q565Pixel & 0b0000011111100000) >> 5));
                str.append(" Blue: ");
                str.append(QString::number(statement.q565Pixel & 0b0000000000011111));
                str.append( " Pixel: ");
                str.append(QString::number(statement.fullPixel,16));
                str.append("\n");
                mTextData.append(str);
                break;
            }
            case Q565_Encoder::DIFF_INDEX_STATEMENT: {
                QString str("Diff Index Statement [" );
                str.append(QString::number(statement.data));
                str.append("] RGB: ");
                str.append(QString::number(statement.q565Pixel,2));
                str.append(" Red: ");
                str.append(QString::number(statement.q565Pixel >> 11));
                str.append(" Green: ");
                str.append(QString::number((statement.q565Pixel & 0b0000011111100000) >> 5));
                str.append(" Blue: ");
                str.append(QString::number(statement.q565Pixel & 0b0000000000011111));
                str.append( " Pixel: ");
                str.append(QString::number(statement.fullPixel,16));
                str.append("\n");
                mTextData.append(str);
                break;
            }
            case Q565_Encoder::LUMA_STATEMENT: {
                QString str("Luma Statement: [" );
                str.append(QString::number(statement.data));
                str.append("] RGB: ");
                str.append(QString::number(statement.q565Pixel,2));
                str.append(" Red: ");
                str.append(QString::number(statement.q565Pixel >> 11));
                str.append(" Green: ");
                str.append(QString::number((statement.q565Pixel & 0b0000011111100000) >> 5));
                str.append(" Blue: ");
                str.append(QString::number(statement.q565Pixel & 0b0000000000011111));
                str.append( " Pixel: ");
                str.append(QString::number(statement.fullPixel,16));
                str.append("\n");
                mTextData.append(str);
                break;
            }
            case Q565_Encoder::RUN_STATEMENT: {
                QString str("Run Statement: [" );
                str.append(QString::number(statement.data));
                str.append("] RGB: ");
                str.append(QString::number(statement.q565Pixel,2));
                str.append(" Red: ");
                str.append(QString::number(statement.q565Pixel >> 11));
                str.append(" Green: ");
                str.append(QString::number((statement.q565Pixel & 0b0000011111100000) >> 5));
                str.append(" Blue: ");
                str.append(QString::number(statement.q565Pixel & 0b0000000000011111));
                str.append( " Pixel: ");
                str.append(QString::number(statement.fullPixel,16));
                str.append("\n");
                mTextData.append(str);
                break;
            }
            case Q565_Encoder::END_STATEMENT: {
                QString str("Q565 End Statement\n" );
                mTextData.append(str);
                break;
            }
            default:{
                QString str("Inalid Operation Type: ");
                str.append(QString::number(statement.type));
                str.append("\n");
                mTextData.append(str);
                break;
            }
            }
        }
        statementFile.write(mTextData.toStdString().c_str(), mTextData.size());
        statementFile.close();
        emit textDataChanged(mTextData);
    }
}

QString Q565Transcoder::decodeQRGB565(QUrl fileUrl)
{
    QImage image;
    QString outputPath;
    if(image.load(fileUrl.path(), "q565")) {
        QFileInfo fileInfo(fileUrl.path());
        if(fileInfo.exists() && fileInfo.isFile()) {
            mSizeIn = fileInfo.size();
            // Generating statements takes a long time on large files
            generateStatements(fileUrl);
        }
        qDebug() << "Successfully loaded image";
        outputPath = fileUrl.path().replace(".q565", "(2).png");
        image.save(outputPath, "png");
        mSizeOut = image.sizeInBytes();
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
        //image = image.toImageFormat(QPixelFormat());
        mSizeIn = image.sizeInBytes();
        outputPath = fileUrl.path();
        QString type = outputPath.mid(outputPath.lastIndexOf("."));
        outputPath = fileUrl.path().replace(type, ".q565");
        qDebug() << "Successfully loaded image";
        image.save(outputPath, "q565");
        QFileInfo fileInfo(outputPath);
        if(fileInfo.exists() && fileInfo.isFile()) {
            mSizeOut = fileInfo.size();
        }
        qDebug() << "failed to load q565 image";
    }
    return "file://" + outputPath;
}
