#include "brushfire.h"

brushfire::brushfire(std::string fileName)
{
    QDir path = QDir::current();
    path.cdUp();
    std::string filePath = path.path().toStdString() + "/test_files/maps/" + fileName;
    ori_map = cv::imread(filePath,cv::IMREAD_COLOR);
    point_map = cv::imread(filePath,cv::IMREAD_COLOR);
    connected_map = cv::imread(filePath,cv::IMREAD_COLOR);
    path_map = cv::imread(filePath,cv::IMREAD_COLOR);

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

   // cv::imwrite("/media/kenni/usb1/linuxUbuntu/Robotics5semesterProject/Tests/RoadMap/Brushfire1.png", ori_map);
    cv::imshow(name, ori_map);
}


void brushfire::showValues(std::string name)
{
   // cv::imwrite("/media/kenni/usb1/linuxUbuntu/Robotics5semesterProject/Tests/RoadMap/RoadPoints1.png", point_map);
   cv::imshow(name, point_map);
}

void brushfire::showConnectedPoints(std::string name)
{
  //  cv::imwrite("/media/kenni/usb1/linuxUbuntu/Robotics5semesterProject/Tests/RoadMap/RoadMap1.png", connected_map);
    cv::imshow(name, connected_map);
}

void brushfire::showPath(std::string name)
{
    cv::imshow(name, path_map);
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

    cv::Mat tempImageThree = cv::Mat(newHeight,newWidth,CV_8UC3);
    tempImageThree.setTo(0);

    cv::Mat tempImageFour = cv::Mat(newHeight,newWidth,CV_8UC3);
    tempImageFour.setTo(0);

    for (int row = 0; row < ori_map.rows; row++)
    {
        for (int col = 0; col < ori_map.cols; col++)
        {
            cv::Vec3b colorOne = *ori_map.ptr<cv::Vec3b>(row,col);
            cv::Vec3b colorTwo = *point_map.ptr<cv::Vec3b>(row,col);
            cv::Vec3b colorThree = *connected_map.ptr<cv::Vec3b>(row,col);
            cv::Vec3b colorFour = *connected_map.ptr<cv::Vec3b>(row,col);

            for (int newRow = row * factor; newRow < row * factor + factor; newRow++)
            {
                for (int newCol = col * factor; newCol < col * factor + factor; newCol++)
                {
                    *tempImageOne.ptr<cv::Vec3b>(newRow,newCol) = colorOne;
                    *tempImageTwo.ptr<cv::Vec3b>(newRow,newCol) = colorTwo;
                    *tempImageThree.ptr<cv::Vec3b>(newRow,newCol) = colorThree;
                    *tempImageFour.ptr<cv::Vec3b>(newRow,newCol) = colorFour;
                }
            }
        }
    }

    ori_map = tempImageOne.clone();
    point_map = tempImageTwo.clone();
    connected_map = tempImageThree.clone();
    path_map = tempImageFour.clone();

}

void brushfire::brushfireAlgorithm(int iterations)
{
    float label = 1.0;
    for (int h = 0; h < iterations; h++)
    {
        label++;
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

void brushfire::findCenterPoints()
{
    for(unsigned int i = 1; i < matrix.size() - 1; i++)
    {
        for(unsigned int j = 1; j < matrix[i].size() - 1; j++)
        {
            if (matrix[i][j] != 1)
            {
                int neighbor[4] = {0, 0, 0, 0};
                neighbor[0] = matrix[i][j - 1];
                neighbor[1] = matrix[i + 1][j];
                neighbor[2] = matrix[i][j + 1];
                neighbor[3] = matrix[i - 1][j];

                if (matrix[i][j] == neighbor[0] && matrix[i][j] == neighbor[2] && matrix[i][j] == (neighbor[1] - 1) && matrix[i][j] == (neighbor[3] - 1))
                   centerPoints.push_back(cv::Point(j,i));
                else if (matrix[i][j] == neighbor[1] && matrix[i][j] == neighbor[3] && matrix[i][j] == (neighbor[0] - 1) && matrix[i][j] == (neighbor[2] - 1))
                   centerPoints.push_back(cv::Point(j,i));

            }
        }
    }

    for (unsigned int i = 0; i < centerPoints.size(); i++)
    {
        *point_map.ptr<cv::Vec3b>(centerPoints[i].y,centerPoints[i].x) = cv::Vec3b({0, 0, 255});
        std::cout << centerPoints.size() << std::endl;
    }
}

void brushfire::findCornerPoints()
{
    for(unsigned int i = 2; i < matrix.size() - 1; i++)
    {
        for(unsigned int j = 2; j < matrix[i].size() - 1; j++)
        {
            if (matrix[i][j] != 1)
            {
                int neighbor[4] = {0, 0, 0, 0};
                neighbor[0] = matrix[i - 1][j];
                neighbor[1] = matrix[i][j + 1];
                neighbor[2] = matrix[i + 1][j];
                neighbor[3] = matrix[i][j - 1];

                 // Detect corners
                 if (matrix[i][j] == neighbor[0] && matrix[i][j] == neighbor[1] && matrix[i][j] != matrix[i-1][j+1] )
                        cornerPoints.push_back(cv::Point(j,i));

                 else if (matrix[i][j] == neighbor[1] && matrix[i][j] == neighbor[2] && matrix[i][j] != matrix[i+1][j+1])
                        cornerPoints.push_back(cv::Point(j,i));

                 else if (matrix[i][j] == neighbor[2] && matrix[i][j] == neighbor[3] && matrix[i][j] != matrix[i+1][j-1])
                        cornerPoints.push_back(cv::Point(j,i));

                 else if (matrix[i][j] == neighbor[3] && matrix[i][j] == neighbor[0] && matrix[i][j] != matrix[i-1][j-1])
                        cornerPoints.push_back(cv::Point(j,i));
            }
        }
    }

    for (unsigned int i = 0; i < cornerPoints.size(); i++)
        *point_map.ptr<cv::Vec3b>(cornerPoints[i].y,cornerPoints[i].x) = cv::Vec3b({0, 0, 255});
}

void brushfire::findCenterPoints()
{

    for(unsigned int i = 2; i < matrix.size() - 1; i++)
    {
        for(unsigned int j = 2; j < matrix[i].size() - 1; j++)
        {
            if (matrix[i][j] != 1)
            {
                int neighbor[4] = {0, 0, 0, 0};
                neighbor[0] = matrix[i - 1][j];
                neighbor[1] = matrix[i][j + 1];
                neighbor[2] = matrix[i + 1][j];
                neighbor[3] = matrix[i][j - 1];


                // Detect center points
                if (matrix[i][j] != neighbor[0] && matrix[i][j] != neighbor[2] && neighbor[0] == neighbor[2])
                       centerPoints.push_back(cv::Point(j,i));

                else if (matrix[i][j] != neighbor[1] && matrix[i][j] != neighbor[3] && neighbor[1] == neighbor[3])
                       centerPoints.push_back(cv::Point(j,i));

                else if (matrix[i][j] == matrix[i+1][j] && matrix[i-1][j] == matrix[i+2][j] && matrix[i][j] != matrix[i-1][j])
                {
                       centerPoints.push_back(cv::Point(j,i));
                       centerPoints.push_back(cv::Point(j,i+1));
                }
                else if (matrix[i][j] == matrix[i][j+1] && matrix[i][j-1] == matrix[i][j+2] && matrix[i][j] != matrix[i][j-1])
                {
                       centerPoints.push_back(cv::Point(j,i));
                       centerPoints.push_back(cv::Point(j+1,i));
                }
            }
        }
    }

    for (unsigned int i = 0; i < centerPoints.size(); i++)
        *point_map.ptr<cv::Vec3b>(centerPoints[i].y,centerPoints[i].x) = cv::Vec3b({0, 0, 255});
}

void brushfire::findLinePoints()
{

    std::vector<std::vector<float>> matrix1 = matrix;
    std::vector<cv::Point> lines;

    for(unsigned int i = 0; i < centerPoints.size(); i++)
     {
       matrix1[centerPoints[i].y][centerPoints[i].x] = 20;

     }


   // Horisontal
   for(unsigned int i = 1; i < matrix1.size() - 1; i++)
       {

       for(unsigned int j = 1; j < matrix1[i].size() - 1; j++)
         {

                 while (matrix1[i][j] == 20)
                 {
                   lines.push_back(cv::Point(j,i));
                   j++;
                 }

                 if (lines.size() > 2)
                 {
                   intersectionPoins.push_back( lines[lines.size()/2]);
                   lines.clear();
                   j--;
                 }
                 else
                 {
                   lines.clear();
                 }

        }

     }

   // Vertical
   for(unsigned int j = 1; j < matrix1[0].size() - 1; j++)
       {

       for(unsigned int i = 1; i < matrix1.size() - 1; i++)
         {

                 while (matrix1[i][j] == 20)
                 {
                   lines.push_back(cv::Point(j,i));
                   i++;
                 }

                 if (lines.size() > 2)
                 {
                   intersectionPoins.push_back( lines[lines.size()/2]);
                   lines.clear();
                   i--;
                 }
                 else
                 {
                   lines.clear();
                 }

        }

     }

   // Remove points closer than 3 from another
   for (unsigned int i = 0; i < intersectionPoins.size(); i++)
   {
      for (unsigned int j = 0; j < intersectionPoins.size(); j++)
      {
         if (i != j)
         {
            if ( sqrt( pow( (intersectionPoins[i].x - intersectionPoins[j].x),2) + pow(intersectionPoins[i].y - intersectionPoins[j].y,2)) < 6)
                     intersectionPoins.erase(intersectionPoins.begin() + i);
         }
      }
   }

    for (unsigned int i = 0; i < intersectionPoins.size(); i++)
        *point_map.ptr<cv::Vec3b>(intersectionPoins[i].y,intersectionPoins[i].x) = cv::Vec3b({255, 0, 0});

}


void brushfire::connectPoints()
{
    cv::Point start, end;

    bool obstacle_on_line = false;

    std::vector<cv::Point> buf;

    // Find all edges that connects two points without hitting an obstacle
    for (unsigned int i = 0; i < intersectionPoins.size(); i++)
    {
       for (unsigned int j = 0; j < intersectionPoins.size(); j++)
       {

          if (i != j)
          {

              start = intersectionPoins[i];
              end = intersectionPoins[j];

              cv::LineIterator pic(ori_map, start, end, 8);

              for(int p = 0; p < pic.count; p++)
              {
                  buf.push_back( pic.pos() );
                  pic++;
              }

              for (int k = 0; k < buf.size(); k++)
              {

                  int neighbor[4] = {0, 0, 0, 0};

                  neighbor[0] = matrix[buf[k].y - 1][buf[k].x];
                  neighbor[1] = matrix[buf[k].y][buf[k].x + 1];
                  neighbor[2] = matrix[buf[k].y + 1][buf[k].x];
                  neighbor[3] = matrix[buf[k].y][buf[k].x - 1];


                  if (matrix[buf[k].y][buf[k].x] == 1)
                  {
                      obstacle_on_line = true;
                  }

                  for (int o = 0; o < 4; o++)
                  {
                      if (neighbor[o] == 1)
                      {
                          obstacle_on_line = true;
                      }

                  }

              }

              if ( !obstacle_on_line )
              {
                  ct::edge line;
                  line.start = start;
                  line.v1 = i;
                  line.end = end;
                  line.v2 = j;
                  line.distance = sqrt( pow( (start.x - end.x),2) + pow( (start.y - end.y),2));
                  edges.push_back(line);
              }

              obstacle_on_line = false;
              buf.clear();

          }

       }

    }

    // Remove dublicates
    for (unsigned int i = 0; i < edges.size(); i++)
    {
        for (unsigned int j = 0; j < edges.size(); j++)
        {
            if (edges[i].v1 == edges[j].v2 && edges[i].v2 == edges[j].v1)
                    edges.erase(edges.begin() + i);
        }

    }

    // Sort edges by distance
    int numVertices = intersectionPoins.size();
    int numEdges = edges.size();
    ct::edge buffer;

    for (int i = 1; i < numEdges; i++)
    {
        buffer = std::move(edges[i]);
        int j;

        for (j = i; j > 0 && buffer.distance < edges[j - 1].distance; --j)
            edges[j] = std::move(edges[j-1]);

        edges[j]= std::move(buffer);
    }

    // Start Kruskal's algorithm
    for (int i = 0; i < numVertices; i++)
        s.push_back(-1);

    std::vector<ct::edge> mst;

    for (int k = 0; k < edges.size(); k++)
    {
        ct::edge e = edges[k];

        int V1 = find(e.v1);
        int V2 = find(e.v2);

       if (V1 != V2)
        {
            // Accept the edge
            mst.push_back(e);
            unionSets(V1,V2);

        }

    }

    edges = mst;

    for (unsigned int i = 0; i < mst.size(); i++)
         cv::line(connected_map, mst[i].start*5, mst[i].end*5, cv::Scalar({0,0,255}), 2, 1, 0);

    for (unsigned int i = 0; i < intersectionPoins.size(); i++)
         cv::circle(connected_map,intersectionPoins[i]*5,3,cv::Scalar({255,0,0}),-1);

}

void brushfire::findPathPoints(cv::Point curPos, cv::Point goal)
{

  // Scaling from gazebo coordinates to picture pixels
  curPos.x = (curPos.x*1.411764706)+60; // (120/2)/(85/2) = 1.411..
  curPos.y = ((-1)*curPos.y*1.428571429)+40; // (80/2)/(56/2) = 1.428..

  goal.x =(goal.x*1.411764706)+60; // (120/2)/(85/2) = 1.411..
  goal.y = ((-1)*goal.y*1.428571429)+40; // (80/2)/(56/2) = 1.428..

  cv::Point start, end;
  float closest_point_start = 1000, closest_point_end = 1000;
  pathPointsResult.clear();
  pathPoints.clear();
  connectedPoints.clear();

  for (int i = 0; i < intersectionPoins.size(); i++)
  {
      if (sqrt( pow( (intersectionPoins[i].y - curPos.y),2) +  pow( (intersectionPoins[i].x - curPos.x),2) ) < closest_point_start )
      {
          closest_point_start = sqrt( pow( (intersectionPoins[i].y - curPos.y),2) +  pow( (intersectionPoins[i].x - curPos.x),2) );
          start = intersectionPoins[i];
      }

      if (sqrt( pow( (intersectionPoins[i].y - goal.y),2) +  pow( (intersectionPoins[i].x - goal.x),2) ) < closest_point_end )
      {
          closest_point_end = sqrt( pow( (intersectionPoins[i].y - goal.y),2) +  pow( (intersectionPoins[i].x - goal.x),2) );
          end = intersectionPoins[i];
      }
  }


  std::vector<cv::Point> adjacent;

  for (int i = 0; i < intersectionPoins.size(); i++)
  {

      for (int j = 0; j < edges.size(); j++)
      {
             if (intersectionPoins[i] == edges[j].start)
                 adjacent.push_back(edges[j].end);
             else if (intersectionPoins[i] == edges[j].end)
                  adjacent.push_back(edges[j].start);

      }

      connectedPoints.push_back(adjacent);
      adjacent.clear();

  }

/*
  for (int i = 0; i < connectedPoints.size(); i++)
  {
      std::cout <<"Point " << i + 1 << ": " << intersectionPoins[i] << " is connected to: " <<std::endl;
      for (int j = 0; j < connectedPoints[i].size(); j++)
      {
          std::cout << connectedPoints[i][j];

      }
    std::cout << std::endl;
  }

*/

  DFS(start,end);

  // for (int i = 0; i < pathPointsResult.size(); i++)
  //    std::cout << std::endl << "Point" << i+1 << ": " << pathPointsResult[i] << std::endl;

  for (unsigned int i = 0; i < pathPointsResult.size(); i++)
       cv::circle(connected_map,pathPointsResult[i]*5,4,cv::Scalar({0,0,0}),-1);

  // cv::imwrite("/media/kenni/usb1/linuxUbuntu/Robotics5semesterProject/Tests/KruskalsAndDFSTest/PathTest3.png", connected_map);

  // To make the last point the goal
  // pathPointsResult.pop_back();
  // pathPointsResult.push_back(goal);

}


int brushfire::find(int x) const
{
    if (s[x] < 0)
        return x;
    else
        return find(s[x]);
}

void brushfire::unionSets(int root1, int root2)
{
    s[root2] = root1;
}

void brushfire::DFS(cv::Point start, cv::Point goal)
{

    pathPoints.push_back(start);

    bool visited = false;

    int index = 0;

    for (int i = 0; i < intersectionPoins.size(); i++)
    {
        if (start == intersectionPoins[i])
            index = i;
    }

    if (start == goal)
        pathPointsResult = pathPoints;

    for (int i = 0; i < connectedPoints[index].size(); i++)
    {

            if (connectedPoints[index][i] == goal)
            {
                 pathPoints.push_back(connectedPoints[index][i]);
                 pathPointsResult = pathPoints;
            }
            else
            {
                for (int j = 0; j < pathPoints.size(); j++)
                {
                    if (connectedPoints[index][i] == pathPoints[j])
                    {
                        visited = true;
                    }
                }

                if (!visited)
                {
                    DFS(connectedPoints[index][i], goal);
                    pathPoints.pop_back();
                }
            }
            visited = false;
    }
}

std::vector<cv::Point> brushfire::getRoadPath()
{
    cv::Point pointPath;
    std::vector<cv::Point> path;

    //Scaling from picture pixels to gazebo coordinates
    std::cout << "New set of points from position to goal: " << std::endl;
    for (int i = 0; i < pathPointsResult.size(); i++)
    {
        pointPath.x = (pathPointsResult[i].x-60)/1.411764706; // To find the coordinate from pixel of picture;
        pointPath.y = ( (-1)*(pathPointsResult[i].y-40) )/1.428571429;
        path.push_back(pointPath);

        std::cout << "Point" << i + 1 << " " << pointPath.x << "," << pointPath.y << std::endl;
    }

    std::cout << std::endl;

    return path;
}

brushfire::~brushfire()
{

}
