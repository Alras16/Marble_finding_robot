

#include "fuzzybugcontroller.h"

FuzzyBugController::FuzzyBugController()
{

}

/*************************************************************/
/*************************************************************/

ControlOutput FuzzyBugController::getControlOutput(ct::line line_input, ct::marble marble_input)
{
    m_pflObstacleDistance->setValue(line_input.range);
    m_pflObstacleDirection->setValue(line_input.alpha);
    m_pflGoalDirection->setValue(marble_input.distance_to_center);

   // std::cout << "FL - Distance " << m_pcLaserScanner->getClosestDistance(-1.57, 1.57) << ", direction " << m_pcLaserScanner->getClosestDirection(-1.57, 1.57) << std::endl;

    m_pcFLEngine->process();

    ControlOutput out;
    out.direction = m_pflSteerDirection->getValue();
    out.speed     = m_pflSpeed->getValue();

    return out;
}

/*************************************************************/
/*************************************************************/

void FuzzyBugController::buildController()
{
    using namespace fl;
    m_pcFLEngine = FllImporter().fromFile("/media/kenni/usb1/linuxUbuntu/rb-rca5/robot_control/fuzzybugcontroller.fll");

    std::string status;
    if (not m_pcFLEngine->isReady(&status))
        throw Exception("[engine error] engine is not ready:n" + status, FL_AT);

    m_pflObstacleDirection = m_pcFLEngine->getInputVariable("ObstacleDirection");
    m_pflObstacleDistance  = m_pcFLEngine->getInputVariable("ObstacleDistance");
    m_pflGoalDirection     = m_pcFLEngine->getInputVariable("GoalDirection");
    m_pflGoalDistance      = m_pcFLEngine->getInputVariable("GoalDistance");
    m_pflSteerDirection    = m_pcFLEngine->getOutputVariable("SteerDirection");
    m_pflSpeed             = m_pcFLEngine->getOutputVariable("Speed");
}

/*************************************************************/
/*************************************************************/
