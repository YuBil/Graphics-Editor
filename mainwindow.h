#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QGraphicsScene>
#include <iostream>

#include "opencv2/opencv.hpp"
#include "qbasictimer.h"
#include <QTimer>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:

    // path to the original image (File -> Open)
    QString         imagePath;
    QString         defaultFolderPath;
    Ui::MainWindow  *ui;                        //  User interface Qt
    QGraphicsScene  *sceneP1;                   //  Used in QGraphicsView to display images (Player 1)
    QGraphicsScene  *sceneP2;                   //  Used in QGraphicsView to display images (Player 2)

    QTimer          *timerOutput;               //  Timer for updating processed image (video)

    /// Images must be stored in cv::Mat format to be proccessed using OpenCV
    Mat         originalImage;
    Mat         processedImage;

    /// Arguments that can be passed to convertation methods
    enum    ImageFormats        { JPEG = 0, BMP };
    enum    ConvertationMode    { RGB = 0, R, G, B };


    /** Global variables  (cascades for face detection) */
    String face_cascade_name = "/home/keanu/webcam_connecdt/haarcascade_frontalface_alt.xml";
    String eyes_cascade_name = "/home/keanu/webcam_connecdt/haarcascade_eye_tree_eyeglasses.xml";
    CascadeClassifier   face_cascade;
    CascadeClassifier   eyes_cascade;
    VideoCapture        videoCapture;
    Mat                 playerFrame;
    bool                bVideoCapture;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void displayImage(cv::Mat &mat, QGraphicsView* view, QGraphicsScene* scene);

    /**
     * @brief openImage
     * @return image in cv::Mat format
     */
    cv::Mat openImage();

    /**
     * @brief changeImageSize
     * Resize image
     * @param img is image to be resized
     * @param x is width of image
     * @param y is height of image
     */
    void changeImageSize(cv::Mat img, double x, double y);



    /**
     * @brief convertBlack
     * @param cm choose what channel to use
     */
    void convertBlack(ConvertationMode cm);

    /**
     * @brief enhanceBrightness
     * @param multi - enhance brightness in "multi" times
     */
    void enhanceBrightness(double multi);

    void convertGrayUsingChannels();

private slots:

    void openFileDialog();

    /**
     * @brief convertGray
     * @param cm choose what channel to use
     */
    void convertGray();

    /**
     * @brief updatePlayer
     * video
     */
    void updatePlayer();

    void createGrid();

    void faceDetection();

    /// TO DO ...
    void saveImage();

    void saveAs();

    void flushChanges();

    void restoreDefaults();

    void convertBlackWhite();

    void test();
};

#endif // MAINWINDOW_H
