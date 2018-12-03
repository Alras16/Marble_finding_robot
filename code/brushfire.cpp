#include "brushfire.h"

brushfire::brushfire(std::string fileName)
{
    QDir path = QDir::current();
    path.cdUp();
    std::string filePath = path.path().toStdString() + "/test_files/maps/" + fileName;
    ori_map = cv::imread(filePath,cv::IMREAD_COLOR);
    point_map = cv::imread(filePath,cv::IMREAD_COLOR);

    for(int i = 0; i < ori_map.rows; i++)
    {
        std::vector<float> temp;

        for(int j = 0; j < ori_map.cols; j++)
        {
            cv::Vec3b cur_pixel = *ori_map.ptr<cv::Vec3b>(i,j);
            if ((cur_pixel[0] == 0) && (cur_pixel[1] == 0) && (cur_pixel[2] == 0))
                temp.push_back(1.0);
            else
                temp.push_back(0.0);
        }
        matrix.push_back(temp);
    }
}

void brushfire::showImage(std::string name)
{
    cv::imshow(name, ori_map);
}


void brushfire::showValues(std::string name)
{
    cv::imshow(name, point_map);
}


void brushfire::paintMap()
{
    for (int i = 0; i < ori_map.rows; i++)
    {
        for(int j = 0; j < ori_map.cols; j++)
        {
            if (matrix[i][j] == 1.0)
                *ori_map.ptr<cv::Vec3b>(i,j) = {0, 0, 0};
            else if (matrix[i][j] == 0.0)
                *ori_map.ptr<cv::Vec3b>(i,j) = {255, 255, 255};
            else
            {
                if((matrix[i][j] - 2)/(maxValue - 2) < 0.5) // red to yellow
                {
                    int green = (matrix[i][j] - 2)/(maxValue - 2) * (255 / 0.5);
                    *ori_map.ptr<cv::Vec3b>(i,j) = {0, green, 255};
                }
                else // yellow to green
                {
                    int red = (1 - (matrix[i][j] - 2)/(maxValue - 2)) * (255 / 0.5);
                    *ori_map.ptr<cv::Vec3b>(i,j) = {0, 255, red};
                }
            }
        }
    }
}

void brushfire::scaleImage(int factor)
{
    int newWidth = ori_map.cols * factor;
    int newHeight = ori_map.rows * factor;

    cv::Mat tempImageOne = cv::Mat(newHeight,newWidth,CV_8UC3);
    tempImageOne.setTo(0);

    cv::Mat tempImageTwo = cv::Mat(newHeight,newWidth,CV_8UC3);
    tempImageTwo.setTo(0);

    for (int row = 0; row < ori_map.rows; row++)
    {
        for (int col = 0; col < ori_map.cols; col++)
        {
            cv::Vec3b colorOne = *ori_map.ptr<cv::Vec3b>(row,col);
            cv::Vec3b colorTwo = *point_map.ptr<cv::Vec3b>(row,col);
            for (int newRow = row * factor; newRow < row * factor + factor; newRow++)
            {
                for (int newCol = col * factor; newCol < col * factor + factor; newCol++)
                {
                    *tempImageOne.ptr<cv::Vec3b>(newRow,newCol) = colorOne;
                    *tempImageTwo.ptr<cv::Vec3b>(newRow,newCol) = colorTwo;
                }
            }
        }
    }
    ori_map = tempImageOne.clone();
    point_map = tempImageTwo.clone();
}

void brushfire::brushfireAlgorithm(int iterations)
{
    float label = 1.0;
    for (int h = 0; h < iterations; h++)
    {
        label++;
        //std::cout << matrix.size() << std::endl;
        //std::cout << matrix[0].size() << std::endl;
        for(unsigned int i = 1; i < matrix.size() - 1; i++)
        {
            for(unsigned int j = 1; j < matrix[i].size() - 1; j++)
            {
                if (matrix[i][j] == 0)
                {
                    int neighbor[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                    neighbor[0] = matrix[i - 1][j - 1];
                    neighbor[1] = matrix[i][j - 1];
                    neighbor[2] = matrix[i + 1][j - 1];
                    neighbor[3] = matrix[i + 1][j];
                    neighbor[4] = matrix[i + 1][j + 1];
                    neighbor[5] = matrix[i][j + 1];
                    neighbor[6] = matrix[i - 1][j + 1];
                    neighbor[7] = matrix[i - 1][j];

                    for (int k = 0; k < 8; k++)
                    {
                        if (neighbor[k] == label - 1.0)
                            matrix[i][j] = label;
                    }
                }
            }
        }
    }
    maxValue = label;
}

void brushfire::findMedianPoints(int threshold)
{
    for (int i = 0; i < matrix.size(); i++)
    {
        for (int j = 0; j < matrix[i].size(); j++)
        {
            if (matrix[i][j] >= threshold)
            {
                cv::Point temp;
                temp.x = j;
                temp.y = i;
                imagePoints.push_back(temp);
            }
        }
    }
}


void brushfire::plotMedianPoints()
{ 
    for (int i = 0; i < imagePoints.size(); i++)
        *point_map.ptr<cv::Vec3b>(imagePoints[i].y,imagePoints[i].x) = cv::Vec3b({255, 0, 0});
}

void brushfire::findCornerPoints()
{
    for(unsigned int i = 1; i < matrix.size() - 1; i++)
    {
        for(unsigned int j = 1; j < matrix[i].size() - 1; j++)
        {
            if (matrix[i][j] != 1)
            {
                int neighbor[4] = {0, 0, 0, 0};
                neighbor[0] = matrix[i - 1][j];
                neighbor[1] = matrix[i][j + 1];
                neighbor[2] = matrix[i + 1][j];
                neighbor[3] = matrix[i][j - 1];

                for (int k = 0; k < 4; k++)
                {
                    if (k == 3)
                    {
                        if (matrix[i][j] == neighbor[0] && matrix[i][j] == neighbor[k])
                            cornerPoints.push_back(cv::Point(j,i));
                    }
                    else
                    {
                        if (matrix[i][j] == neighbor[k] && matrix[i][j] == neighbor[k + 1])
                            cornerPoints.push_back(cv::Point(j,i));
                    }
                }
            }
        }
    }

    for (unsigned int i = 0; i < cornerPoints.size(); i++)
        *point_map.ptr<cv::Vec3b>(cornerPoints[i].y,cornerPoints[i].x) = cv::Vec3b({0, 0, 255});
}


brushfire::~brushfire()
{

}
