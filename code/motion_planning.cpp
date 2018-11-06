#include "motion_planning.h"

motion_planning::motion_planning()
{

}


goal_position motion_planning::tangent_bug_algoritm(){


    // Load the data from the text file into the vector containing info of the center of mass
    // of the rooms


    // Find the center of mass of the rooms and save it in a vector to be used throughtout the
    // rest of the simulation

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

            target_locations[i].push_back(center_of_mass_x);
            target_locations[i].push_back(center_of_mass_y);
        }

    }


    // Start tangent bug algoritm and continue to all rooms is serached

    if ()



}

