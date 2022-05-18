#include "SwordTrail.h"

void SwordTrail::SetSwordPos(const Vec3& topPos, const Vec3& underPos) {
	TrailBuffer& p = posArray.emplace_back();
	p.top = topPos;
	p.under = underPos;
	p.isUsed = true;
}

void SwordTrail::Update() {
	for (auto& p : posArray) {
		//if (!p.isUsed) posArray.erase(p);
	}

}
