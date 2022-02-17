#include "Grass.hlsli"
#include "Constants.hlsli"

VS_OUT main(
    float4 position : POSITION,
    float4 normal : NORMAL,
    float4 tangent : TANGENT,
    float2 texcoord : TEXCOORD,
    float4 boneWeights : WEIGHTS,
    uint4 boneIndices : BONES
)
{    
    VS_OUT vout;
    // ワールド、ビュー、射影変換
    vout.position = mul(position, world); //geometryでviewprojection
    
    //法線のワールドトランスフォーム
    float4 worldNormal = normalize(mul(normal, world));
    //色の調整
    {
        //トランスフォームされた法線とライトの正規化
        float3 N = normalize(worldNormal.xyz);
        float3 L = normalize(-lightDirection.xyz);
    
        //法線とライトの方向の内積が、頂点が受けるライトの強さ
        float lightPower = dot(N, L);
        //ライトが負にならないように + 環境光計算
        lightPower = max(0, lightPower) * 0.5f + 0.5f;
        //ライトの色とライトの強さで色を調整
        float4 color = (1, 1, 1, 1);
        vout.color.rgb = color.rgb * materialColor.rgb * lightPower;
        vout.color.a = color.a * materialColor.a;
    }
    //受け取ったテクスチャの頂点座標をそのまま出力
    vout.normal = normal;
    vout.tangent = tangent;
    
    return vout;
}