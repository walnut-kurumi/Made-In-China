#pragma once


class Cost
{
public:
	void Reset();
	void Update(float elapsedTime);
	bool Consume(float time);//減らす
	void Trg(bool t) { trg = t; }
	bool Approval(float time) { return costTimer >= time ? true : false; }// 許可

	float GetMaxCost() { return MAX_COST; }
	float GetCost() { return costTimer; }
	bool GetTrg() { return trg; }

private:
	const float MAX_COST = 3.0f; // 2倍の時間になる
	const float MAX_CT = 2.0f;
	
	float costTimer = MAX_COST;
	float ctTimer = MAX_CT;

	bool ct = false;

	bool trg = false;

};