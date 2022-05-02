#pragma once


class Cost
{
public:
	void Reset();
	void Update(float elapsedTime);
	bool Consume(float time);//Œ¸‚ç‚·
	void Trg(bool t) { trg = t; }
	bool Approval(float time) { return costTimer >= time ? true : false; }// ‹–‰Â

	float GetMaxCost() { return MAX_COST; }
	float GetCost() { return costTimer; }

private:
	const float MAX_COST = 100.0f; // 2”{‚ÌŽžŠÔ‚É‚È‚é
	const float MAX_CT = 2.0f;
	
	float costTimer = MAX_COST;
	float ctTimer = MAX_CT;

	bool ct = false;

	bool trg = false;

};