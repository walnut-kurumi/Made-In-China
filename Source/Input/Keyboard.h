#pragma once
class Key
{
private:
	bool keys[256];
	bool keysTrg[256];
	bool alt;
public:
	void Set(const char key);
	void Release(const char key);
	void SetAlt(const char key);
	void ReleaseAlt(const char key);
	bool STATE(const char key);
	bool TRG(const char key);
	bool ALT();
};