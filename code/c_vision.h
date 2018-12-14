#ifndef C_VISION_H
#define C_VISION_H
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include "ct.h"

class c_vision
{
public:
    c_vision();
    void set_image(cv::Mat &image);
    void find_color();
    void find_marbles();
    cv::Mat getImage();
    //void hough_transform(cv::Mat img);
    ct::marble find_closest_marble();
    std::vector<ct::marble> getMarbles();

private:
    cv::Mat ori_image;
    cv::Mat processed_image;
    std::vector<ct::marble> marbles;
    int number_of_white_pixels = 0;
    int canny_threshold;
    bool image_updated;

};

#endif // C_VISION_H
