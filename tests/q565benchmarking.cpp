#include <QObject>
#include <QDebug>
#include <QJsonArray>
#include <QFileInfo>
#include <QImage>
#include "q565tests.h"
class Q565Benchmarking : public QObject
{
    Q_OBJECT
public:
    explicit Q565Benchmarking(QObject *parent = nullptr)
        : QObject{parent}
    {}

private slots:
    void initTestCase()
    {

        auto testFiles = Q565Test::getTestFileNames();
        auto pathToImages = Q565Test::getImageDataPath();
        auto pathToRustEncodedData = Q565Test::getRustEncodedDataPath();
        auto pathToPythonEncodedData = Q565Test::getPythonEncodedDataPath();
        // for each file remove the artifacts from previous runs

    }

    void encodeCompare_data()
    {
        outputDir = Q565Test::getOutputPath();
        qDebug() << "Unit test artifacts will be written to" << outputDir;
        QDir dir(outputDir);
        if(!dir.exists()) { // make the output directory if it does not exist
            QCOMPARE(dir.mkpath(outputDir),true); // fail if cannot create
        }
        QTest::addColumn<QString>("inputFile");
        QTest::addColumn<QString>("outputFile");
        QTest::addColumn<QString>("encoder");
        QTest::addColumn<QString>("testFile");

        auto testFiles = Q565Test::getTestFileNames();
        auto pathToImages = Q565Test::getImageDataPath();
        auto pathToRustEncodedData = Q565Test::getRustEncodedDataPath();
        auto pathToPythonEncodedData = Q565Test::getPythonEncodedDataPath();

        // For each (raw) input image there is a corresponding python and rust encoded q565 image file
        for(const auto& testFile : std::as_const(testFiles)) {
            // Add the rust encoded data test
            QTest::newRow((testFile + Q565Test::RUST_SIG).toStdString().data()) << pathToImages + testFile + Q565Test::PNG_SUFFIX                                  // png inputFile
                                                                                << outputDir + testFile + Q565Test::Q565_SUFFIX                                    // outputFile
                                                                                << Q565Test::Q565_ENCODER                                                          // encoder
                                                                                << pathToRustEncodedData + testFile + Q565Test::RUST_SIG + Q565Test::Q565_SUFFIX;  // rust testFile
            // Add the python encoded data test
            QTest::newRow((testFile + Q565Test::PY_SIG).toStdString().data()) << pathToImages + testFile + Q565Test::PNG_SUFFIX                                    // png inputFile
                                                                              << outputDir + testFile + Q565Test::Q565_SUFFIX                                      // outputFile
                                                                              << Q565Test::AIOUNCHAINED_PY                                                         // encoder
                                                                              << pathToPythonEncodedData + testFile + Q565Test::PY_SIG + Q565Test::Q565_SUFFIX;    // python testFile
        }
    }

    void encodeCompare()
    {
        QFETCH(QString, inputFile);
        QFETCH(QString, outputFile);
        QFETCH(QString, encoder);
        QFETCH(QString, testFile);
        //qDebug() << "[" << testCase++ << "] Testing " << encoder <<  ": " << inputFile <<  "\nagainst\n " << testFile;
        //QFileInfo inputInfo(inputFile);
        QImage inputImage;
        //QCOMPARE(inputInfo.exists(), true);
        if(inputImage.load(inputFile) ){
            QBENCHMARK {
                QVERIFY(inputImage.save(outputFile, "q565"));
            }
            Q565Test::compareFileSizes(outputFile, encoder, testFile);
        }else {
            QCOMPARE(false, true);
        }
    }


    void decodeCompare_data() {
        outputDir = Q565Test::getOutputPath();
        QTest::addColumn<QString>("encoder");
        QTest::addColumn<QString>("testFile");
        QTest::addColumn<QString>("outputFile");
        QTest::addColumn<QString>("originalImage");


        auto testFiles = Q565Test::getTestFileNames();
        auto pathToRustEncodedData = Q565Test::getRustEncodedDataPath();
        auto pathToPythonEncodedData = Q565Test::getPythonEncodedDataPath();
        auto pathToRawImages = Q565Test::getImageDataPath();

        // For each (raw) input image there is a corresponding python and rust encoded q565 image files
        for(const auto& testFile : std::as_const(testFiles)) {
            QString testName(testFile);
            // Add the rust encoded data test
            QTest::newRow((testName + Q565Test::RUST_SIG).toStdString().data()) << Q565Test::Q565_ENCODER
                                                                                << pathToRustEncodedData + testFile + Q565Test::RUST_SIG + Q565Test::Q565_SUFFIX  // Original Image
                                                                                << outputDir + testFile + Q565Test::RUST_SIG + Q565Test::PNG_SUFFIX               // Original Image
                                                                                << pathToRawImages + testFile  + Q565Test::PNG_SUFFIX;                            // Original Image
            // Add the python encoded data test
            QTest::newRow((testName + Q565Test::PY_SIG).toStdString().data()) << Q565Test::AIOUNCHAINED_PY
                                                                              << pathToPythonEncodedData + testFile + Q565Test::PY_SIG + Q565Test::Q565_SUFFIX // Original Image
                                                                              << outputDir + testFile + Q565Test::PY_SIG + Q565Test::PNG_SUFFIX                // output image
                                                                              << pathToRawImages + testFile  + Q565Test::PNG_SUFFIX;                           // Original Image
            // Decode the previously encoded q565 image
            QTest::newRow(testName.toStdString().data()) << Q565Test::Q565_IMAGE_PLUGIN                         // Encoder
                                                         << outputDir + testFile + Q565Test::Q565_SUFFIX        // Input q565
                                                         << outputDir + testFile + Q565Test::PNG_SUFFIX         // output image
                                                         << pathToRawImages + testFile  + Q565Test::PNG_SUFFIX; // Original Image
        }

    }



    void decodeCompare()
    {
        QFETCH(QString, encoder);
        QFETCH(QString, testFile);
        QFETCH(QString, outputFile);
        QFETCH(QString, originalImage);
        //QFileInfo inputInfo(inputFile);
        QImage inputImage;
        //QCOMPARE(inputInfo.exists(), true);
        bool loadedFile{false};
        QBENCHMARK {
            loadedFile = inputImage.load(testFile);
        }
        QVERIFY(loadedFile);
        if(loadedFile) {
            inputImage.save(outputFile, "png");
        }
        Q565Test::compareImages(originalImage, outputFile, inputImage,Q565Test::originalImages, Q565Test::corruptImages);
    }

    void cleanupTestCase()
    {
        if(Q565Test::corruptImages.size() > 0) {
            Q565Test::showCorruptImages();
            Q565Test::originalImages.clear();
            Q565Test::corruptImages.clear();
        }
    }

protected:
    QJsonArray testData;
    QString outputDir;

};

QTEST_MAIN(Q565Benchmarking)
#include "q565benchmarking.moc"
