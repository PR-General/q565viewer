#include <QObject>
#include <QDebug>
#include <QTest>
#include <QJsonArray>
#include <QFile>
#include <QFileInfo>
#include <QCoreApplication>
#include <QImage>

constexpr char Q565_ENCODER[] = "q565-rust";
constexpr char AIOUNCHAINED_PY[] = "aio-unchained-py";
class Q565Benchmarking : public QObject
{
    Q_OBJECT
public:
    explicit Q565Benchmarking(QObject *parent = nullptr)
        : QObject{parent}
    {}

    static void initMain()
    {
        // clean any temporary test cases
        // remove all .q565 files in executable dir
    }


private slots:
    void initTestCase()
    {

    }

    void encodeCompare_data()
    {
        outputDir = QCoreApplication::applicationDirPath();
        qDebug() << "Unit test artifacts will be written to" << outputDir;
        // load test data
        // look for file next to
        QTest::addColumn<QString>("inputFile");
        QTest::addColumn<QString>("outputFile");
        QTest::addColumn<QString>("encoder");
        QTest::addColumn<QString>("testFile");

#ifdef QT_IMAGE_ROOT_DIR
        QString fullPathToImagesSource(QT_IMAGE_ROOT_DIR);
        QString rgbaFolder(fullPathToImagesSource);
        rgbaFolder.append( + "rgba/");
        QString q565RustFolder(fullPathToImagesSource);
        q565RustFolder.append(Q565_ENCODER);
        q565RustFolder.append("/");
        QString q565PyFolder(fullPathToImagesSource);
        q565RustFolder.append("q565py/");

// add files
        QTest::newRow("testing_rust") << QString(rgbaFolder + "testing.png") << QString(outputDir + "/testing.q565") << Q565_ENCODER << QString(q565RustFolder + "testing_rust.png");
        QTest::newRow("testing_py") << QString(rgbaFolder + "testing.png") << QString(outputDir + "/testing.q565") << AIOUNCHAINED_PY << QString(q565PyFolder + "testing_py.png");
        QTest::newRow("testing_h_rust") << QString(rgbaFolder + "testing_h.png") << QString(outputDir + "/testing_h.q565") << Q565_ENCODER << QString(q565RustFolder + "testing_h_rust.png");
        QTest::newRow("testing_h_py") << QString(rgbaFolder + "testing_h.png") << QString(outputDir + "/testing_h.q565") << AIOUNCHAINED_PY << QString(q565PyFolder + "testing_h_py.png");
        QTest::newRow("testing_v_rust") << QString(rgbaFolder + "testing_v.png") << QString(outputDir + "/testing_v.q565") << Q565_ENCODER << QString(q565RustFolder + "testing_v_rust.png");
        QTest::newRow("testing_v_py") << QString(rgbaFolder + "testing_v.png") << QString(outputDir + "/testing_v.q565") << AIOUNCHAINED_PY << QString(q565PyFolder + "testing_v_py.png");
        QTest::newRow("testcard_rgba_rust") << QString(rgbaFolder + "testcard_rgba.png") << QString(outputDir + "/testcard_rgba.q565") << Q565_ENCODER << QString(q565RustFolder + "testcard_rgb_rust.png");
        QTest::newRow("testcard_rgba_py") << QString(rgbaFolder + "testcard_rgba.png") << QString(outputDir + "/testcard_rgba.q565") << AIOUNCHAINED_PY << QString(q565PyFolder + "testcard_rgb_py.png");
        QTest::newRow("solstice_rust") << QString(rgbaFolder + "GM-Pontiac-Solstice.png") << QString(outputDir + "/testcard_rgba.q565") << Q565_ENCODER << QString(q565RustFolder + "testcard_rgb_rust.png");
        QTest::newRow("solstice_py") << QString(rgbaFolder + "GM-Pontiac-Solstice.png") << QString(outputDir + "/testcard_rgba.q565") << AIOUNCHAINED_PY << QString(q565PyFolder + "testcard_rgb_py.png");


#else
        qDebug() << "Missing image source path, no files were added to the test case";
#endif

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
        QBENCHMARK {
            if(inputImage.load(inputFile) ){
                inputImage.save(outputFile, "q565");
            }else {
                QCOMPARE(false, true);
            }
        }
    }
    void decodeCompare_data() {
        QTest::addColumn<QString>("inputFile");
        QTest::addColumn<QString>("outputFile");
        QTest::addColumn<QString>("encoder");
        QTest::addColumn<QString>("testFile");

#ifdef QT_IMAGE_ROOT_DIR
        QString fullPathToImagesSource(QT_IMAGE_ROOT_DIR);
        QString rgbaFolder(fullPathToImagesSource);
        rgbaFolder.append( + "rgba/");
        QString q565RustFolder(fullPathToImagesSource);
        q565RustFolder.append(Q565_ENCODER);
        q565RustFolder.append("/");
        QString q565PyFolder(fullPathToImagesSource);
        q565RustFolder.append("q565py/");

        // add files
        QTest::newRow("testing_rust") << QString(outputDir + "/testing.q565") << QString(outputDir + "/testing.png") << Q565_ENCODER << QString(q565RustFolder + "testing_rust.png");
        QTest::newRow("testing_py") << QString(outputDir + "/testing.q565") << QString(outputDir + "/testing.png") << AIOUNCHAINED_PY << QString(q565PyFolder + "testing_py.png");
        QTest::newRow("testing_h_rust") << QString(outputDir + "/testing_h.q565") << QString(outputDir + "/testing_h.png") << Q565_ENCODER << QString(q565RustFolder + "testing_h_rust.png");
        QTest::newRow("testing_h_py") << QString(outputDir + "/testing_h.q565") << QString(outputDir + "/testing_h.png") << AIOUNCHAINED_PY << QString(q565PyFolder + "testing_h_py.png");
        QTest::newRow("testing_v_rust") << QString(outputDir + "/testing_v.q565") << QString(outputDir + "/testing_v.png") << Q565_ENCODER << QString(q565RustFolder + "testing_v_rust.png");
        QTest::newRow("testing_v_py") << QString(outputDir + "/testing_v.q565") << QString(outputDir + "/testing_v.png") << AIOUNCHAINED_PY << QString(q565PyFolder + "testing_v_py.png");
        QTest::newRow("testcard_rgba_rust") << QString(outputDir + "/testcard_rgba.q565") << QString(outputDir + "/testcard_rgba.png") << Q565_ENCODER << QString(q565RustFolder + "testcard_rgb_rust.png");
        QTest::newRow("testcard_rgba_py") << QString(outputDir + "/testcard_rgba.q565") << QString(outputDir + "/testcard_rgba.png") << AIOUNCHAINED_PY << QString(q565PyFolder + "testcard_rgb_py.png");
        QTest::newRow("solstice_rust") << QString(outputDir + "/M-Pontiac-Solstice.q565") << QString(outputDir + "/testcard_rgba.png") << Q565_ENCODER << QString(q565RustFolder + "testcard_rgb_rust.png");
        QTest::newRow("solstice_py") << QString(outputDir + "/GM-Pontiac-Solstice.q565") << QString(outputDir + "/testcard_rgba.png") << AIOUNCHAINED_PY << QString(q565PyFolder + "testcard_rgb_py.png");


#else
        qDebug() << "Missing image source path, no files were added to the test case";
#endif
    }

    void decodeCompare()
    {
        QFETCH(QString, inputFile);
        QFETCH(QString, outputFile);
        QFETCH(QString, encoder);
        QFETCH(QString, testFile);
        //qDebug() << "[" << testCase++ << "] Testing " << encoder <<  ": " << inputFile <<  "\nagainst\n " << testFile;
        //QFileInfo inputInfo(inputFile);
        QImage inputImage;
        //QCOMPARE(inputInfo.exists(), true);
        QBENCHMARK {
            if(inputImage.load(inputFile) ){
                inputImage.save(outputFile, "png");
            }
        }
    }

    void cleanupTestCase()
    {
        qDebug("Called after myFirstTest and mySecondTest.");
    }
protected:
    QJsonArray testData;
    QString outputDir;
    quint8  testCase = 1;

};

QTEST_MAIN(Q565Benchmarking)
#include "q565benchmarking.moc"
