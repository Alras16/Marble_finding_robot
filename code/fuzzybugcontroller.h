#ifndef FUZZYBUGCONTROLLER_H
#define FUZZYBUGCONTROLLER_H

/*************************************************************/
/*************************************************************/

#include <fl/Headers.h>
#include "ct.h"
#include "laserscanner.h"
struct ControlOutput
{
    float direction;
    float speed;
};

/*************************************************************/
/*************************************************************/

class FuzzyBugController
{
public:
    FuzzyBugController(LaserScanner* pc_laser_scanner);
    virtual ~FuzzyBugController() = default;

    virtual void buildController();
    ct::robot_action getControlOutput(ct::marble, ct::robot_orientation);

protected:

    float obstacleDistance;
    float obstacleDirection;

    LaserScanner*        m_pcLaserScanner;

    fl::Engine*          m_pcFLEngine;
    fl::InputVariable*   m_pflObstacleDirection;
    fl::InputVariable*   m_pflObstacleFree;
    fl::InputVariable*   m_pflObstacleDistance;
    fl::InputVariable*   m_pflGoalDirection;
    fl::InputVariable*   m_pflBoundaryDirection;
    fl::InputVariable*   m_pflMarbleDirection;
    fl::InputVariable*   m_pflMarbleFound;
    fl::OutputVariable*  m_pflSteerDirection;
    fl::OutputVariable*  m_pflSpeed;

};

/*************************************************************/
/*************************************************************/

#endif // FUZZYBUGCONTROLLER_H
