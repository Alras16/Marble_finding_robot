#include "ct.h"
#include "c_vision.h"
#include <QCoreApplication>
#include <QDir>

#include <ctime>

int main(int _argc, char **_argv)
{
    c_vision camera;

    QDir path = QDir::current();
    path.cdUp();
    std::string name = "camera_test_image_6";
    std::string test_name = "camera_detected_marbles_6";
    std::string filePath = path.path().toStdString() + "/test_files/camera_screenshots//" + name + ".png";
    cv::Mat image = cv::imread(filePath);

    camera.set_image(image);
    camera.find_color();
    camera.find_marbles();

    cv::Mat im = camera.getImage();
    //cv::Mat im = camera.hls_histogram();

    //cv::imshow("circle image",im);

    filePath = path.path().toStdString() + "/test_files/camera_screenshots//" + test_name + ".png";
    cv::imwrite(filePath,im);

    cv::waitKey(0);

}
