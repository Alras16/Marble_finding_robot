#include "map_class.h"

map_class::map_class(std::string fileName, int numb_of_rooms, bool setAsTree)
{
    asTree = setAsTree;
    set_image(fileName);
    image_w = ori_map.cols; // cols
    image_h = ori_map.rows; // rows
    currLocation.x = image_w / 2;
    currLocation.y = image_h / 2;

    init_rooms(numb_of_rooms);
    cv::Vec3i color = {255,255,255};
    for (int i = 0; i < numb_of_rooms; i++)
        paintRoom(listOfRooms[i],color);

}

std::vector<ct::room> map_class::getRooms()
{
    return listOfRooms;
}

void map_class::showMap(std::string title)
{
    cv::Mat temp = ori_map.clone();
    cv::circle(temp,currLocation,6,cv::Scalar({255,255,0}),-1);
    cv::imshow(title,temp);
}

cv::Point map_class::convertToPixel(ct::point aPoint)
{
    int pixelX = round((aPoint.x*image_w)/WORLD_WIDTH) + (image_w / 2);
    int pixelY = round((aPoint.y*image_h)/WORLD_HEIGHT) + (image_h / 2);
    return cv::Point(pixelX,pixelY);
}

ct::point map_class::convertToCoordinate(cv::Point pixel)
{
    ct::point temp;
    temp.x = ((pixel.x*WORLD_WIDTH)/image_w) - (WORLD_WIDTH / 2);
    temp.y = ((pixel.y*WORLD_HEIGHT)/image_h) - (WORLD_HEIGHT / 2);
    return temp;
}

void map_class::setCurrentLocation(cv::Point coordinate)
{
    currLocation = coordinate;
}

cv::Point map_class::getCurrentLocation()
{
    return currLocation;
}

void map_class::set_image(std::string fileName)
{
    QDir path = QDir::current();
    path.cdUp();
    std::string filePath = path.path().toStdString() + "/test_files/maps/" + fileName;
  //  std::cout << filePath;
    ori_map = cv::imread(filePath,cv::IMREAD_COLOR);
}

std::vector<cv::Point> map_class::find_color(int color)
{

    std::vector<cv::Point> points;
    for (int i = 0; i < image_h; i++)
        for (int j = 0; j < image_w; j++)
        {
            cv::Vec3b cur_pixel = *ori_map.ptr<cv::Vec3b>(i,j);
            if ((int(cur_pixel[0]) == color) && (int(cur_pixel[1]) == color) && (int(cur_pixel[2]) == color))
                points.push_back(cv::Point(j,i));

        }
    return points;
}

void map_class::init_rooms(int numb_of_rooms)
{
    int color = 245;
    int roomNumb = 1;

    if (asTree)
    {
        for (int i = 0; i < numb_of_rooms; i++)
        {
            ct::room  tempRoom;
            tempRoom.roomNumber = roomNumb++;
            tempRoom.probabilityOfMarbles = 0.0;
            std::vector<cv::Point> temp = find_color(color);
            tempRoom.numbOfPixels = temp.size();
            tempRoom.coordinates = temp;
            tempRoom.coordinatesTree.generateCompleteTree(temp);
            listOfRooms.push_back(tempRoom);
            color -= 10;
        }
    }
    else
    {
        for (int i = 0; i < numb_of_rooms; i++)
        {
            ct::room tempRoom; // = new ct::room;
            tempRoom.roomNumber = roomNumb++;
            tempRoom.probabilityOfMarbles = 0.0;
            std::vector<cv::Point> temp = find_color(color);
            tempRoom.numbOfPixels = temp.size();
            tempRoom.coordinates = temp;
            ct::room  tempRoom1;
            tempRoom1.roomNumber = roomNumb++;
            tempRoom1.probabilityOfMarbles = 0.0;
            tempRoom1.coordinates = find_color(color);
            listOfRooms.push_back(tempRoom);
            color -= 10;
        }
    }
}

void map_class::paintRoom(ct::room theRoom, cv::Vec3i color)
{
    std::vector<cv::Point> temp;
    if (asTree)
        temp = theRoom.coordinatesTree.convertToList();
    else
        temp = theRoom.coordinates;

    unsigned int numbOfPoints = temp.size();
    for (unsigned int i = 0; i < numbOfPoints; i++)
    {
        cv::Point point = temp[i];
        *ori_map.ptr<cv::Vec3b>(point.y,point.x) = color;
    }
}

void map_class::paintMarble(ct::foundMarble marble)
{
    cv::circle(ori_map,marble.fMarble.center,5,cv::Scalar({255,0,0}),-1);
}

void map_class::find_center_of_mass()
{
    // Original png picture 120*80 and tmp.eps paper size 85 × 56 mm.
    // x = (120/2)/(85/2) = 1.411.. so the invers is x/1.411764706
    // y = (80/2)/(56/2) = 1.428.. so the invers is y/1.428571429

    int center_of_mass_x = 0, center_of_mass_y = 0;
    for (unsigned int i = 0; i < listOfRooms.size(); i++)
    {
        for(unsigned int j = 0; j < listOfRooms[i].coordinates.size() ; j++)
        {
            center_of_mass_x += listOfRooms[i].coordinates[j].x;
            center_of_mass_y += listOfRooms[i].coordinates[j].y;
        }
        center_of_mass_x /= listOfRooms[i].coordinates.size();
        center_of_mass_y /= listOfRooms[i].coordinates.size();

        cv::Point point;
        point.x = (center_of_mass_x-60)/1.411764706; // To find the coordinate from pixel of picture;
        point.y = ( (-1)*(center_of_mass_y-40) )/1.428571429;
        listOfRooms[i].centerOfMass = point;
        listOfRooms[i].centerOfMassPic.x = center_of_mass_x*5;
        listOfRooms[i].centerOfMassPic.y = center_of_mass_y*5;
    }
}

std::vector<std::vector<float>> map_class::genStateMap(float freeSpaceVal, float wallVal)
{
    std::vector<std::vector<float>> stateMatrix;
    for (int i = 0; i < image_h; i++)
    {
        std::vector<float> temp;
        for (int j = 0; j < image_w; j++)
        {
            cv::Vec3b cur_pixel = *ori_map.ptr<cv::Vec3b>(i,j);
            if ((int(cur_pixel[0]) == 0) && (int(cur_pixel[1]) == 0) && (int(cur_pixel[2]) == 0))
                temp.push_back(wallVal);
            else
                temp.push_back(freeSpaceVal);
        }
        stateMatrix.push_back(temp);
    }
    return stateMatrix;
}
