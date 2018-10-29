#ifndef FUZZYBUGCONTROLLER_H
#define FUZZYBUGCONTROLLER_H

/*************************************************************/
/*************************************************************/

#include "laserscanner.h"
#include <fl/Headers.h>

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
  //  FuzzyBugController();
    FuzzyBugController(LaserScanner* pc_laser_scanner);
    virtual ~FuzzyBugController() = default;

    virtual void buildController();
    virtual ControlOutput getControlOutput();

protected:
    LaserScanner*        m_pcLaserScanner;

    fl::Engine*          m_pcFLEngine;
    fl::InputVariable*   m_pflObstacleDirection;
    fl::InputVariable*   m_pflObstacleDistance;
    fl::InputVariable*   m_pflGoalDirection;
    fl::InputVariable*   m_pflGoalDistance;
    fl::OutputVariable*  m_pflSteerDirection;
    fl::OutputVariable*  m_pflSpeed;
};

/*************************************************************/
/*************************************************************/

#endif // FUZZYBUGCONTROLLER_H
