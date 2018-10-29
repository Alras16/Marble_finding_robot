#ifndef C_VISION_H
#define C_VISION_H
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

class c_vision
{
public:
    struct marble_position
    {
        cv::Point center;
        float radius;
    };

    c_vision();
    void set_image(cv::Mat &image);
    void find_color();
    void find_marbles();
    std::vector<marble_position*> getMarbles();

private:
    cv::Mat ori_image;
    cv::Mat processed_image;
    std::vector<marble_position*> marbles;
    int number_of_white_pixels = 0;
    int canny_threshold;
    bool image_updated;

};

#endif // C_VISION_H
