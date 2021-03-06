#include "Cost.h"

bool Cost::Consume(float time) {
	if (Approval(time)) {
		costTimer -= time;
		ct = true;
		return true;
	}
	return false;
}

void Cost::Update(float elapsedTime, bool sb) {
	if (tutorial) {
		if (trg) {
			costTimer -= elapsedTime;
			ct = true;
		}
		else {
			if (sb) {
				if (ctTimer >= 0.0f && ct && trg) {
					ctTimer -= elapsedTime * 0.30f;
				}
				else {
					if (costTimer < TUTORIAL_COST) {
						costTimer += elapsedTime * 0.30f;
					}
					else {
						costTimer = TUTORIAL_COST;
						ctTimer = MAX_CT;
						ct = false;
					}
				}
			}
		}
	}
	else {
		if (trg) {
			costTimer -= elapsedTime;
			ct = true;
		}
		else {
			if (sb) {
				if (ctTimer >= 0.0f && ct && trg) {
					ctTimer -= elapsedTime * 0.05f;
				}
				else {
					if (costTimer < MAX_COST) {
						costTimer += elapsedTime * 0.30f;
					}
					else {
						costTimer = MAX_COST;
						ctTimer = MAX_CT;
						ct = false;
					}
				}
			}
		}
	}
}

void Cost::Reset() {
	// チュートリアルか否か
	costTimer = tutorial ? TUTORIAL_COST : MAX_COST;
	ctTimer = MAX_CT;
	ct = false;
	trg = false;
}
