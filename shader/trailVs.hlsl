#include "Constants.hlsli"
#include "trail.hlsli"
VS_OUT main(float4 position : POSITION)
{
    VS_OUT vout;
    // ���[���h�A�r���[�A�ˉe�ϊ�
    vout.position = mul(position,vp);
	
	
    return vout;
}