#include "c_vision.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <vector>

c_vision::c_vision()
{
    canny_threshold = 5;
    image_updated = false;
}

void c_vision::set_image(cv::Mat &image)
{
    resize(image,ori_image,cv::Size(image.cols / 2, image.rows / 2));
    image_updated = true;
}

void c_vision::find_color()
{
    if (image_updated)
    {
        number_of_white_pixels = 0;
        float hue_color = 120.0;
        float deviation = 2.0;
        //Convert to HLS
        cv::Mat hls_image;
        cv::cvtColor(ori_image, hls_image, CV_BGR2HLS);

        // Make one channel image for result
        int image_w =hls_image.cols; // cols
        int image_h = hls_image.rows; // rows
        cv::Mat binary_image(image_h,image_w,CV_8UC1,cv::Scalar(0));

        for (int i = 0; i < image_h; i++)
            for (int j = 0; j < image_w; j++)
            {
                cv::Vec3b cur_pixel = *hls_image.ptr<cv::Vec3b>(i,j);
                float hue = cur_pixel[0];

                if ((hue >= (hue_color - deviation)) && (hue <= (hue_color + deviation)))
                {
                    number_of_white_pixels++;
                    *binary_image.ptr<int>(i,j) = 255;
                }
            }
        //processed_image = binary_image.clone();
        processed_image = binary_image.clone();
        image_updated = false;
        //cv::imshow("Color found",binary_image);
    }


}

void c_vision::find_marbles()
{
    if (image_updated)
    {
        find_color();

        int image_w = processed_image.cols; // cols
        int image_h = processed_image.rows; // rows
        cv::Mat temp_image(image_h,image_w,CV_8UC3,cv::Scalar(0,0,0));

        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierachy;

        cv::findContours(processed_image,contours,hierachy,CV_RETR_TREE,CV_CHAIN_APPROX_NONE);

        std::vector<int> min_row;
        std::vector<int> max_row;
        std::vector<int> min_col;
        std::vector<int> max_col;
        for (unsigned int contour = 0; contour < contours.size(); contour++)
        {
            min_row.push_back(image_h - 1);
            max_row.push_back(0);
            min_col.push_back(image_h - 1);
            max_col.push_back(0);
            std::vector<cv::Point> points = contours[contour];
            for (unsigned int point = 0; point < points.size(); point++)
            {
                if (points[point].y < min_row[contour])
                    min_row[contour] = points[point].y;

                if (points[point].y > max_row[contour])
                    max_row[contour] = points[point].y;

                if (points[point].x < min_col[contour])
                    min_col[contour] = points[point].x;

                if (points[point].x > max_col[contour])
                    max_col[contour] = points[point].x;
            }
        }

        std::cout << "number of contours: " << contours.size() << std::endl;

        std::vector<cv::Vec3f> circles;

        ct::marble *new_marble;

        for (unsigned int i = 0; i < contours.size(); i++)
        {
            new_marble->center.x = ((max_col[i] - min_col[i])/2) + min_col[i];
            new_marble->center.y = ((max_row[i] - min_row[i])/2) + min_row[i];
            new_marble->distance_to_center = ((max_col[i] - min_col[i])/2) + min_col[i] - ori_image.cols / 2;
            new_marble->radius = (max_row[i] - min_row[i])/2;
        }

        marbles.push_back(new_marble);

        for (unsigned int i = 0; i < circles.size(); i++)
        {
            // circle center
            cv::circle( temp_image, new_marble->center, 2, cv::Scalar(0,255,0), -1, 8, 0 );
             //circle outline
            cv::circle( temp_image, new_marble->center, new_marble->radius, cv::Scalar(0,0,255), 1, 8, 0 );
        }
      //  cv::imshow( "Contour demo", temp_image ); // Must have the same mutex lock
    }
}

/*
void c_vision::hough_transform(cv::Mat img)
{
    if (image_updated)
    {
        cv::Mat gray;
        cvtColor(img, gray, COLOR_BGR2GRAY);
        GaussianBlur(gray, gray, Size(9, 9), 2, 2);
        vector<cv::Vec3f> circles;
        HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 2, 20, 300, 20, 5, 15);

        for(int i = 0; i < circles.size(); i++ )
        {
            cv::Vec3i c = circles[i];
            cv::Point center = cv::Point(c[0], c[1]);
            // circle center
            cv::circle(img, center, 1, Scalar(0,100,100), 3, LINE_AA);
            // circle outline
            int radius = c[2];
            cv::circle(img, center, radius, Scalar(255,0,255), 3, LINE_AA);
        }
    }
}
*/

std::vector<ct::marble*> c_vision::getMarbles()
{
    return marbles;
}

