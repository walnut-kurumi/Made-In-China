#include "Keyboard.h"
void Key::Set(const char key) {
	keysTrg[key] = true;
	keys[key] = true;
	// “ü—Í‚ª•¶šƒR[ƒh‚È‚ç
	if (!(key <= 90 && key >= 65)) return;
	keysTrg[key + 32] = true;
	keys[key + 32] = true;
}
void Key::Release(const char key) {
	keysTrg[key] = false;
	keys[key] = false;
	if (!(key <= 90 && key >= 65)) return;
	keysTrg[key + 32] = false;
	keys[key + 32] = false;
}
void Key::SetAlt(const char key) {
	// 0x12 = Alt
	if (key == 0x12) 
		alt = true;
}
void Key::ReleaseAlt(const char key) {
	// 0x12 = Alt
	if (key == 0x12)
		alt = false;
}
bool Key::TRG(const char key) {
	bool result = false;
	if (keysTrg[key]) result = true;
	keysTrg[key] = false;
	return result;
}
bool Key::ALT() {
	return alt;
}
bool Key::STATE(const char key) {
	bool result = false;
	if (keys[key])	result = true;
	return result;
}