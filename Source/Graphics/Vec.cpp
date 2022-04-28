#include "Vec.h"

Vec2& Vec2::operator=(const Vec2& v)
{
    x = v.x;
    y = v.y;
    return *this;
}

Vec2& Vec2::operator+=(const Vec2& v)
{
    x += v.x;
    y += v.y;
    return *this;
}

Vec2& Vec2::operator-=(const Vec2& v)
{
    x -= v.x;
    y -= v.y;
    return *this;
}

Vec2& Vec2::operator*=(float f)
{
    x *= f;
    y *= f;
    return *this;
}

Vec2& Vec2::operator/=(float f)
{
    x /= f;
    y /= f;
    return *this;
}

Vec2 Vec2::operator+() const
{
    return Vec2(x, y);
}

Vec2 Vec2::operator-() const
{
    return Vec2(-x, -y);
}

Vec2 Vec2::operator+(const Vec2& v) const
{
    return Vec2(x + v.x, y + v.y);
}

Vec2 Vec2::operator-(const Vec2& v) const
{
    return Vec2(x - v.x, y - v.y);
}

Vec2 Vec2::operator*(float f) const
{
    return Vec2(x * f, y * f);
}

Vec2 operator*(float f, const Vec2& v)
{
    return Vec2(v.x * f, v.y * f);
}

Vec2 Vec2::operator/(float f) const
{
    return Vec2(x / f, y / f);
}

bool Vec2::operator==(const Vec2& v) const
{
    return (x == v.x) && (y == v.y);
}

bool Vec2::operator != (const Vec2& v) const
{
    return (x != v.x) || (y != v.y);
}






Vec3& Vec3::operator=(const Vec3& v)
{
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}

Vec3& Vec3::operator+=(const Vec3& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

Vec3& Vec3::operator-=(const Vec3& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

Vec3& Vec3::operator*=(float f)
{
    x *= f;
    y *= f;
    z *= f;
    return *this;
}

Vec3& Vec3::operator/=(float f)
{
    x /= f;
    y /= f;
    z /= f;
    return *this;
}

Vec3 Vec3::operator+() const
{
    return Vec3(x, y, z);
}

Vec3 Vec3::operator-() const
{
    return Vec3(-x, -y, -z);
}

Vec3 Vec3::operator+(const Vec3& v) const
{
    return Vec3(x + v.x, y + v.y, z + v.z);
}

Vec3 Vec3::operator-(const Vec3& v) const
{
    return Vec3(x - v.x, y - v.y, z - v.z);
}

Vec3 Vec3::operator*(float f) const
{
    return Vec3(x * f, y * f, z * f);
}

Vec3 operator*(float f, const Vec3& v)
{
    return Vec3(v.x * f, v.y * f, v.z * f);
}

Vec3 Vec3::operator/(float f) const
{
    return Vec3(x / f, y / f, z / f);
}

bool Vec3::operator==(const Vec3& v) const
{
    return (x == v.x) && (y == v.y) && (z == v.z);
}

bool Vec3::operator!=(const Vec3& v) const
{
    return (x != v.x) || (y != v.y) || (z != v.z);
}

Vec3::Vec3(const Vec2& v)
{
    x = v.x;
    y = v.y;
    z = 0;
}

Vec3& Vec3::operator=(const Vec2& v)
{
    x = v.x;
    y = v.y;
    z = 0;
    return *this;
}

float VecMath::Dot(Vec3 v1, Vec3 v2)
{
    DirectX::XMVECTOR V1 = DirectX::XMLoadFloat3(&v1);
    DirectX::XMVECTOR V2 = DirectX::XMLoadFloat3(&v2);
    DirectX::XMVECTOR D = DirectX::XMVector3Dot(V1, V2);
    float dot;
    DirectX::XMStoreFloat(&dot, D);
    return dot;
}

Vec3 VecMath::Cross(Vec3 v1, Vec3 v2)
{
    DirectX::XMVECTOR V1 = DirectX::XMLoadFloat3(&v1);
    DirectX::XMVECTOR V2 = DirectX::XMLoadFloat3(&v2);
    DirectX::XMVECTOR C = DirectX::XMVector3Cross(V1, V2);
    Vec3 cross; 
    DirectX::XMStoreFloat3(&cross, C);
    return cross;
}

Vec3 VecMath::Projection(Vec3 v1, Vec3 v2)
{
    // ³‹K‰»‚µ‚½v1‚Æv2‚Ì“àÏ‚ÉAv1‚ğ‘‚¯‚é
    DirectX::XMVECTOR V1 = DirectX::XMLoadFloat3(&v1);
    DirectX::XMVECTOR V2 = DirectX::XMLoadFloat3(&v2);
    V1 = DirectX::XMVector3Normalize(V1);
    DirectX::XMVECTOR D = DirectX::XMVector3Dot(V1, V2);
    float dot;
    DirectX::XMStoreFloat(&dot, D);

    Vec3 projection;
    DirectX::XMStoreFloat3(&projection,DirectX::XMVectorScale(V1, dot));
    return projection;
}

float VecMath::LengthVec3(Vec3 v1, bool SQ)
{
    DirectX::XMVECTOR V1 = DirectX::XMLoadFloat3(&v1);
    DirectX::XMVECTOR LEN;
    if (SQ) LEN = DirectX::XMVector3LengthSq(V1);
    else LEN = DirectX::XMVector3Length(V1);
    float length;
    DirectX::XMStoreFloat(&length, LEN);

    return length;
}

Vec3 VecMath::Normalize(Vec3 v1)
{
    DirectX::XMVECTOR V1 = DirectX::XMLoadFloat3(&v1); 
    Vec3 normalize;
    DirectX::XMStoreFloat3(&normalize,DirectX::XMVector3Normalize(V1));
    return normalize;
}

Vec3 VecMath::Subtract(Vec3 v1, Vec3 v2)
{
    DirectX::XMVECTOR V1 = DirectX::XMLoadFloat3(&v1);
    DirectX::XMVECTOR V2 = DirectX::XMLoadFloat3(&v2);
    Vec3 sub;
    DirectX::XMStoreFloat3(&sub,DirectX::XMVectorSubtract(V1, V2));
    return sub;
}

float VecMath::AngleBetweenVectors(Vec3 v1, Vec3 v2)
{
    DirectX::XMVECTOR V1 = DirectX::XMLoadFloat3(&v1);
    DirectX::XMVECTOR V2 = DirectX::XMLoadFloat3(&v2);
    float ang;
    DirectX::XMStoreFloat(&ang, DirectX::XMVector3AngleBetweenVectors(V1, V2));
    return ang;
}

int VecMath::sign(float num)
{
    return (num > 0) - (num < 0);
}
