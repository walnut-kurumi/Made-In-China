#pragma once

#include <DirectXMath.h>
#include <memory>
#include "Model.h"

// �q�b�g�v�Z
struct HitResult
{
	Vec3 position = { 0, 0, 0 }; // ���C�ƃ|���S���̌�_
	Vec3 normal = { 0, 0, 0 };   // �Փ˂����|���S���̖@���x�N�g��
	float distance = 0.0f;                    // ���C�̎n�_�����_�܂ł̋���
	int materialIndex = -1;                   // �Փ˂����|���S���̃}�e���A���ԍ�
	Vec3 rotation = { 0, 0, 0 }; // ��]��
};

class Collision
{
public:
	static bool SphereVsSphere(DirectX::XMFLOAT3 pos1, DirectX::XMFLOAT3 pos2, float r1, float r2);
	static bool BoundingVsBounding(const Model* model, const Model* model2, HitResult& hit);
	static bool SphereVsCapsule(DirectX::XMFLOAT3& capsule1, DirectX::XMFLOAT3 capsule2, float capsuleRadius, DirectX::XMFLOAT3 sphere, float sphereRadius);

	// 2�����ł̋�`�Ƌ�`�̏Փ˔��� & �����o�� �֐�
	static bool RectVsRectAndExtrusion(DirectX::XMFLOAT2& pos1, DirectX::XMFLOAT2 scale1, DirectX::XMFLOAT2 pos2, DirectX::XMFLOAT2 scale2);

	// 2�����ł̉~�Ɖ~�̏Փ˔��� & �����o�� �֐�
	static bool CircleVsCircleAndExtrusion(DirectX::XMFLOAT2& pos1, float radius1, DirectX::XMFLOAT2 pos2, float radius2, bool extrusion = true);

	// 2�����ł̉~�̉����o�� �֐�
	static void CircleExtrusion(DirectX::XMFLOAT2& pos1, float radius1, DirectX::XMFLOAT2 pos2, float radius2);

	// �_�Ƌ�`�̓����蔻��
	static bool PointVsRect(const DirectX::XMFLOAT2& pos1, const DirectX::XMFLOAT2 pos2, const DirectX::XMFLOAT2 size2);

	// 3�������W��̐�����3�p�|���S�����������Ă邩�𔻒�
	bool detectIsIntersectedLineSegmentAndPolygon(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2);

	// �|���S����ɓ_���܂܂�邩�𔻒�
	bool detectPointIsEnclosedByPolygon(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2);

	// 3�������W��̐����ƕ��ʂ̌�_���W�����߂�
	DirectX::XMFLOAT3 calcIntersectionLineSegmentAndPlane(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2);

	// ����_���畽�ʂ܂ł̋���
	float calcDistancePointAndPlane(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2);

	// 3�������W��̐����ƕ��ʂ��������Ă邩�𔻒�
	bool detectCollisionLineSegmentAndPlane(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2);


	// ���C�ƃ��f���̌�������
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

	//���Ƌ��̌�������
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
	// ���̒��S�ƃJ�v�Z���̐����̋����i�̓��j���v�Z�p
	static float GetSqDistancePoint2Segment(DirectX::XMFLOAT3 point, DirectX::XMFLOAT3 seg_start, DirectX::XMFLOAT3 seg_end, DirectX::XMFLOAT3& dir);
	float LenSegOnSepAxis(DirectX::XMFLOAT3* sep, DirectX::XMFLOAT3* v1, DirectX::XMFLOAT3* v2, DirectX::XMFLOAT3* v3 = 0);

	float Dot(DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1);
	DirectX::XMFLOAT3 Cross(DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1);
	DirectX::XMFLOAT3 Normalize(DirectX::XMFLOAT3 v);

	//collisionSpCapFlg = false;
	////�~�����̃x�N�g���ip0����p1�j
	//Vec3 cylinderVec = c0.p1 - c0.p0;
	////�ˉe�����x�N�g��                         //���S//�~���̒��S���̎n�[
	//Vec3 proj = VecMath::Projection(cylinderVec, sp.c - c0.p0);
	//Vec3 len = sp.c - (c0.p0 + proj);
	//float d = VecMath::LengthVec3(len);

	//// �ˉe�����~��������O��Ă��Ȃ�  p0��
	//// �ˉe�x�N�g���Ɖ~���̃x�N�g���̌������Ⴄ
	//if (!VecMath::SignVec(cylinderVec, proj)) {
	//	// �����𑪂蒼��
	//	len = sp.c - c0.p0;
	//	d = VecMath::LengthVec3(len);
	//}
	//// �ˉe�����~��������O��Ă��Ȃ�  p1�� 
	//// �ˉe���̒������~���̒�����������
	//if (VecMath::LengthVec3(proj) > VecMath::LengthVec3(cylinderVec)) {
	//	// �����𑪂蒼��
	//	len = sp.c - c0.p1;
	//	d = VecMath::LengthVec3(len);
	//}
	//// �ˉe�̒��������ƃJ�v�Z���̔��a��菬�����Ɠ������Ă���
	//if (d < sp.r + c0.r) collisionSpCapFlg = true;
};

extern Collision collision;

