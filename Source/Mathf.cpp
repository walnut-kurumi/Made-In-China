#include"Mathf.h"
#include<stdlib.h>

//垂直移動更新処理
float Mathf::Lerp(float a, float b, float t)
{
	return a * (1.0f - t) + (b * t);
}

//指定範囲のランダム値を計算する
float Mathf::RandomRange(float min, float max)
{
	// 0.0～1.0の間までのランダム値
	float value = static_cast<float>(rand()) / RAND_MAX;
	// min～maxまでのランダム値に変換
	return min + (max - min) * value;
}