#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imgconvert.h"
#include <QFileDialog>
#include <QMouseEvent>
#include <QGraphicsScene>
#include <QString>
#include <QInputDialog>
#include <qbasictimer.h>
#include <QDebug>
#include "opencv2/opencv.hpp"
//#include "boost/filesystem/path.hpp"

MainWindow::MainWindow(QWidget *parent) :   QMainWindow(parent),ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    defaultFolderPath = "/home/keanu/MyProjects/FractalTool/res";

    // Image data will be "put" in QGraphicsScene instances
    // and than displayed via QGraphicsView components added
    // to the GUI
    sceneP1 = new QGraphicsScene(QRectF(0, 0, ui->viewOriginal->geometry().width(),
                                        ui->viewOriginal->geometry().height()), 0);
    sceneP2 = new QGraphicsScene(QRectF(0, 0, ui->viewProcessed->geometry().width(),
                                        ui->viewProcessed->geometry().height()), 0);

    // Creating connection between scene and view component on GUI
    ui->viewOriginal->setScene(sceneP1);
    ui->viewProcessed->setScene(sceneP2);

    ui->editBrightness->setInputMask("0.00");
    ui->editWidth->setInputMask("0000");
    ui->editHeight->setInputMask("0000");

    // set actions to the buttons (signal-slot system)
    connect(ui->actionOpen, SIGNAL(triggered(bool)),
            this, SLOT(openFileDialog()));

    connect(ui->actionConvert_to_GRAY, SIGNAL(triggered(bool)),
            this, SLOT(convertGray()));

    connect(ui->actionFace, SIGNAL(triggered(bool)),
            this, SLOT(faceDetection()));

    connect(ui->actionConvert_to_B_W, SIGNAL(triggered(bool)),
           this, SLOT(convertBlackWhite()));

    connect(ui->actionSave, SIGNAL(triggered(bool)),
            this, SLOT(saveImage()));

    connect(ui->actionSave_As_2, SIGNAL(triggered(bool)),
            this, SLOT(saveAs()));

    connect(ui->actionExit_2, SIGNAL(triggered(bool)),
            this, SLOT(close()));

    connect(ui->actionCreate_grid, SIGNAL(triggered(bool)),
            this, SLOT(createGrid()));

    connect(ui->actionTEST, SIGNAL(triggered(bool)),
            this, SLOT(test()));

    connect(ui->btnFlush, SIGNAL(clicked(bool)),
            this, SLOT(flushChanges()));

    connect(ui->btnDefaults, SIGNAL(clicked(bool)),
            this, SLOT(restoreDefaults()));
    
    /// timer
    timerOutput    = new QTimer();
    connect(timerOutput, SIGNAL(timeout()),
            this, SLOT(updatePlayer()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//+++
void MainWindow::displayImage(Mat &mat, QGraphicsView* view, QGraphicsScene *scene)
{
    // remove all items from the scene
    scene->clear();

    // creates and add pixmap to the scene
    // used three steps of conversations: cv::Mat -> QIMage -> QPixmap
    scene->addPixmap(QPixmap::fromImage(ConvertImage(mat)));

    // create the bounding rectangle of the scene
    scene->setSceneRect(scene->itemsBoundingRect());
    view->setSceneRect(scene->itemsBoundingRect());

    // no scroll bar. even if the image is larger that display it will be fit in it
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

//+++
void MainWindow::openFileDialog()
{
    // open file dialog. find the image
    imagePath = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    defaultFolderPath,
                                                    tr("Images (*.png *.xpm *.jpg)"));

    // check path (must be not empty to proceed)
    if (!imagePath.isEmpty())
    {
        // load this image
        originalImage = openImage();

        // display the original image
        displayImage(originalImage, ui->viewOriginal, sceneP1);

        // at the beginning original and processed images are the same
        processedImage = openImage();

        // display processed image
        displayImage(processedImage, ui->viewProcessed, sceneP2);
    }

}

//+++
cv::Mat MainWindow::openImage()
{
    String path = imagePath.toUtf8().constData();

    // Read an image stored on HDD
    Mat image = imread( path, CV_LOAD_IMAGE_COLOR );

    // Check if the image is loaded
    if( !image.data )
    {
        // process an exception
        throw "No image loaded . . .";
    };

    return image;
}

//+++
void MainWindow::saveImage()
{
    qDebug() << "inside save";

    if(!imagePath.isEmpty())
    {
        String path = imagePath.toUtf8().constData();

        imwrite( path, processedImage );
    }
}

//+++
void MainWindow::saveAs()
{
    qDebug() << "inside save as";

    QFileDialog dialog(this);

    if(!imagePath.isEmpty())
    {
        QString path = dialog.getSaveFileName(this, "Save as", defaultFolderPath).toUtf8().constData();

        //dialog.setNameFilter(tr("Images (*.png *.xpm *.jpg)"));
        qDebug(path.toLatin1());
        imwrite( path.toUtf8().constData(), processedImage );
    }
}

void MainWindow::flushChanges()
{
    if (bVideoCapture)
    {
        timerOutput->stop();
        bVideoCapture = false;
    }

    // if brightess changed
    if (ui->editBrightness->text() != ".")
    {
        qDebug(ui->editBrightness->text().toUtf8().constData());
        qDebug(ui->editHeight->text().toUtf8().constData());
        double brightness = ui->editBrightness->text().toDouble();
        enhanceBrightness(brightness);

        // display processed image

    }

    // if new width and height is modified
    if ((ui->editWidth->text() != "") && (ui->editHeight->text() != ""))
    {
        // change the size of the image
        changeImageSize( processedImage,
                         ui->editWidth->text().toInt(),
                         ui->editHeight->text().toInt());
    }

    displayImage(processedImage, ui->viewProcessed, sceneP2);
}

void MainWindow::restoreDefaults()
{
    qDebug("inside restore");

    if(!imagePath.isEmpty())
    {
        processedImage = openImage();

        // display processed image
        displayImage(processedImage, ui->viewProcessed, sceneP2);
    }
}

void MainWindow::test()
{
    threshold(originalImage, processedImage, 128, 255, CV_THRESH_BINARY);
    displayImage( processedImage, ui->viewProcessed, sceneP2 );
}

void MainWindow::changeImageSize(cv::Mat img, double x, double y)
{
    // Resize image
    cv::resize( img,
                processedImage,
                cv::Size(x, y),
                0,
                0,
                INTER_LINEAR);
}

//+++
void MainWindow::convertGray()
{
    cvtColor( originalImage, processedImage, CV_BGR2GRAY );
    displayImage( processedImage, ui->viewProcessed, sceneP2 );
}

void MainWindow::createGrid()
{
    for( int i = 0; i < processedImage.rows; i++ )
        for( int j = 0; j < processedImage.cols; j++ )
            if (        ( i % 20 == 10 && j % 2 == 1 ) ||
                        ( j % 50 == 25 && i % 2 == 1 ) )
            {
                processedImage.at<Vec3b>(i,j)[0]= 255;
                processedImage.at<Vec3b>(i,j)[1]= 255;
                processedImage.at<Vec3b>(i,j)[2]= 255;
            }

    // display processed image
    displayImage(processedImage, ui->viewProcessed, sceneP2);
}

void MainWindow::convertBlack(MainWindow::ConvertationMode cm)
{
    cvtColor( originalImage, processedImage, CV_BGR2GRAY );
    displayImage( processedImage, ui->viewProcessed, sceneP2 );
}

void MainWindow::enhanceBrightness(double multi)
{
    processedImage = originalImage * multi;
}

void MainWindow::convertGrayUsingChannels()
{
    // copy original image to the image after processing
    // further work here will be done with processedImage
    originalImage.copyTo( processedImage );

    // image in OpenCV is presented as matrix [i, j]
    // each element of matrix is a pixel
//    for( int i = 0; i < processedImage.rows; i++ )
//        for( int j = 0; j < processedImage.cols; j++ )
//            if (        ( i % 20 == 10 && j % 2 == 1 ) ||
//                        ( j % 50 == 25 && i % 2 == 1 ) )
//            {
//                processedImage.at<Vec3b>(i,j)[0]= 255;
//                processedImage.at<Vec3b>(i,j)[1]= 255;
//                processedImage.at<Vec3b>(i,j)[2]= 255;
//            }

      // each green pixel change to red one
//    for( int i = 0; i < processedImage.rows; i++ )
//        for( int j = 0; j < processedImage.cols; j++ )
//            if (processedImage.at<Vec3b>(i,j)[0] < processedImage.at<Vec3b>(i,j)[1] - 10 &&
//                processedImage.at<Vec3b>(i,j)[2] < processedImage.at<Vec3b>(i,j)[1] - 10 &&
//                processedImage.at<Vec3b>(i,j)[1] > 64 )
//                {
//                    processedImage.at<Vec3b>(i,j)[0]= 0;
//                    processedImage.at<Vec3b>(i,j)[1]= 0;
//                    processedImage.at<Vec3b>(i,j)[2]= 255;
//                }

    // provided access to modify each channel
    for( int i = 0; i < processedImage.rows; i++ )
        for( int j = 0; j < processedImage.cols; j++ )
        {
//            processedImage.at<Vec3b>(i,j)[0]= 0;
            processedImage.at<Vec3b>(i,j)[1]= 0;
//            processedImage.at<Vec3b>(i,j)[2]= 0;
        }
}




void MainWindow::convertBlackWhite()
{
    cvtColor( originalImage, processedImage, CV_BGR2GRAY );
    processedImage = processedImage > 128;
    displayImage( processedImage, ui->viewProcessed, sceneP2 );
}

void MainWindow::faceDetection()
{
    bVideoCapture = true;

    //-- 1. Load the cascades
    if( !face_cascade.load( face_cascade_name ) ){ qDebug("no cascade"); };
    if( !eyes_cascade.load( eyes_cascade_name ) ){ qDebug("no cascade"); };

    //-- 2. Read the video stream
    videoCapture.open( -1 );
    if ( ! videoCapture.isOpened() ) { qDebug("not loaded image"); }

    timerOutput->start (1000/30);
}

void MainWindow::updatePlayer()
{
    videoCapture.read(playerFrame);
    playerFrame.copyTo( originalImage );

    if( playerFrame.empty() )
    {
        qDebug("not loaded image");
    }
    //================================
    std::vector<Rect> faces;
    Mat frame_gray;

    cvtColor( playerFrame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    //-- Detect faces
    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(30, 30) );

    for( size_t i = 0; i < faces.size(); i++ )
    {
        Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );
        ellipse( playerFrame, center, Size( faces[i].width/2, faces[i].height/2), 0, 0, 360, Scalar( 198, 19, 19 ), 4, 8, 0 );

        Mat faceROI = frame_gray( faces[i] );
        std::vector<Rect> eyes;

        //-- In each face, detect eyes
        eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CASCADE_SCALE_IMAGE, Size(30, 30) );

        for( size_t j = 0; j < eyes.size(); j++ )
        {
            Point eye_center( faces[i].x + eyes[j].x + eyes[j].width/2, faces[i].y + eyes[j].y + eyes[j].height/2 );
            int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
            circle( playerFrame, eye_center, radius, Scalar( 183, 198, 18 ), 4, 8, 0 );
        }
    }
    //-- Show what you got
    displayImage(originalImage, ui->viewOriginal, sceneP1);
    processedImage = playerFrame;
    displayImage(processedImage, ui->viewProcessed, sceneP2);
}
