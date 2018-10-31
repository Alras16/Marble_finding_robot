#include "motion_planning.h"

motion_planning::motion_planning()
{

}


goal_position motion_planning::tangent_bug_algoritm(vector<vector<cv::Point>> list_of_points, vector<int> rooms){

    ofstream target_locations;

    vector<int>coordinates_x;
    vector<int>coordinates_y;

    bool x_coordinate_found = false;
    bool y_coordinate_found = false;

    int center_of_mass_x = 0, center_of_mass_y = 0;

    if (target_locations.size() == 0){

        for (unsigned int i = 0; i < rooms.size(); i++){

            for(unsigned int j = 0; j < list_of_points[i].size() ; j++){

              coordinates_x.push.back(list_of_points[i][j].x);
              coordinates_y.push.back(list_of_points[i][j].y);

            }

            for (int k = 0; 0  < coordinates_x.size(); k++){

              center_of_mass_x += coordinates_x[k];

            }
            center_of_mass_x = center_of_mass_x/coordinates_x.size();

            for (int k = 0; 0  < coordinates_y.size(); k++){

              center_of_mass_y += coordinates_y[k];

            }
            center_of_mass_y = center_of_mass_y/coordinates_y.size();

            target_locations.open("target_locations");
            target_locations << center_of_mass_x << "," << center_of_mass_y << ""
        }

    }


}

