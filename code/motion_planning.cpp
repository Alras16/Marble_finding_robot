#include "motion_planning.h"
#include <fstream>
#include <ctime>


motion_planning::motion_planning()
{
    former_position = cv::Point(0,0);
}


float motion_planning::target_location()
{
    return target_location_reached;
}

ct::robot_orientation motion_planning::get_rotation()
{
    return rotation;
}

std::vector<ct::room> motion_planning::get_rooms()
{
    return room_info;
}

void motion_planning::get_sensor_data(std::vector<ct::polarPoint> sensor_data)
{
    data_ori = sensor_data;
}

void motion_planning::get_room_info(std::vector<ct::room> rooms)
{

    cv::Mat world = cv::imread("/home/kenni/git_workspace/Marble_finding_robot/test_files/maps/map_medium.png",cv::IMREAD_COLOR);

    for (unsigned i = 0; i < rooms.size(); i++)
    {
        room_info.push_back(rooms[i]);
    }


    for (int i = 0; i<room_info.size(); i++)
    {
        std:: cout << room_info[i].centerOfMass << std::endl;
    }


    for (unsigned i = 0; i < room_info.size(); i++)
    {
       cv::circle(world,room_info[i].centerOfMassPic,2,cv::Scalar({0,255,255}),-1);
    }

   cv::imshow("World",world);

}

void motion_planning::show_path_of_robot(cv::Point pos, cv::Point goal, cv::Point nextPoint, bool target_reached)

{
    if (!dummy)
    {
        cv::Point start;
        start.x = path_of_robot.cols/2;
        start.y = path_of_robot.rows/2;
        cv::circle(path_of_robot,start,3,cv::Scalar({0,0,0}),-1);
        cv::circle(path_of_robot,nextPoint,4,cv::Scalar({0,0,0}),-1);
        dummy++;
    }

    cv::circle(path_of_robot,pos,1,cv::Scalar({0,0,255}),-1);
    cv::imshow("Path of robot",path_of_robot);

    if ( target_reached )
    {
       cv::circle(path_of_robot,nextPoint,3,cv::Scalar({255,0,0}),-1);
       // cv::imwrite("/media/kenni/usb1/linuxUbuntu/Robotics5semesterProject/Tests/TangentbugTest/Analysis4.png", path_of_robot);
       dummy++;
    }


}

float motion_planning::homogeneous_transformation(ct::current_position robot, cv::Point goal)
{
    float transform;

    float determinant;
    float T_inv[3][3];

   // Transformation matrix with the robot relative to the world frame
   float T_matrix[3][3] =
    {
      {cos(robot.angle_yaw_radi-M_PI/2),-sin(robot.angle_yaw_radi-M_PI/2),robot.robot_pos.x}, // Minus to indicate world relative to robot
      {sin(robot.angle_yaw_radi-M_PI/2),cos(robot.angle_yaw_radi-M_PI/2),robot.robot_pos.y}, // Minus to indicate world relative to robot
      {0,0,1}
    };

   // Find the determinant of the T_matrix
   for(int i = 0; i < 3; i++)
   {
     determinant = determinant + (T_matrix[0][i] * (T_matrix[1][(i+1)%3] * T_matrix[2][(i+2)%3] - T_matrix[1][(i+2)%3] * T_matrix[2][(i+1)%3]));
   }

   // Find the invers of the T_matrix
   for(int i = 0; i < 3; i++)
   {
         for(int j = 0; j < 3; j++)
         {
             T_inv[i][j] = ((T_matrix[(j+1)%3][(i+1)%3] * T_matrix[(j+2)%3][(i+2)%3]) - (T_matrix[(j+1)%3][(i+2)%3] * T_matrix[(j+2)%3][(i+1)%3]))/determinant;
         }
   }

   // Vector with the point relative to the world frame
   float W_world[3] = {goal.x,goal.y,1};
   float W_robot[3] = {0,0,0};
   // std:: cout << room_info[index].centerOfMass.x << "," << room_info[index].centerOfMass.y << std::endl;

   // Get the shifted point relative to the robot now seen from the world frame
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            W_robot[i] += T_inv[i][j]*W_world[j];
        }
   // std::cout << W_robot[i] << std::endl;
    }

    transform = atan2(W_robot[0], W_robot[1])*(-1);
    //transform.rho = sqrt( pow((goal.x-robot.robot_pos.x),2) + pow((goal.y-robot.robot_pos.y),2));

    return transform;

}

void motion_planning::tangent_bug_algoritm(ct::current_position pos, std::vector<ct::room> Rooms){


    cv::Point obstacle;
    std::vector<ct::polarPoint> data = data_ori;

    float result1, result2 = 1000, boundary_x, boundary_y,dis_robot_to_obstacle, dis_obstacle_to_goal, dis_robot_to_goal;
    float shortest_distance_to_obstacle = 100;
    cv::Point2f goal(35, 16); // = Rooms[index].centerOfMass;

    for (int i = 0; i < data.size(); i++)
    {
        if (data[i].rho < 10 )
        {
             boundary_x = pos.robot_pos.x + data[i].rho*cos(pos.angle_yaw_radi+data[i].theta);
             boundary_y = pos.robot_pos.y + data[i].rho*sin(pos.angle_yaw_radi+data[i].theta);

             dis_obstacle_to_goal = sqrt( pow((boundary_x-goal.x),2)+pow((boundary_y-goal.y),2));
             dis_robot_to_obstacle = sqrt( pow((boundary_x-pos.robot_pos.x),2)+pow((boundary_y-pos.robot_pos.y),2));

             result1 = dis_obstacle_to_goal;

         if (result1 < result2)
         {
              result2 = result1;
              obstacle.x = boundary_x;
              obstacle.y = boundary_y;
         }

         if (data[i].rho < shortest_distance_to_obstacle)
         {
              shortest_distance_to_obstacle = data[i].rho;
         }

        }

    }

    // The laser scanner has radius 10 pixels. We have 72 dots per inch (dpi) and converting it to
    // to millimeter we multiply by 25.4. Thereby getting the result in mm which is converted to m in gazebo

    dist_traveled += sqrt( pow((former_position.x - pos.robot_pos.x),2)+pow((former_position.y - pos.robot_pos.y),2));
    former_position = pos.robot_pos;

    obstacle_range.push_back( (((shortest_distance_to_obstacle*2)/72)*25.4) );
    distance_traveled.push_back( (((dist_traveled*2)/72)*25.4) );


    // Used for tests
/*
    // Calculate running time of robot path
    timeval curTime;
    gettimeofday(&curTime, NULL);
    float milli = curTime.tv_usec / 1000;
    float sec = curTime.tv_usec;
    path_time += sec/1000 + milli/1000;

    time_steps.push_back( path_time/10000 );

    if (dis_robot_to_goal < 1 && !target_allowed)
    {
        target_allowed = true;

        std::cout << "Target location reached!" << std::endl;

        std::ofstream testRange ("/media/kenni/usb1/linuxUbuntu/Robotics5semesterProject/Tests/TangentbugTest/tangentbugObs.txt");
          if (testRange.is_open())
          {
              for (int i = 0; i < obstacle_range.size(); i++)
              {
                  testRange << time_steps[i];
                  testRange << " ";
                  testRange << obstacle_range[i];
                  testRange << std::endl;
              }
           testRange.close();
          }
          else {
              std::cout << "Cannot open file!" << std::endl;
          }

         std::ofstream testDis ("/media/kenni/usb1/linuxUbuntu/Robotics5semesterProject/Tests/TangentbugTest/tangentbugDis.txt");
            if (testDis.is_open())
            {
                for (int j = 0; j < distance_traveled.size(); j++)
                {
                    testDis << time_steps[j];
                    testDis << " ";
                    testDis << distance_traveled[j];
                    testDis << std::endl;
                }
             testDis.close();
            }
            else {
                std::cout << "Cannot open file!" << std::endl;
            }
        std::cout << "Target location reached!";
      //  target_location_reached = true;
    }
*/

    rotation.orientation_to_goal = homogeneous_transformation(pos,goal);
    rotation.orientation_to_obstacle = homogeneous_transformation(pos,pos.obstacle_point);

    goal.x = (goal.x*1.411764706+60)*5; // (120/2)/(85/2) = 1.411..
    goal.y = ((-1)*goal.y*1.428571429+40)*5; // (80/2)/(56/2) = 1.428.

    show_path_of_robot(pos.robot_pos_pic, cv::Point(0,0), goal, target_location_reached);
}

void motion_planning::model_based_planner(ct::current_position pos, std::vector<cv::Point> points, float index, float indexRooms, cv::Point room)
{
   rotation.orientation_to_goal = homogeneous_transformation(pos,points[index]);
   cv::Point start, nextPoint;

   if ( sqrt( pow( (pos.robot_pos.x - points[index].x),2) + pow( (pos.robot_pos.y - points[index].y),2) ) < 0.02 )
       target_location_reached = true;
   else
       target_location_reached = false;

   cv::Point2f goal = points[points.size()-1];

   start.x = (points[0].x*1.411764706+60)*5; // (120/2)/(85/2) = 1.411..
   start.y = ((-1)*points[0].y*1.428571429+40)*5; // (80/2)/(56/2) = 1.428..

   nextPoint.x = ((points[index].x)*1.411764706+60)*5; // (120/2)/(85/2) = 1.411..
   nextPoint.y = (((-1)*points[index].y)*1.428571429+40)*5; // (80/2)/(56/2) = 1.428..

   goal.x = (points[points.size()-1].x*1.411764706+60)*5; // (120/2)/(85/2) = 1.411..
   goal.y = ((-1)*points[points.size()-1].y*1.428571429+40)*5; // (80/2)/(56/2) = 1.428.

   show_path_of_robot(pos.robot_pos_pic, goal, nextPoint, target_location_reached);
}

