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

    //ori_image = image.clone();
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
    //int canny_min = 5;
    //int canny_max = 15;

    if (image_updated)
    {
        find_color();

        cv::imshow("marbles", processed_image);

        int image_w = processed_image.cols; // cols
        int image_h = processed_image.rows; // rows
        cv::Mat temp_image(image_h,image_w,CV_8UC3,cv::Scalar(0,0,0));

        /*std::vector<cv::Vec3f> circles;
        std::vector<cv::Vec3f> final_circles;
        cv::HoughCircles( processed_image, circles, CV_HOUGH_GRADIENT, 1, 20, 75, canny_threshold, 0, 0 );

        for (unsigned int i = 0; i < circles.size(); i++)
        {
            int pix_val = *processed_image.ptr(cvRound(circles[i][1]), cvRound(circles[i][0]));
            if (pix_val == 255)
                final_circles.push_back({circles[i][0],circles[i][1],circles[i][2]});
        }

        int detected_area = 0;
        for (unsigned int i = 0; i < final_circles.size(); i++)
            detected_area += M_PI * pow(final_circles[i][2],2);

        if (detected_area > number_of_white_pixels)
            canny_threshold++;
        else if (detected_area < number_of_white_pixels)
            canny_threshold--;

        if (canny_threshold < canny_min)
            canny_threshold = canny_min;
        else if (canny_threshold > canny_max)
            canny_threshold = canny_max;

        std::cout << "canny threshold: " << canny_threshold << std::endl;

        for (unsigned int i = 0; i < final_circles.size(); i++)
        {
            cv::Point center(cvRound(final_circles[i][0]), cvRound(final_circles[i][1]));
            int radius = cvRound(final_circles[i][2]);
            // circle center
            cv::circle( temp_image, center, 2, cv::Scalar(0,255,0), -1, 8, 0 );
             //circle outline
            cv::circle( temp_image, center, radius, cv::Scalar(0,0,255), 1, 8, 0 );
        }
        cv::imshow( "Hough Circle Transform Demo", temp_image );*/

        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;

        cv::findContours(processed_image,contours,hierarchy,CV_RETR_TREE,CV_CHAIN_APPROX_NONE);

        /*for (unsigned int contour = 0; contour < contours.size(); contour++)
        {
            cv::Scalar color(rand()&0xFF,rand()&0xFF,rand()&0xFF);
            cv::drawContours(temp_image,contours,contour,color,CV_FILLED,8,hierarchy);
        }*/

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
        for (unsigned int i = 0; i < contours.size(); i++)
        {
            circles.push_back({((max_col[i] - min_col[i])/2) + min_col[i],((max_row[i] - min_row[i])/2) + min_row[i],(max_row[i] - min_row[i])/2});

            //cv::line(temp_image, cv::Point(min_col[i],min_row[i]),cv::Point(min_col[i],max_row[i]),cv::Scalar(255,255,255),1,8,0);
            //cv::line(temp_image, cv::Point(min_col[i],max_row[i]),cv::Point(max_col[i],max_row[i]),cv::Scalar(255,255,255),1,8,0);
            //cv::line(temp_image, cv::Point(max_col[i],max_row[i]),cv::Point(max_col[i],min_row[i]),cv::Scalar(255,255,255),1,8,0);
            //cv::line(temp_image, cv::Point(max_col[i],min_row[i]),cv::Point(min_col[i],min_row[i]),cv::Scalar(255,255,255),1,8,0);
        }

        for (unsigned int i = 0; i < circles.size(); i++)
        {
            cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
            int radius = cvRound(circles[i][2]);
            // circle center
            cv::circle( temp_image, center, 2, cv::Scalar(0,255,0), -1, 8, 0 );
             //circle outline
            cv::circle( temp_image, center, radius, cv::Scalar(0,0,255), 1, 8, 0 );
        }
        //std::vector<cv::Point> point = contours[0];
        //point[0].x;
        //std::cout << point[0].x << std::endl;
        //std::cout << "Number of contours: " << contours.size() << std::endl;

        cv::imshow( "Contour demo", temp_image );
    }


}



