#pragma once
#include <DirectXMath.h>

class Vec2 : public DirectX::XMFLOAT2
{
public:
    Vec2() : DirectX::XMFLOAT2(0, 0) {}
    Vec2(float x, float y) : DirectX::XMFLOAT2(x, y) {}
    Vec2(const Vec2& v) : DirectX::XMFLOAT2(v.x, v.y) {}
    Vec2(DirectX::XMFLOAT2 xy) : DirectX::XMFLOAT2(xy) {}
    ~Vec2() {}

    const DirectX::XMFLOAT2& GetXmfloat2() const { return DirectX::XMFLOAT2(this->x, this->y); }

    Vec2& operator=(const Vec2&);
    Vec2& operator+=(const Vec2&);
    Vec2& operator-=(const Vec2&);
    Vec2& operator*=(float);
    Vec2& operator/=(float);

    Vec2 operator+() const;
    Vec2 operator-() const;

    Vec2 operator+(const Vec2&) const;
    Vec2 operator-(const Vec2&) const;
    Vec2 operator*(float) const;
    friend Vec2 operator*(float, const Vec2&);
    Vec2 operator/(float) const;

    bool operator == (const Vec2&) const;
    bool operator != (const Vec2&) const;
};

class Vec3 : public DirectX::XMFLOAT3
{
public:
    Vec3() : DirectX::XMFLOAT3(0, 0, 0) {}
    Vec3(float x, float y, float z) : DirectX::XMFLOAT3(x, y, z) {}
    Vec3(const Vec3& v) : DirectX::XMFLOAT3(v.x, v.y, v.z) {}
    Vec3(DirectX::XMFLOAT3 xyz) : DirectX::XMFLOAT3(xyz) {}
    ~Vec3() {}

    const DirectX::XMFLOAT3& GetXmfloat3() const { return DirectX::XMFLOAT3(this->x, this->y, this->z); }

    Vec3& operator=(const Vec3&);
    Vec3& operator+=(const Vec3&);
    Vec3& operator-=(const Vec3&);
    Vec3& operator*=(float);
    Vec3& operator/=(float);

    Vec3 operator+() const;
    Vec3 operator-() const;

    Vec3 operator+(const Vec3&) const;
    Vec3 operator-(const Vec3&) const;
    Vec3 operator*(float) const;
    friend Vec3 operator*(float, const Vec3&);
    Vec3 operator/(float) const;

    bool operator == (const Vec3&) const;
    bool operator != (const Vec3&) const;

    Vec3(const Vec2& v);
    // VECTOR3��VECTOR2����
    Vec3& operator=(const Vec2&);
};

class Vec4 : public DirectX::XMFLOAT4
{
public:
    Vec4() : DirectX::XMFLOAT4(0, 0, 0, 0) {}
    Vec4(float x, float y, float z, float w) : DirectX::XMFLOAT4(x, y, z, w) {}
    Vec4(DirectX::XMFLOAT4 xyzw) : DirectX::XMFLOAT4(xyzw) {}
    ~Vec4() {}

    const DirectX::XMFLOAT4& XMFLOAT() const { return DirectX::XMFLOAT4(this->x, this->y, this->z, this->w); }
};

class VecMath
{
public:

    // ����
    static float Dot(Vec3 v1, Vec3 v2);
    // �O��
    static Vec3 Cross(Vec3 v1, Vec3 v2);
    // v1���n�ʁAv2�����́i�e�����j
    static Vec3 Projection(Vec3 v1, Vec3 v2);
    // �x�N�g���Ԃ̒���
    static float LengthVec3(Vec3 v1, bool SQ = false);
    // ���K��
    static Vec3 Normalize(Vec3 v1);
    // �x�N�g���̈����Z
    static Vec3 Subtract(Vec3 v1, Vec3 v2);
    // �x�N�g���Ԃ̊p�x
    static float AngleBetweenVectors(Vec3 v1, Vec3 v2);
    // ������Ԃ� -:-1   0:0   +:1
    static float sign(float num);

    // �x�N�g��v�ɑ΂��ă|���S�����\���ǂ���������������߂�
    // �߂�l    0:�\    1:��    -1:�G���[
    //static bool PolygonSideCk(Vec3 v, Vec3);
};