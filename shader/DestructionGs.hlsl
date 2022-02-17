#include "Destruction.hlsli"
#include "Constants.hlsli"

float3 ConstructNormal(float3 p0, float3 p1, float3 p2)
{
    float3 v1 = p1 - p0;
    float3 v2 = p2 - p1;
    return normalize(cross(v1, v2));
}

float3 Rotate(float3 p, float3 rotation)
{
    float3 a = normalize(rotation);
    float angle = length(rotation);
    if (abs(angle) < 0.001)
        return p;
    float s = sin(angle);
    float c = cos(angle);
    float r = 1.0 - c;
    float3x3 m = float3x3(
        a.x * a.x * r + c,
        a.y * a.x * r + a.z * s,
        a.z * a.x * r - a.y * s,
        a.x * a.y * r - a.z * s,
        a.y * a.y * r + c,
        a.z * a.y * r + a.x * s,
        a.x * a.z * r + a.y * s,
        a.y * a.z * r - a.x * s,
        a.z * a.z * r + c
    );
    return mul(m, p);
}

inline float Rand(float2 seed)
{
    return frac(sin(dot(seed.xy, float2(12.9898f, 78.233f))) * 43758.5453f);
}

[maxvertexcount(3)]
void main(
	triangle GS_IN input[3] : SV_POSITION,
	inout TriangleStream<GS_OUT> output
)
{
    float3 center = (input[0].position + input[1].position + input[2].position) / 3;
    
    // ポリゴンの辺ベクトルを計算し、ポリゴンの法線を計算する。
    float3 normal = ConstructNormal(input[0].position.xyz, input[1].position.xyz, input[2].position.xyz);
    float destruction = _Destruction;
    
    // 省略していますが、独自で定義した「rand」関数を使って乱数を生成しています。
    // ここではポリゴン位置などをseedにして乱数を生成しています。
    float r = 2.0 * (Rand(center.xy) - 0.5);
    float3 r3 = r.xxx;
    float3 up = float3(0, 1, 0);

    // コードをループじゃない状態に展開することを明示する（詳細は下の記事を参照）
    [unroll]
    for (int i = 0; i < 3; i++)
    {
        GS_IN v = input[i];
        GS_OUT o;
        

        // 以下では、各要素（位置、回転、スケール）に対して係数に応じて変化を与えます。

        // center位置を起点にスケールを変化させます。
        v.position.xyz = (v.position.xyz - center) * (1.0 - destruction * _ScaleFactor) + center + (up * destruction * _ScaleFactor);

        // center位置を起点に、乱数を用いて回転を変化させます。
        v.position.xyz = Rotate(v.position.xyz - center, r3 * destruction * _RotationFactor) + center;

        // 法線方向に位置を変化させます
        v.position.xyz += normal * destruction * _PositionFactor * r3 * 5;

        // 最後に、修正した頂点位置を射影変換しレンダリング用に変換します。
        o.position = mul(v.position, viewProjection);
        o.color = v.color;
        //o.color.a *= 1.0 - destruction * _AlphaFactor;
        o.texcoord = v.texcoord;
        
        output.Append(o);
    }

    output.RestartStrip();
}