#include "motion_planning.h"
#include <fstream>
#include <ctime>


motion_planning::motion_planning(){}


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
       cv::circle(world,room_info[i].centerOfMassPic,3,cv::Scalar({255,255,0}),-1);
    }

   cv::imshow("World",world);

}

void motion_planning::show_path_of_robot(cv::Point pos, cv::Point room_start, cv::Point room_end, bool target_reached)

{
    if (!dummy)
    {
        cv::Point start;
        start.x = path_of_robot.cols/2;
        start.y = path_of_robot.rows/2;
        cv::circle(path_of_robot,room_start,4,cv::Scalar({0,0,0}),-1);
        cv::circle(path_of_robot,room_end,4,cv::Scalar({0,0,255}),-1);
        dummy++;
    }
     cv::circle(path_of_robot,pos,1,cv::Scalar({100,255,255}),-1);
     cv::imshow("Path of robot",path_of_robot);

     if ( target_reached && (dummy == 1) )
     {
        cv::imwrite("/media/kenni/usb1/linuxUbuntu/Robotics5semesterProject/Tests/test3.png", path_of_robot);
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


    cv::Point test_point, obstacle, obstacle_point;
    ct::polarPoint robot_to_goal;
    std::vector<ct::polarPoint> data = data_ori;

    float result1, result2 = 1000, goal_x, goal_y, boundary_x, boundary_y,dis_robot_to_obstacle, dis_obstacle_to_goal, dis_robot_to_goal;
    float d_followed, d_reach, shortest_distance_to_obstacle = 100;
    cv::Point2f goal; // = Rooms[index].centerOfMass;

    // Find the point on the obstacle (if there are any) that minimizes the path from the robot to the obstacle and then to the goal
    goal.x = -30; //Rooms[index].centerOfMass.x;
    goal.y = 13; //Rooms[index].centerOfMass.y;

    d_followed = sqrt( pow((pos.obstacle_point.x-goal.x),2) + pow((pos.obstacle_point.y-goal.y),2));
    d_reach = sqrt( pow((goal.x-pos.robot_pos.x),2) + pow((goal.y-pos.robot_pos.y),2));
    dis_robot_to_goal = sqrt( pow((goal.x-pos.robot_pos.x),2)+pow((goal.y-pos.robot_pos.y),2));

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

    obstacle_range.push_back( ((shortest_distance_to_obstacle/72)*25.4) );
    pos.obstacle_point = obstacle;

    // Calculate running time of robot path
    timeval curTime;
    gettimeofday(&curTime, NULL);
    float milli = curTime.tv_usec / 1000;
    float sec = curTime.tv_usec;
    path_time += sec/1000 + milli/1000;

    time_steps.push_back( path_time/10000 );

    // std::cout << "Time: " << path_time/10000 << std::endl;
    //std:: cout << dis_robot_to_obstacle << std::endl;

    if (dis_robot_to_goal < 0.5 && !target_location_reached )
    {
        std::ofstream myfile ("/media/kenni/usb1/linuxUbuntu/Robotics5semesterProject/Tests/test3.txt");
          if (myfile.is_open())
          {
              for (int i = 0; i < obstacle_range.size(); i++)
              {
                  myfile << time_steps[i];
                  myfile << " ";
                  myfile << obstacle_range[i];
                  myfile << std::endl;
              }
           myfile.close();
          }
          else {
              std::cout << "Cannot open file!" << std::endl;
          }

        std::cout << "Target location reached!";
        target_location_reached = true;
    }

/*
    std::cout << "Current pos: " << pos.robot_pos.x << "," << pos.robot_pos.y << "   " << "Obstacle pos: " << obstacle.x<<","<< obstacle.y << std::endl;
    std::cout << dis_obstacle_to_goal << std::endl;
    std::cout << dis_robot_to_goal << std::endl;
    std::cout << dis_robot_to_obstacle << std::endl;
*/
    rotation.orientation_to_goal = homogeneous_transformation(pos,goal);
    rotation.orientation_to_obstacle = homogeneous_transformation(pos,pos.obstacle_point);

   // show_path_of_robot(pos.robot_pos_pic,Rooms[9].centerOfMassPic, Rooms[1].centerOfMassPic, target_location_reached);

  //  std::cout << "Current pos: " << pos.robot_pos.x << "," << pos.robot_pos.y << "   " << "Goal pos: " << goal.x<<","<< goal.y << std::endl;


}

