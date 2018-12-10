
#include "fuzzybugcontroller.h"

FuzzyBugController::FuzzyBugController(LaserScanner *pc_laser_scanner) : m_pcLaserScanner(pc_laser_scanner)
{

}

/*************************************************************/
/*************************************************************/

ct::robot_action FuzzyBugController::getControlOutput(ct::marble marble_input, ct::robot_orientation angle)
{
    ct::robot_action robot_action_output;
    ct::angle_to_obstacle fAngle;

    obstacleDistance = m_pcLaserScanner->getClosestDistance(-1.57, 1.57);
    fAngle = m_pcLaserScanner->getClosestDirection(-1.2,1.2);

    //std::cout << fAngle.fFurthestReading<< std::endl;

    m_pflObstacleDistance->setValue(obstacleDistance);
    m_pflObstacleDirection->setValue(fAngle.fSmallestReading);
    m_pflObstacleFree->setValue(fAngle.fFurthestReading);
    m_pflMarbleFound->setValue(marble_input.radius);
    m_pflMarbleDirection->setValue(marble_input.distance_to_center);
    m_pflGoalDirection->setValue(angle.orientation_to_goal);
    m_pflBoundaryDirection->setValue(angle.orientation_to_obstacle);

   // std::cout << "FL - Distance " << m_pcLaserScanner->getClosestDistance(-1.57, 1.57) << ", direction " << m_pcLaserScanner->getClosestDirection(-1.57, 1.57) << std::endl;

    m_pcFLEngine->process();

    robot_action_output.dir = m_pflSteerDirection->getValue();
    robot_action_output.speed = m_pflSpeed->getValue();

    return robot_action_output;
}

/*************************************************************/
/*************************************************************/

void FuzzyBugController::buildController()
{
    using namespace fl;
    m_pcFLEngine = FllImporter().fromFile("/home/kenni/git_workspace/Marble_finding_robot/code/fuzzybugcontroller.fll");

    std::string status;
    if (not m_pcFLEngine->isReady(&status))
        throw Exception("[engine error] engine is not ready:n" + status, FL_AT);

    m_pflObstacleDirection = m_pcFLEngine->getInputVariable("ObstacleDirection");
    m_pflObstacleFree      = m_pcFLEngine->getInputVariable("ObstacleFree");
    m_pflObstacleDistance  = m_pcFLEngine->getInputVariable("ObstacleDistance");
    m_pflMarbleDirection   = m_pcFLEngine->getInputVariable("MarbleDirection");
    m_pflMarbleFound       = m_pcFLEngine->getInputVariable("MarbleFound");
    m_pflGoalDirection     = m_pcFLEngine->getInputVariable("GoalDirection");
    m_pflBoundaryDirection = m_pcFLEngine->getInputVariable("BoundaryDirection");
    m_pflSteerDirection    = m_pcFLEngine->getOutputVariable("SteerDirection");
    m_pflSpeed             = m_pcFLEngine->getOutputVariable("Speed");
}

/*************************************************************/
/*************************************************************/
