#include "Outline.hlsli"
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
    normal.w = 0;
    float4 blendedPosition = { 0, 0, 0, 1 };
    float4 blendedNormal = { 0, 0, 0, 0 };
    for (int boneIndex = 0; boneIndex < 4; ++boneIndex)
    {
        blendedPosition += mul(boneWeights[boneIndex], mul(position, boneTransforms[boneIndices[boneIndex]]));
        blendedNormal += mul(boneWeights[boneIndex], mul(float4(normal.xyz, 0.0f), boneTransforms[boneIndices[boneIndex]]));
    }
    position = float4(blendedPosition.xyz, 1.0f);
    normal = float4(blendedNormal.xyz, 0.0f);

    VS_OUT vout;
    // ���[���h�A�r���[�A�ˉe�ϊ�
    vout.position = mul(position, world); //geometry��viewprojection
    
    //�@���̃��[���h�g�����X�t�H�[��
    float4 worldNormal = normalize(mul(normal, world));
    //�F�̒���
    {
        vout.color = materialColor;
    }
        
    //�󂯎�����e�N�X�`���̒��_���W�����̂܂܏o��
    vout.texcoord = texcoord;
    
    return vout;
}