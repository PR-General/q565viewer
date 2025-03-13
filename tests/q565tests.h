#include <QString>
#include <QCoreApplication>
#include <QFile>
#include <QTest>
#include <QtQml/qqmlapplicationengine.h>
#include <QtQml/qqmlengine.h>
#include <QQmlContext>

namespace Q565Test{
    static constexpr char Q565_ENCODER[] = "q565-rust";
    static constexpr char AIOUNCHAINED_PY[] = "aio-unchained-py";
    static constexpr char Q565_IMAGE_PLUGIN[] = "q565-qt-image-plugin";
    static constexpr char RAW_IMAGES[] = "raw/";
    static constexpr char RESULTS_DIR[] = "/results/";
    static constexpr char PNG_SUFFIX[] = ".png";
    static constexpr char Q565_SUFFIX[] = ".q565";
    static constexpr char RUST_SIG[] = "_rust";
    static constexpr char PY_SIG[] = "_py";
    static QList<QString> originalImages;
    static QList<QString> corruptImages;
    static QString getRootPath()
    {
        QString rootPath;
#ifdef QT_IMAGE_ROOT_DIR
        rootPath.append(QT_IMAGE_ROOT_DIR);
#else
        rootPath.append(QCoreApplication::applicationDirPath() + "/images/");
#endif
        return rootPath;
    }

    static QString getRustEncodedDataPath()
    {
        QString pathToRustEncodedData(getRootPath());
        pathToRustEncodedData.append(Q565_ENCODER);
        pathToRustEncodedData.append("/");
        return pathToRustEncodedData;
    }
    static QString getPythonEncodedDataPath()
    {
        QString pathToPythonEncodedData(getRootPath());
        pathToPythonEncodedData.append(AIOUNCHAINED_PY);
        pathToPythonEncodedData.append("/");
        return pathToPythonEncodedData;
    }
    static QString getImageDataPath()
    {
        QString pathToImageData(getRootPath());
        pathToImageData.append(RAW_IMAGES);
        return pathToImageData;
    }

    static QString getOutputPath()
    {
        QString outputPath(QCoreApplication::applicationDirPath());
        outputPath.append(RESULTS_DIR);
        return outputPath;
    }

    static QStringList getTestFileNames()
    {
        QStringList testFiles{"testing", "testing_h", "testing_v", "testcard_rgba", "GM-Pontiac-Solstice"};
        return testFiles;
    }

    static void compareFileSizes(QString generatedFile, QString encoder, QString testFile)
    {
        QFile gFile(generatedFile);
        QFile tFile(testFile);
        // test the files open
        QCOMPARE(gFile.open(QIODeviceBase::ReadOnly), true);
        QCOMPARE(tFile.open(QIODeviceBase::ReadOnly), true);
        // test the resulting file size is less than or equal (smaller)
        QCOMPARE_LE(gFile.size(), tFile.size());
    }


    struct ImageMetrics {
        QMap<QRgb, QColor> colorsFound;

    };

    static ImageMetrics findImageMetrics(QImage image)
    {
        ImageMetrics metrics;
        unsigned int* line;
        QRgb rgb;
        for(int y = 0; y < image.height(); ++y) {
            line = (QRgb*)image.scanLine(y);
            for(int x = 0; x < image.width(); ++x) {
                rgb = line[x];
                if(!metrics.colorsFound.contains(rgb)) { // final all unique colors
                    metrics.colorsFound.insert(rgb, QColor(rgb));
                }
            }
        }
        return metrics;
    }

    static void compareImages(QString originalImagePath, QString decodedPath,  QImage  decodedImage,QList<QString>& originalImages, QList<QString>& corruptImages)
    {
        QImage ogImage;
        // Load the original file
        QVERIFY(ogImage.load(originalImagePath));

        // compare the two image sizes
        QCOMPARE(decodedImage.height(), ogImage.height());
        QCOMPARE(decodedImage.width(), ogImage.width());
        // fetch the metrics for each image
        auto ogMetrics = findImageMetrics(ogImage);
        auto decodedMetrics = findImageMetrics(decodedImage);

        // compare the number of colors found
        if(decodedMetrics.colorsFound.size() !=  ogMetrics.colorsFound.size()) {
            qDebug() << "Found corrupt image";
            if(!corruptImages.contains(decodedPath)) {
                originalImages.append(originalImagePath);
                corruptImages.append(decodedPath);
            }
        }
        QCOMPARE(decodedMetrics.colorsFound.size(), ogMetrics.colorsFound.size());
    }

    static void showCorruptImages()
    {
#ifdef Q565_TEST_QML_MAIN
        qApp->setApplicationName("Corrupt Image Viewer");
        //qmlRegisterType<Q565Transcoder>("com.q565viewer.components", 1, 0, "Q565Transcoder");
        QQmlApplicationEngine engine(qApp);
        engine.rootContext()->setContextProperty("CorruptImages", corruptImages);
        engine.rootContext()->setContextProperty("OriginalImages", originalImages);
        engine.load(Q565_TEST_QML_MAIN);
        qApp->exec();
#endif
    }
}
