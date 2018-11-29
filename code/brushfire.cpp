#include "brushfire.h"

brushfire::brushfire(std::string fileName)
{
    QDir path = QDir::current();
    path.cdUp();
    std::string filePath = path.path().toStdString() + "/test_files/maps/" + fileName;
    ori_map = cv::imread(filePath,cv::IMREAD_COLOR);

    for(int i = 0; i < ori_map.rows; i++)
    {
        std::vector<int> temp;

        for(int j = 0; j < ori_map.cols; j++)
        {
            cv::Vec3b cur_pixel = *ori_map.ptr<cv::Vec3b>(i,j);
            if ((cur_pixel[0] == 0) && (cur_pixel[1] == 0) && (cur_pixel[2] == 0))
                temp.push_back(1);
            else
                temp.push_back(0);
        }
        matrix.push_back(temp);
    }
}

void brushfire::showImage(std::string name)
{
    cv::imshow(name, ori_map);
}

void brushfire::paintMap()
{
    for (int i = 0; i < ori_map.rows; i++)
    {
        for(int j = 0; j < ori_map.cols; j++)
        {
            if (matrix[i][j] == 1)
                *ori_map.ptr<cv::Vec3b>(i,j) = {0, 0, 0};
            else if (matrix[i][j] == 0)
                *ori_map.ptr<cv::Vec3b>(i,j) = {255, 255, 255};
            else
            {
                if(matrix[i][j] < ((maxValue - 1) / 2)) // red to yellow
                {
                    int green = matrix[i][j] * (255 / ((maxValue - 1) / 2));
                    *ori_map.ptr<cv::Vec3b>(i,j) = {0, green, 255};
                }
                else // yellow to green
                {
                    int red = (1 - matrix[i][j]) * (255 / ((maxValue - 1) / 2));
                    *ori_map.ptr<cv::Vec3b>(i,j) = {0, 255, red};
                }
            }
        }
    }
}

void brushfire::brushfireAlgorithm()
{
    int label = 1;
    for (int h = 0; h < 14; h++)
    {
        label++;
        std::cout << matrix.size() << std::endl;
        std::cout << matrix[0].size() << std::endl;
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
                        if (neighbor[k] == label - 1)
                            matrix[i][j] = label;
                    }
                }
            }
        }
        std::cout << "method terminated" << std::endl;
    }
    maxValue = label;
}

brushfire::~brushfire()
{

}
