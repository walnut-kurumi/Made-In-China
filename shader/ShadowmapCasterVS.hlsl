#include "Ramp.hlsli"
#include "Constants.hlsli"

float4 main(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD) : SV_POSITION
{
    return mul(position, mul(world, viewProjection));
}
