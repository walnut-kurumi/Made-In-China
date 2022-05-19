#include "Constants.hlsli"
#include "trail.hlsli"
VS_OUT main(float4 position : POSITION)
{
    VS_OUT vout;
    // ワールド、ビュー、射影変換
    vout.position = mul(position,vp);
	
	
    return vout;
}