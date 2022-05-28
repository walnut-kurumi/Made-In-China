#include "SkinnedMesh.hlsli"
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
    // �@�������Ɋg��(�c���)
    float4 N = normalize(normal);
    
    position += N * 0.2f;
    for (int boneIndex = 0; boneIndex < 4; ++boneIndex)
    {
        blendedPosition += mul(boneWeights[boneIndex], mul(position, boneTransforms[boneIndices[boneIndex]]));
        blendedNormal += mul(boneWeights[boneIndex], mul(float4(normal.xyz, 0.0f), boneTransforms[boneIndices[boneIndex]]));
    }
    position = float4(blendedPosition.xyz, 1.0f);
    normal = float4(blendedNormal.xyz, 0.0f);
    
    VS_OUT vout;
    // ���[���h�A�r���[�A�ˉe�ϊ�
    vout.position = mul(position, mul(world, viewProjection));
    
    
    //�@���̃��[���h�g�����X�t�H�[��
    float4 worldNormal = normalize(mul(normal, world));
    //�F�̒���
    {
        //�g�����X�t�H�[�����ꂽ�@���ƃ��C�g�̐��K��
        float3 N = normalize(worldNormal.xyz);
        float3 L = normalize(-lightDirection.xyz);
    
        //�@���ƃ��C�g�̕����̓��ς��A���_���󂯂郉�C�g�̋���
        float lightPower = dot(N, L);
        //���C�g�����ɂȂ�Ȃ��悤�� + �����v�Z
        lightPower = max(0, lightPower) * 0.5f + 0.5f;
        //���C�g�̐F�ƃ��C�g�̋����ŐF�𒲐�
        float4 color = (1, 1, 1, 1);
        vout.color.rgb = color.rgb * materialColor.rgb * lightPower;
        vout.color.a = color.a * materialColor.a;
    }
    //�󂯎�����e�N�X�`���̒��_���W�����̂܂܏o��
    vout.texcoord = texcoord;

    return vout;
}