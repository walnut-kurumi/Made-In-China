#include "Collision.h"
#include "Framework.h"
Collision collision;


//******************************************************
//球体　vs　球体
//******************************************************
bool Collision::SphereVsSphere(DirectX::XMFLOAT3 pos1, DirectX::XMFLOAT3 pos2, float r1, float r2)
{
	if ((pos2.x - pos1.x) * (pos2.x - pos1.x) + (pos2.y - pos1.y) * (pos2.y - pos1.y) + (pos2.z - pos1.z) * (pos2.z - pos1.z) <= (r1 + r2) * (r1 + r2))
	{
		return true;
	}
	else return false;
}


bool Collision::BoundingVsBounding(const Model* model, const Model* model2, HitResult& hit)
{
	for (const SkinnedMesh::Mesh& mesh : model->GetMeshs())
	{
		for (const SkinnedMesh::Mesh& mesh2 : model2->GetMeshs())
		{
			DirectX::XMFLOAT3 min = mesh.boundingBox[0];
			DirectX::XMFLOAT3 max = mesh.boundingBox[1];

			DirectX::XMFLOAT3 min2 = mesh2.boundingBox[0];
			DirectX::XMFLOAT3 max2 = mesh2.boundingBox[1];


			if (min.x < max2.x) return true;
			if (min2.x < max.x) return true;

			if (min.y < max2.y) return true;
			if (min2.y < max.y) return true;

			if (min.z < max2.z) return true;
			if (min2.z < max.z) return true;
		}
	}
	return false;
}

//******************************************************
//球体　vs　カプセル
//******************************************************
bool Collision::SphereVsCapsule(DirectX::XMFLOAT3& capsule1, DirectX::XMFLOAT3 capsule2, float capsule_radius, DirectX::XMFLOAT3 sphere, float sphere_radius)
{
	// 球の中心とカプセルの線分の距離（の二乗）を計算
	DirectX::XMFLOAT3 dir;
	float dis = GetSqDistancePoint2Segment(sphere, capsule1, capsule2, dir);

	// 距離（の二乗）が半径の和（の二乗）より小さければ当たっている
	float radius = sphere_radius + capsule_radius;

	if (radius * radius < dis) return false;
	if (dis == 0.0f) return false;
	if (dir.x == 0.0f && dir.y == 0.0f && dir.z == 0.0f) return false;

	float length = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

	dir.x /= length;
	dir.y /= length;
	dir.z /= length;

	capsule1.x = sphere.x + radius * dir.x;
	capsule1.y = sphere.y + radius * dir.y;
	capsule1.z = sphere.z + radius * dir.z;

	return true;
}
float Collision::GetSqDistancePoint2Segment(DirectX::XMFLOAT3 point, DirectX::XMFLOAT3 seg_start, DirectX::XMFLOAT3 seg_end, DirectX::XMFLOAT3& dir)
{
	const float eqsilon = 1.0e-5f;	// 誤差吸収用の微小な値

	// 線分の始点から終点へのベクトル
	DirectX::XMFLOAT3 segment_sub = DirectX::XMFLOAT3(seg_end.x - seg_start.x, seg_end.y - seg_start.y, seg_end.z - seg_start.z);

	// 線分の始点から点へのベクトル
	DirectX::XMFLOAT3 segment_point = DirectX::XMFLOAT3(point.x - seg_start.x, point.y - seg_start.y, point.z - seg_start.z);
	dir = segment_point;

	// 射影ベクトル
	DirectX::XMFLOAT3 cross_point;

	// 2ベクトルの内積
	float dot = (segment_sub.x * segment_point.x) + (segment_sub.y * segment_point.y) + (segment_sub.z * segment_point.z);
	if (dot < eqsilon)
	{// 内積が負なら、線分の始点が最近傍
		return ((segment_point.x * segment_point.x) + (segment_point.y * segment_point.y) + (segment_point.z * segment_point.z));
	}

	// 点から線分の終点へのベクトル
	segment_point = DirectX::XMFLOAT3(seg_end.x - point.x, seg_end.y - point.y, seg_end.z - point.z);

	// 2ベクトルの内積
	dot = (segment_sub.x * segment_point.x) + (segment_sub.y * segment_point.y) + (segment_sub.z * segment_point.z);
	if (dot < eqsilon)
	{// 内積が負なら、線分の終点が最近傍
		return ((segment_point.x * segment_point.x) + (segment_point.y * segment_point.y) + (segment_point.z * segment_point.z));
	}

	// 上記のどちらにも該当しない場合、線分上に落とした射影が最近傍
	// (本来ならサインで求めるが、外積の大きさ/線分のベクトルの大きさで求まる)
	cross_point.x = segment_sub.y * segment_point.z - segment_sub.z * segment_point.y;
	cross_point.y = segment_sub.z * segment_point.x - segment_sub.x * segment_point.z;
	cross_point.z = segment_sub.x * segment_point.y - segment_sub.y * segment_point.x;
	dir = segment_sub;
	return (((cross_point.x * cross_point.x) + (cross_point.y * cross_point.y) + (cross_point.z * cross_point.z))
		/ ((segment_sub.x * segment_sub.x) + (segment_sub.y * segment_sub.y) + (segment_sub.z * segment_sub.z)));
}

float Collision::LenSegOnSepAxis(DirectX::XMFLOAT3* sep, DirectX::XMFLOAT3* v1, DirectX::XMFLOAT3* v2, DirectX::XMFLOAT3* v3)
{
	// 3つの内積の絶対値の和で投影線分長を計算
	// 分離軸Sepは標準化されていること
	float dot1 = ((sep->x * v1->x) + (sep->y * v1->y) + (sep->z * v1->z));
	float dot2 = ((sep->x * v2->x) + (sep->y * v2->y) + (sep->z * v2->z));
	float dot3 = 0.0f;
	if (!v3)
	{
		dot3 = 0.0f;
	}
	else
	{
		dot3 = ((sep->x * v3->x) + (sep->y * v3->y) + (sep->z * v3->z));
	}

	float r1 = fabs(dot1);
	float r2 = fabs(dot2);
	float r3 = v3 ? (fabs(dot3)) : 0;

	return r1 + r2 + r3;
}

// 矩形　vs　矩形
bool Collision::RectVsRectAndExtrusion(DirectX::XMFLOAT2& pos1, DirectX::XMFLOAT2 scale1, DirectX::XMFLOAT2 pos2, DirectX::XMFLOAT2 scale2)
{
	bool isHit = false;

	if (pos1.x - scale1.x <= pos2.x - scale2.x)
	{
		pos1.x = pos2.x - scale2.x + scale1.x;
		isHit = true;
	}
	if (pos2.x + scale2.x <= pos1.x + scale1.x)
	{
		pos1.x = pos2.x + scale2.x - scale1.x;
		isHit = true;
	}
	if (pos1.y - scale1.y <= pos2.y - scale2.y)
	{
		pos1.y = pos2.y - scale2.y + scale1.y;
		isHit = true;
	}
	if (pos2.y + scale2.y <= pos1.y + scale1.y)
	{
		pos1.y = pos2.y + scale2.y - scale1.y;
		isHit = true;
	}

	return isHit;
}

//円　vs　円
bool Collision::CircleVsCircleAndExtrusion(DirectX::XMFLOAT2& pos1, float radius1, DirectX::XMFLOAT2 pos2, float radius2, bool extrusion)
{
	if ((pos2.x - pos1.x) * (pos2.x - pos1.x) + (pos2.y - pos1.y) * (pos2.y - pos1.y) <= (radius1 + radius2) * (radius1 + radius2))
	{
		if (extrusion) CircleExtrusion(pos1, radius1, pos2, radius2);

		return true;
	}
	else return false;
}

void Collision::CircleExtrusion(DirectX::XMFLOAT2& pos1, float radius1, DirectX::XMFLOAT2 pos2, float radius2)
{
	DirectX::XMFLOAT2 dir = { pos1.x - pos2.x, pos1.y - pos2.y };
	float length = sqrtf(dir.x * dir.x + dir.y * dir.y);

	dir.x /= length;
	dir.y /= length;

	pos1.x = pos2.x + (radius1 + radius2) * dir.x;
	pos1.y = pos2.y + (radius1 + radius2) * dir.y;
}

//点　vs　矩形
bool Collision::PointVsRect(const DirectX::XMFLOAT2& pos1, const DirectX::XMFLOAT2 pos2, const DirectX::XMFLOAT2 size2)
{
	if (pos2.x > pos1.x) return false;
	if (pos2.y > pos1.y) return false;
	if (pos2.x + size2.x < pos1.x) return false;
	if (pos2.y + size2.y < pos1.y) return false;

	return true;
}

float Collision::Dot(DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1)
{
	float dot;

	dot = (v0.x * v1.x) + (v0.y * v1.y) + (v0.z * v1.z);

	return dot;
}
DirectX::XMFLOAT3 Collision::Cross(DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1)
{
	DirectX::XMFLOAT3 cross;
	cross.x = v0.y * v1.z - v0.z * v1.y;
	cross.y = v0.z * v1.x - v0.x * v1.z;
	cross.z = v0.x * v1.y - v0.y * v1.x;
	return cross;
}
DirectX::XMFLOAT3 Collision::Normalize(DirectX::XMFLOAT3 v)
{
	DirectX::XMFLOAT3 _v = v;
	float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);

	_v.x /= length;
	_v.y /= length;
	_v.z /= length;

	return _v;
}

// 3次元座標上の線分と3角ポリゴンが交差してるかを判定
bool Collision::detectIsIntersectedLineSegmentAndPolygon(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2)
{
	bool bCollision = detectCollisionLineSegmentAndPlane(a, b, v0, v1, v2);

	if (bCollision)
	{
		DirectX::XMFLOAT3 p = calcIntersectionLineSegmentAndPlane(a, b, v0, v1, v2);
		if (detectPointIsEnclosedByPolygon(p, v0, v1, v2))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

// ポリゴン上に点が含まれるかを判定
bool Collision::detectPointIsEnclosedByPolygon(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2)
{
	DirectX::XMFLOAT3 vec1 = { v1.x - v0.x, v1.y - v0.y, v1.z - v0.z };
	DirectX::XMFLOAT3 vec2 = { v2.x - v1.x, v2.y - v1.y, v2.z - v1.z };
	DirectX::XMFLOAT3 vec3 = { v0.x - v2.x, v0.y - v2.y, v0.z - v2.z };

	DirectX::XMFLOAT3 vec4 = { p.x - v1.x, p.y - v1.y, p.z - v1.z };
	DirectX::XMFLOAT3 vec5 = { p.x - v2.x, p.y - v2.y, p.z - v2.z };
	DirectX::XMFLOAT3 vec6 = { p.x - v0.x, p.y - v0.y, p.z - v0.z };

	DirectX::XMFLOAT3 n = Normalize(Cross(vec1, vec2));

	DirectX::XMFLOAT3 n0 = Normalize(Cross(vec1, vec4));
	DirectX::XMFLOAT3 n1 = Normalize(Cross(vec2, vec5));
	DirectX::XMFLOAT3 n2 = Normalize(Cross(vec3, vec6));

	if ((1.0f - Dot(n, n0)) > 0.001f) return false;
	if ((1.0f - Dot(n, n1)) > 0.001f) return false;
	if ((1.0f - Dot(n, n2)) > 0.001f) return false;

	return true;
}

// 3次元座標上の線分と平面の交点座標を求める
DirectX::XMFLOAT3 Collision::calcIntersectionLineSegmentAndPlane(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2)
{
	float distAP = calcDistancePointAndPlane(a, v0, v1, v2);
	float distBP = calcDistancePointAndPlane(b, v0, v1, v2);

	float t = distAP / (distAP + distBP);

	DirectX::XMFLOAT3 result;
	result.x = (b.x - a.x) * t + a.x;
	result.y = (b.y - a.y) * t + a.y;
	result.z = (b.z - a.z) * t + a.z;

	return result;
}

// ある点から平面までの距離
float Collision::calcDistancePointAndPlane(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2)
{
	DirectX::XMFLOAT3 vec1 = { v1.x - v0.x, v1.y - v0.y, v1.z - v0.z };
	DirectX::XMFLOAT3 vec2 = { v2.x - v1.x, v2.y - v1.y, v2.z - v1.z };

	DirectX::XMFLOAT3 n = Normalize(Cross(vec1, vec2));
	DirectX::XMFLOAT3 g;
	g.x = (v0.x + v1.x + v2.x) / 3.0f;
	g.y = (v0.y + v1.y + v2.y) / 3.0f;
	g.z = (v0.z + v1.z + v2.z) / 3.0f;

	DirectX::XMFLOAT3 vec3;
	vec3.x = p.x - g.x;
	vec3.y = p.y - g.y;
	vec3.z = p.z - g.z;

	return abs(Dot(n, vec3));
}

// 3次元座標上の線分と平面が交差してるかを判定
bool Collision::detectCollisionLineSegmentAndPlane(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2)
{
	DirectX::XMFLOAT3 vec1 = { v1.x - v0.x, v1.y - v0.y, v1.z - v0.z };
	DirectX::XMFLOAT3 vec2 = { v2.x - v1.x, v2.y - v1.y, v2.z - v1.z };

	DirectX::XMFLOAT3 n = Normalize(Cross(vec1, vec2));
	DirectX::XMFLOAT3 g;
	g.x = (v0.x + v1.x + v2.x) / 3.0f;
	g.y = (v0.y + v1.y + v2.y) / 3.0f;
	g.z = (v0.z + v1.z + v2.z) / 3.0f;

	DirectX::XMFLOAT3 vec3 = { a.x - g.x, a.y - g.y, a.z - g.z };
	DirectX::XMFLOAT3 vec4 = { b.x - g.x, b.y - g.y, b.z - g.z };

	if (Dot(vec3, n) * Dot(vec4, n) <= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//レイとモデルの交差判定
bool Collision::RayVsModel(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	SkinnedMesh* skin,
	DirectX::XMFLOAT4X4 transform,
	HitResult& result)

{
	DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
	DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
	DirectX::XMVECTOR worldRayLength = DirectX::XMVector3Length(WorldRayVec);

	//ワールド空間のレイの長さ
	DirectX::XMStoreFloat(&result.distance, worldRayLength);

	bool hit = false;
	for (const SkinnedMesh::Mesh& mesh : skin->GetMeshs())
	{

		//レイをワールド空間からローカル空間へ変換
		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&mesh.defaultGlobalTransform) * DirectX::XMLoadFloat4x4(&transform);
		DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

		DirectX::XMVECTOR Start = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
		DirectX::XMVECTOR End = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
		DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);
		DirectX::XMVECTOR Dir = DirectX::XMVector3Normalize(Vec);
		DirectX::XMVECTOR Length = DirectX::XMVector3Length(Vec);

		//レイの長さ
		float neart;
		DirectX::XMStoreFloat(&neart, Length);

		//三角形(面)との交差判定
		const std::vector<SkinnedMesh::Vertex>& vertices = mesh.vertices;
		const std::vector<UINT> indices = mesh.indices;

		int materialIndex = -1;
		DirectX::XMVECTOR HitPosition;
		DirectX::XMVECTOR HitNormal;
		for (const SkinnedMesh::Subset& subset : mesh.subsets)
		{
			for (UINT i = 0; i < subset.indexCount; i += 3)
			{
				UINT index = subset.startIndexLocation + i;

				//三角形の頂点を抽出
				const SkinnedMesh::Vertex& a = vertices.at(indices.at(index));
				const SkinnedMesh::Vertex& b = vertices.at(indices.at(index + 1));
				const SkinnedMesh::Vertex& c = vertices.at(indices.at(index + 2));

				DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
				DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
				DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

				//三角形の三辺ベクトルを算出
				DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
				DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
				DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

				//三角形の法線ベクトルを算出
				DirectX::XMVECTOR Normal = DirectX::XMVector3Cross(AB, BC);

				//内積の結果がプラスならば裏向き
				DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Dir, Normal);
				float dot;
				DirectX::XMStoreFloat(&dot, Dot);
				if (dot >= 0.0f)continue;

				//レイと平面の交点を算出
				DirectX::XMVECTOR V = DirectX::XMVectorSubtract(A, Start);
				DirectX::XMVECTOR T = DirectX::XMVectorDivide(DirectX::XMVector3Dot(Normal, V), Dot);
				float t;
				DirectX::XMStoreFloat(&t, T);
				if (t<0.0f || t>neart)continue; //交点までの距離が今までに計算した最近距離より
												//大きいときはスキップ				
				DirectX::XMVECTOR Position = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(Dir, T), Start);

				//交点が三角形の内側にあるか判定
				//1つめ
				DirectX::XMVECTOR V1 = DirectX::XMVectorSubtract(A, Position);
				DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(V1, AB);
				DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, Normal);
				float dot1;
				DirectX::XMStoreFloat(&dot1, Dot1);
				if (dot1 < 0.0f)continue;

				//2つめ
				DirectX::XMVECTOR V2 = DirectX::XMVectorSubtract(B, Position);
				DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(V2, BC);
				DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, Normal);
				float dot2;
				DirectX::XMStoreFloat(&dot2, Dot2);
				if (dot2 < 0.0f)continue;

				//3つめ
				DirectX::XMVECTOR V3 = DirectX::XMVectorSubtract(C, Position);
				DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(V3, CA);
				DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, Normal);
				float dot3;
				DirectX::XMStoreFloat(&dot3, Dot3);
				if (dot3 < 0.0f)continue;



				//最近距離を更新
				neart = t;

				//交点と法線を更新
				HitPosition = Position;
				HitNormal = Normal;
				materialIndex = subset.materialUniqueId;
			}
		}
		if (materialIndex >= 0)
		{

			//ローカル空間からワールド空間へ変換
			DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(HitPosition, WorldTransform);
			DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
			DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
			float distance;
			DirectX::XMStoreFloat(&distance, WorldCrossLength);

			//ヒット情報保存
			if (result.distance > distance)
			{
				DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(HitNormal, WorldTransform);

				result.distance = distance;
				result.materialIndex = materialIndex;
				DirectX::XMStoreFloat3(&result.position, WorldPosition);
				DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));


				hit = true;
			}

		}
	}

	return hit;	
}


//球と球の交差判
bool Collision::IntersectSphereVsSphere(const DirectX::XMFLOAT3& positionA, float radiusA,
	const DirectX::XMFLOAT3& positionB, float radiusB, DirectX::XMFLOAT3& outPositionB)
{
	//using namespace DirectX;を使う場合は極所的に
	//A→Bの単位ベクトルを算出
	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA); //subtractは引き算
	DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LengthSq);
	//距離判定
	float range = radiusA + radiusB;
	if (lengthSq > range * range)
	{
		return false;
	}

	//AがBを押し出す
	Vec = DirectX::XMPlaneNormalize(Vec);
	Vec = DirectX::XMVectorScale(Vec, range);
	PositionB = DirectX::XMVectorAdd(PositionA, Vec);
	DirectX::XMStoreFloat3(&outPositionB, PositionB);

	return true;

}


// l : レイの始点
// v : レイの方向ベクトル
// p : 球の中心点の座標
// r : 球の半径
// q1: 衝突開始点（戻り値）
// q2: 衝突終了点（戻り値）

bool Collision::calcRaySphere(
	DirectX::XMFLOAT3 l,
	DirectX::XMFLOAT3 v,
	DirectX::XMFLOAT3 p,
	float r,
	DirectX::XMFLOAT3& q1,
	DirectX::XMFLOAT3& q2
) {
	p.x = p.x - l.x;
	p.y = p.y - l.y;
	p.z = p.z - l.z;

	float A = v.x * v.x + v.y * v.y + v.z * v.z;
	float B = v.x * p.x + v.y * p.y + v.z * p.z;
	float C = p.x * p.x + p.y * p.y + p.z * p.z - r * r;

	if (A == 0.0f)
		return false; // レイの長さが0

	float s = B * B - A * C;
	if (s < 0.0f)
		return false; // 衝突していない

	s = sqrtf(s);
	float a1 = (B - s) / A;
	float a2 = (B + s) / A;

	if (a1 < 0.0f || a2 < 0.0f)
		return false; // レイの反対で衝突

	q1.x = l.x + a1 * v.x;
	q1.y = l.y + a1 * v.y;
	q1.z = l.z + a1 * v.z;
	q2.x = l.x + a2 * v.x;
	q2.y = l.y + a2 * v.y;
	q2.z = l.z + a2 * v.z;

	return true;
}