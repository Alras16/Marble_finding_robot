#include "laserscanner.h"

/*****************************************************************************/
/*****************************************************************************/

LaserScanner::LaserScanner()
{

}

/*****************************************************************************/
/*****************************************************************************/

 void LaserScanner::parseLaserScannerMessage(ConstLaserScanStampedPtr &msg)
{
    m_mutex.lock();

    float fAngleMin       = float(msg->scan().angle_min());
    float fAngleIncrement = float(msg->scan().angle_step());
    m_fAngleIncrement     = fAngleIncrement;

    m_fRangeMin = float(msg->scan().range_min());
    m_fRangeMax = float(msg->scan().range_max());

    m_nLastUpdateSeconds     = msg->time().sec();
    m_nLastUpdateNanoSeconds = msg->time().nsec();

    int nRanges      = msg->scan().ranges_size();
    int nIntensities = msg->scan().intensities_size();

    assert(nRanges == nIntensities);

    m_vsReadings.resize(nRanges);

    float fCurrentAngle = fAngleMin;
    int i = 0;

    for (auto& r : m_vsReadings)
    {
        float fDistance = float(msg->scan().ranges(i++));
        if (fDistance > m_fRangeMax)
            fDistance = m_fRangeMax;

        r.fDistance    = fDistance;
        r.fAngle       = fCurrentAngle;
        fCurrentAngle += fAngleIncrement;
    }

    m_mutex.unlock();
}

/*****************************************************************************/
/*****************************************************************************/

float LaserScanner::getClosestDistance(float f_start_angle, float f_end_angle)
{
    if (m_vsReadings.empty())
    {
        std::cout << "No readings yet, returning" << std::endl;
        return std::numeric_limits<float>::max();
    }
    //std::cout << "Test" << std::endl;
    m_mutex.lock();

    float fSmallestReading = std::numeric_limits<float>::max();
    float fCurrentReading;

    unsigned int unIndex = 0;
    float fCurrentAngle = m_vsReadings[unIndex].fAngle;
    while (unIndex < m_vsReadings.size() - 1 && fCurrentAngle < f_start_angle - m_fAngleIncrement)
    {
        unIndex++;
        fCurrentAngle = m_vsReadings[unIndex].fAngle;
    }

    while (unIndex < m_vsReadings.size() - 1 && fCurrentAngle < f_end_angle)
    {
        fCurrentReading = m_vsReadings[unIndex].fDistance;
        if (fCurrentReading < fSmallestReading)
        {
            fSmallestReading = fCurrentReading;
        }

        unIndex++;
        fCurrentAngle = m_vsReadings[unIndex].fAngle;
    }

    m_mutex.unlock();
    return fSmallestReading;
}

/*****************************************************************************/
/*****************************************************************************/


ct::angle_to_obstacle LaserScanner::getClosestDirection(float f_start_angle, float f_end_angle)
{

    /*
    if (m_vsReadings.empty())
    {
        std::cout << "No readings yet, returning" << std::endl;
        return std::numeric_limits<float>::max();
    }
*/
    m_mutex.lock();

    float fSmallestReading = std::numeric_limits<float>::max();
    float fFurthestReading = std::numeric_limits<float>::min();
    float fCurrentReading;

    unsigned int unIndex = 0;
    float fCurrentAngle = m_vsReadings[unIndex].fAngle;
    while (unIndex < m_vsReadings.size() - 1 && fCurrentAngle < f_start_angle - m_fAngleIncrement)
    {
        unIndex++;
        fCurrentAngle = m_vsReadings[unIndex].fAngle;
    }

    while (unIndex < m_vsReadings.size() - 1 && fCurrentAngle < f_end_angle)
    {
        fCurrentReading = m_vsReadings[unIndex].fDistance;
        if (fCurrentReading < fSmallestReading)
        {
            fAngle.fSmallestReading = fCurrentAngle + m_fAngleIncrement / 2.0f;
            fSmallestReading = fCurrentReading;
        }

        else if (fCurrentReading > fFurthestReading)
        {
            fAngle.fFurthestReading = fCurrentAngle + m_fAngleIncrement / 2.0f;
            fFurthestReading = fCurrentReading;
        }

        unIndex++;
        fCurrentAngle = m_vsReadings[unIndex].fAngle;
    }

    m_mutex.unlock();
    return fAngle;
}

/*****************************************************************************/
/*****************************************************************************/
