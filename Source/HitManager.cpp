#include "HitManager.h"
#include "StageManager.h"

bool HitManager::RayCast(const Vec3& start, const Vec3& end, HitResult& hit)
{
    bool result = false;
    hit.distance = FLT_MAX;

    if (StageManager::Instance().RayCast(start, end, hit)) result = true;


    return result;
}