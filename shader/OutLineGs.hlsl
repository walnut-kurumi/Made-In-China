#include "OutLine.hlsli"
#include "Constants.hlsli"

float3 ConstructNormal(float3 p0, float3 p1, float3 p2)
{
    float3 v1 = p1 - p0;
    float3 v2 = p2 - p1;
    return normalize(cross(v1, v2));
}

[maxvertexcount(3)]
void main(
	triangle GS_IN input[3] : SV_POSITION, 
	inout TriangleStream< GS_OUT > output
)
{
    float3 center = (input[0].position + input[1].position + input[2].position) / 3;
    
    // ポリゴンの辺ベクトルを計算し、ポリゴンの法線を計算する。
    float3 normal = ConstructNormal(input[0].position.xyz, input[1].position.xyz, input[2].position.xyz);   

    // コードをループじゃない状態に展開することを明示する（詳細は下の記事を参照）
    [unroll]
    for (int i = 0; i < 3; i++)
    {
        GS_IN v = input[i];
        GS_OUT o;
        
        // 以下では、各要素（位置、回転、スケール）に対して係数に応じて変化を与えます。
       
         // center位置を起点にスケールを変化させます。
        v.position.xyz = (v.position.xyz - center) * 1.0f + center;

        // 法線方向に位置を変化させます
        v.position.xyz += normal* 0.04f;

        // 最後に、修正した頂点位置を射影変換しレンダリング用に変換します。
        o.position = mul(v.position, viewProjection);
        o.color = v.color;        
        o.texcoord = v.texcoord;
        
        output.Append(o);
    }

    output.RestartStrip();
}