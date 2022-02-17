#pragma once

#include <DirectXMath.h>
#include <memory>
#include "Model.h"

// ヒット計算
struct HitResult
{
	Vec3 position = { 0, 0, 0 }; // レイとポリゴンの交点
	Vec3 normal = { 0, 0, 0 };   // 衝突したポリゴンの法線ベクトル
	float distance = 0.0f;                    // レイの始点から交点までの距離
	int materialIndex = -1;                   // 衝突したポリゴンのマテリアル番号
	Vec3 rotation = { 0, 0, 0 }; // 回転量
};

class Collision
{
public:
	static bool SphereVsSphere(DirectX::XMFLOAT3 pos1, DirectX::XMFLOAT3 pos2, float r1, float r2);
	static bool BoundingVsBounding(const Model* model, const Model* model2, HitResult& hit);
	static bool SphereVsCapsule(DirectX::XMFLOAT3& capsule1, DirectX::XMFLOAT3 capsule2, float capsuleRadius, DirectX::XMFLOAT3 sphere, float sphereRadius);

	// 2次元での矩形と矩形の衝突判定 & 押し出し 関数
	static bool RectVsRectAndExtrusion(DirectX::XMFLOAT2& pos1, DirectX::XMFLOAT2 scale1, DirectX::XMFLOAT2 pos2, DirectX::XMFLOAT2 scale2);

	// 2次元での円と円の衝突判定 & 押し出し 関数
	static bool CircleVsCircleAndExtrusion(DirectX::XMFLOAT2& pos1, float radius1, DirectX::XMFLOAT2 pos2, float radius2, bool extrusion = true);

	// 2次元での円の押し出し 関数
	static void CircleExtrusion(DirectX::XMFLOAT2& pos1, float radius1, DirectX::XMFLOAT2 pos2, float radius2);

	// 点と矩形の当たり判定
	static bool PointVsRect(const DirectX::XMFLOAT2& pos1, const DirectX::XMFLOAT2 pos2, const DirectX::XMFLOAT2 size2);

	// 3次元座標上の線分と3角ポリゴンが交差してるかを判定
	bool detectIsIntersectedLineSegmentAndPolygon(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2);

	// ポリゴン上に点が含まれるかを判定
	bool detectPointIsEnclosedByPolygon(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2);

	// 3次元座標上の線分と平面の交点座標を求める
	DirectX::XMFLOAT3 calcIntersectionLineSegmentAndPlane(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2);

	// ある点から平面までの距離
	float calcDistancePointAndPlane(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2);

	// 3次元座標上の線分と平面が交差してるかを判定
	bool detectCollisionLineSegmentAndPlane(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2);


	// レイとモデルの交差判定
	static bool Collision::RayVsModel(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		SkinnedMesh* skin,
		DirectX::XMFLOAT4X4 transform,
		HitResult& result);


	static bool AabbVsAabb(
		const DirectX::XMFLOAT3& positionA,
		float widthA,
		float heightA,
		const DirectX::XMFLOAT3& positionB,
		float widthB,
		float heightB
	);

	//球と球の交差判定
	static bool IntersectSphereVsSphere(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		DirectX::XMFLOAT3& outPositionB
	);

	static bool calcRaySphere(
		DirectX::XMFLOAT3 l,
		DirectX::XMFLOAT3 v,
		DirectX::XMFLOAT3 p,
		float r,
		DirectX::XMFLOAT3& q1,
		DirectX::XMFLOAT3& q2
	);

private:
	// 球の中心とカプセルの線分の距離（の二乗）を計算用
	static float GetSqDistancePoint2Segment(DirectX::XMFLOAT3 point, DirectX::XMFLOAT3 seg_start, DirectX::XMFLOAT3 seg_end, DirectX::XMFLOAT3& dir);
	float LenSegOnSepAxis(DirectX::XMFLOAT3* sep, DirectX::XMFLOAT3* v1, DirectX::XMFLOAT3* v2, DirectX::XMFLOAT3* v3 = 0);

	float Dot(DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1);
	DirectX::XMFLOAT3 Cross(DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1);
	DirectX::XMFLOAT3 Normalize(DirectX::XMFLOAT3 v);

	//collisionSpCapFlg = false;
	////円柱部のベクトル（p0からp1）
	//Vec3 cylinderVec = c0.p1 - c0.p0;
	////射影したベクトル                         //中心//円柱の中心線の始端
	//Vec3 proj = VecMath::Projection(cylinderVec, sp.c - c0.p0);
	//Vec3 len = sp.c - (c0.p0 + proj);
	//float d = VecMath::LengthVec3(len);

	//// 射影長が円柱部から外れていない  p0側
	//// 射影ベクトルと円柱のベクトルの向きが違う
	//if (!VecMath::SignVec(cylinderVec, proj)) {
	//	// 長さを測り直す
	//	len = sp.c - c0.p0;
	//	d = VecMath::LengthVec3(len);
	//}
	//// 射影長が円柱部から外れていない  p1側 
	//// 射影長の長さが円柱の長さをこえた
	//if (VecMath::LengthVec3(proj) > VecMath::LengthVec3(cylinderVec)) {
	//	// 長さを測り直す
	//	len = sp.c - c0.p1;
	//	d = VecMath::LengthVec3(len);
	//}
	//// 射影の長さが球とカプセルの半径より小さいと当たっている
	//if (d < sp.r + c0.r) collisionSpCapFlg = true;
};

extern Collision collision;

