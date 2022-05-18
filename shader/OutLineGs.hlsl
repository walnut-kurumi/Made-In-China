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
    
    // �|���S���̕Ӄx�N�g�����v�Z���A�|���S���̖@�����v�Z����B
    float3 normal = ConstructNormal(input[0].position.xyz, input[1].position.xyz, input[2].position.xyz);   

    // �R�[�h�����[�v����Ȃ���ԂɓW�J���邱�Ƃ𖾎�����i�ڍׂ͉��̋L�����Q�Ɓj
    [unroll]
    for (int i = 0; i < 3; i++)
    {
        GS_IN v = input[i];
        GS_OUT o;
        
        // �ȉ��ł́A�e�v�f�i�ʒu�A��]�A�X�P�[���j�ɑ΂��ČW���ɉ����ĕω���^���܂��B
       
         // center�ʒu���N�_�ɃX�P�[����ω������܂��B
        v.position.xyz = (v.position.xyz - center) * 1.0f + center;

        // �@�������Ɉʒu��ω������܂�
        v.position.xyz += normal* 0.04f;

        // �Ō�ɁA�C���������_�ʒu���ˉe�ϊ��������_�����O�p�ɕϊ����܂��B
        o.position = mul(v.position, viewProjection);
        o.color = v.color;        
        o.texcoord = v.texcoord;
        
        output.Append(o);
    }

    output.RestartStrip();
}