#pragma once

#include "Collision.h"
#include "Graphics/Vec.h"

class HitManager
{
public:
	HitManager() {}
	~HitManager() {}

	static HitManager& Instance()
	{
		static HitManager instance;
		return instance;
	}

	// レイキャスト
	bool RayCast(const Vec3& start, const Vec3& end, HitResult& hit);

private:

};
