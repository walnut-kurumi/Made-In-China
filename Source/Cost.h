#pragma once


class Cost
{
public:
	void Reset();
	void Update(float elapsedTime);
	bool Consume(float time);//���炷
	void Trg(bool t) { trg = t; }
	bool Approval(float time) { return costTimer >= time ? true : false; }// ����

	float GetMaxCost() { return tutorial ? TUTORIAL_COST : MAX_COST;; }
	float GetCost() { return costTimer; }
	bool GetTrg() { return trg; }
	void SetTutorial(bool b) { tutorial = b; }

private:
	const float MAX_COST = 3.0f; // 2�{�̎��ԂɂȂ�
	const float TUTORIAL_COST = 20.0f; // �`���[�g���A���p�R�X�g
	const float MAX_CT = 2.0f;
	
	float costTimer = MAX_COST;
	float ctTimer = MAX_CT;

	bool ct = false;

	bool trg = false;

	bool tutorial = false;

};