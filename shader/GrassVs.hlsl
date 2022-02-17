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
    // ���[���h�A�r���[�A�ˉe�ϊ�
    vout.position = mul(position, world); //geometry��viewprojection
    
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
    vout.normal = normal;
    vout.tangent = tangent;
    
    return vout;
}