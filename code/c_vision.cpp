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
    //resize(image,ori_image,cv::Size(image.cols / 2, image.rows / 2));
    ori_image = image.clone();
    cv::imshow("original image",ori_image);
    image_updated = true;
}

cv::Mat c_vision::hls_histogram()
{
    //Convert to HLS
    cv::Mat hls_image;
    cv::cvtColor(ori_image, hls_image, CV_BGR2HLS);

    // Generate histogram
    std::vector<int> histogram;
    for (int i = 0; i < 181; i++)
        histogram.push_back(0);

    for (int i = 0; i < hls_image.rows; i++)
        for (int j = 0; j < hls_image.cols; j++)
        {
            cv::Vec3b cur_pixel = *hls_image.ptr<cv::Vec3b>(i,j);
            int H = cur_pixel[0];
            histogram[H]++;
        }

    // Make image for histogram
    int hist_w = 512; // cols
    int hist_h = 400; // rows
    cv::Mat histogram_image(hist_h,hist_w,CV_8UC3,cv::Scalar(0,0,0));
    histogram_image.setTo(cv::Scalar({255,255,255}));

    // Normalize histogram to fit image
    int max = 0;
    for (unsigned int i = 0; i < histogram.size(); i++)
        if (histogram[i] > max)
            max = histogram[i];

    for (unsigned int i = 0; i < histogram.size(); i++)
        histogram[i] = (histogram[i] * (hist_h - 1)) /max;

    // Find top points
    unsigned int numb = 0;
    int offset = 50;
    for (unsigned int i = 0; i < histogram.size() - 1; i++)
    {
        if (i == 0)
        {
            if (histogram[i] > histogram[i+1])
            {
                numb = i;
                std::string text = "Max value at: " + std::to_string(numb);
                cv::putText(histogram_image,text,cv::Point(250,offset), cv::FONT_HERSHEY_COMPLEX_SMALL,1.0,cv::Scalar(0,0,0),1,CV_AA);
                offset+=25;
            }
        }
        else
        {
            if ((histogram[i-1] < histogram[i]) && (histogram[i] > histogram[i+1]))
            {
                numb = i;
                std::string text = "Max value at: " + std::to_string(numb);
                cv::putText(histogram_image,text,cv::Point(250,offset), cv::FONT_HERSHEY_COMPLEX_SMALL,1.0,cv::Scalar(0,0,0),1,CV_AA);
                offset+=25;
            }
        }
    }

    int width = 2;
    // Make plot
    int bin_w = cvRound((double)hist_w / 180);
    for (unsigned int i = 1; i < histogram.size(); i++)
    {
        if (numb == i)
        {
            cv::line(histogram_image, cv::Point(bin_w*(i-width),0),cv::Point(bin_w*(i-width),(hist_h - 1)),cv::Scalar(255,255,0),1,8,0);
            cv::line(histogram_image, cv::Point(bin_w*(i+width),0),cv::Point(bin_w*(i+width),(hist_h - 1)),cv::Scalar(255,255,0),1,8,0);
        }
        cv::line(histogram_image, cv::Point(bin_w*(i-1),hist_h - cvRound(histogram[i-1])),cv::Point(bin_w*(i),hist_h - cvRound(histogram[i])),cv::Scalar(0,0,0),2,8,0);
    }
    cv::imshow("Histogram",histogram_image);
    return histogram_image;
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
        //image_updated = false;
        cv::imshow("Color found",binary_image);
    }


}

void c_vision::find_marbles()
{
    if (image_updated)
    {
        find_color();
        marbles.clear();
        int image_w = processed_image.cols; // cols
        int image_h = processed_image.rows; // rows
        cv::Mat temp_image = ori_image.clone();

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
            min_col.push_back(image_w - 1);
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

        std::vector<cv::Mat> small_images;
        for (unsigned int contour = 0; contour < contours.size(); contour++)
        {
            int w = max_col[contour] - min_col[contour]; // cols
            int h = max_row[contour] - min_row[contour]; // rows
            cv::Mat temp_image(h,w,CV_8UC1,cv::Scalar(0));

            std::vector<std::vector<cv::Point>> new_contours;
            std::vector<cv::Point> temp;
            for (unsigned int i = 0; i < contours[contour].size(); i++)
                temp.push_back(cv::Point(contours[contour][i].x - min_col[contour],contours[contour][i].y - min_row[contour]));
            new_contours.push_back(temp);
            cv::drawContours(temp_image, new_contours, 0, 255, CV_FILLED);
            small_images.push_back(temp_image);
        }

        for (unsigned int i = 0; i < small_images.size(); i++)
        {
            std::string name = "contour " + std::to_string(i);
            cv::imshow(name,small_images[i]);
        }


       // std::cout << "number of contours: " << contours.size() << std::endl;

        ct::marble new_marble;

        for (unsigned int i = 0; i < contours.size(); i++)
        {
            new_marble.center.x = ((max_col[i] - min_col[i])/2) + min_col[i];
            new_marble.center.y = ((max_row[i] - min_row[i])/2) + min_row[i];
            new_marble.distance_to_center = ((max_col[i] - min_col[i])/2) + min_col[i] - ori_image.cols / 2;
            new_marble.radius = (max_row[i] - min_row[i])/2;
            if ( (new_marble.center.x && new_marble.center.y) != 0)
              new_marble.centerPolar.theta = std::atan(new_marble.center.y/new_marble.center.x);
             marbles.push_back(new_marble);
        }

        for (unsigned int marble = 0; marble < marbles.size(); marble++)
        {
            int x_diff = max_col[marble] - min_col[marble];
            int y_diff = max_row[marble] - min_row[marble];
            if (x_diff <= y_diff)
            {
                if (marbles[marble].center.x < image_w / 2) // left half
                    marbles[marble].center.x -= marbles[marble].center.x + marbles[marble].radius - max_col[marble];
                else // right half
                    marbles[marble].center.x += min_col[marble] - marbles[marble].center.x + marbles[marble].radius;
            }
        }

        //circle outline
        for (unsigned int i = 0; i < marbles.size(); i++)
            cv::circle( temp_image, marbles[i].center, marbles[i].radius, cv::Scalar(0,0,255), 2, 8, 0 );
        ori_image = temp_image;
        cv::imshow( "Contour demo", temp_image ); // Must have the same mutex lock
    }
}

cv::Mat c_vision::getImage()
{
    return ori_image;
}

ct::marble c_vision::find_closest_marble()
{

    ct::marble closest_marble;

    if (marbles.size() != 0)
    {

        closest_marble = marbles[0];
        for (unsigned i = 0; i < marbles.size(); i++)
            if (closest_marble.centerPolar.theta > marbles[i].centerPolar.theta)
                closest_marble = marbles[i];
    }
    else
        closest_marble.radius = 0;

    return closest_marble;
}

std::vector<ct::marble> c_vision::getMarbles()
{
    return marbles;
}
