#pragma once
#include <vector>
#include "Graphics/Vec.h"

class SwordTrail
{
public:
	SwordTrail() {}
	~SwordTrail() {}

	void SetSwordPos(const Vec3& topPos, const Vec3& underPos);
	void Update();

private:
	struct TrailBuffer {
		Vec3 top; 
		Vec3 under;
		bool isUsed;
	};

private:
	std::vector<TrailBuffer> posArray;

	static const size_t KEEP_MAX = 5;

};