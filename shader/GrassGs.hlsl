#include "Grass.hlsli"
#include "Constants.hlsli"

float3 ConstructNormal(float3 p0, float3 p1, float3 p2)
{
    float3 v1 = p1 - p0;
    float3 v2 = p2 - p1;
    return normalize(cross(v1, v2));
}

inline float Rand(float3 seed)
{
    return frac(sin(dot(seed.xyz, float3(12.9898f, 78.233f, 53.539f))) * 43758.5453f);
}

float3x3 AngleAxis3x3(float angle, float3 axis)
{
    float c, s;
    sincos(angle, s, c);

    float t = 1 - c;
    float x = axis.x;
    float y = axis.y;
    float z = axis.z;
    
    // 軸に沿った回転行列作成
    return float3x3(
			t * x * x + c, t * x * y - s * z, t * x * z + s * y,
			t * x * y + s * z, t * y * y + c, t * y * z - s * x,
			t * x * z - s * y, t * y * z + s * x, t * z * z + c
			);
}

GS_OUT VertexOutput(float3 pos, float2 uv)
{
    GS_OUT o;

    o.position = mul(float4(pos, 1), viewProjection);
	o.texcoord = uv;

    return o;
}

GS_OUT GenerateGrassVertex(float3 vertexPosition, float width, float height, float forward, float2 uv, float3x3 transformMatrix)
{
    float3 tangentPoint = float3(width, height, forward);

    float3 localPosition = vertexPosition + mul(transformMatrix, tangentPoint);
    return VertexOutput(localPosition, uv);
}


#define BLADE_SEGMENTS 3
#define PI     3.141592653589f
#define TWO_PI 6.283185307178f



[maxvertexcount(BLADE_SEGMENTS * 2 + 1)]
void main(
	triangle GS_IN input[3] : SV_POSITION,
	inout TriangleStream<GS_OUT> output
)
{
    GS_OUT o;
    float4 pos = input[0].position;
	float4 normal = input[0].normal;                                //法線 
	float4 tangent = input[0].tangent;                              //接線
    float3 binormal = cross(normal.xyz, tangent.xyz) * tangent.w;   //接線part2
    
    // 行列作成
    // 地形に沿った回転行列
    float3x3 tangentToLocal = float3x3(
	    tangent.x, binormal.x, normal.x,
	    tangent.y, binormal.y, normal.y,
	    tangent.z, binormal.z, normal.z
	);    
    // ランダムな向き行列
    float3x3 facingRotationMatrix = AngleAxis3x3(Rand(pos.xyz) * TWO_PI, float3(0, 1, 0));
    // 倒れる回転行列
    float3x3 bendRotationMatrix = AngleAxis3x3(Rand(pos.zyx) * bendRotation * PI * 0.5, float3(-1, 0, 0));
    // 接空間行列と向き行列
    float3x3 transformationMatrix = mul(mul(tangentToLocal, facingRotationMatrix), bendRotationMatrix);
    float3x3 transformationMatrixFacing = mul(tangentToLocal, facingRotationMatrix);
	//幅高さの作成
    float width = (Rand(pos.xzy) * 2 - 1) * widthFactor + 0.05f;
    float height = (Rand(pos.zyx) * 2 - 1) * heightFactor + 0.5f;
    //曲げる率
    float forward = Rand(pos.yxz) * grassForward;
    float4 color = input[0].color;    
    
    //見やすくするための仮の幅高さ
    //width = 1;
    //height = 1;
    
    for (int i = 0; i < BLADE_SEGMENTS; i++)
    {
        //縦の分割の割合
        float t = i / (float) BLADE_SEGMENTS;
            
        float segmentHeight = height * t;
        float segmentWidth = width * (1 - t);
        float segmentForward = pow(t, grassCurve) * forward;
        
        float3x3 transformMatrix = i == 0 ? transformationMatrixFacing : transformationMatrix;
        //縦の分割分 両サイドに頂点を設定
        output.Append(GenerateGrassVertex(pos.xyz, segmentWidth, segmentHeight, segmentForward, float2(0, t), transformMatrix));
        output.Append(GenerateGrassVertex(pos.xyz, -segmentWidth, segmentHeight, segmentForward, float2(1, t), transformMatrix));
    }
    // 最後に天辺の頂点
    output.Append(GenerateGrassVertex(pos.xyz, 0, height, forward, float2(0.5, 1), transformationMatrix));
    
    //o.position = float4(pos.xyz + mul(transformationMatrix, float3(width, 0, 0)), 1);
    //o.position = mul(o.position, viewProjection);
    //o.color = input[0].color;
    //o.texcoord = float2(1, 0);
    //output.Append(o); 
    //o.position = float4(pos.xyz + mul(transformationMatrix, float3(-width, 0, 0)), 1);
    //o.position = mul(o.position, viewProjection);
    //o.color = input[1].color;
    //o.texcoord = float2(0, 0);
    //output.Append(o);    
    //o.position = float4(pos.xyz + mul(transformationMatrix, float3(0, height, 0)), 1);
    //o.position = mul(o.position, viewProjection);
    //o.color = input[2].color;
    //o.texcoord = float2(0.5, 1);
    //output.Append(o);
}