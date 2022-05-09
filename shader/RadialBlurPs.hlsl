#include "RadialBlur.hlsli"
#include "Constants.hlsli"
SamplerState sampler_states : register(s0);
Texture2D texture_map : register(t0);
float4 main(VS_OUT In) : SV_TARGET
{
    //１テクセルの大きさをセット
    //float m_TU = 1.0f / 1280.0f;
    //float m_TV = 1.0f / 720.0f;
    //float m_BlurPower = 10.0f;
    float2 m_CenterTexel = (0.5f, 0.5f);
    float4 Color[10];
   
   //ブラーの中心位置 ← 現在のテクセル位置
    float2 dir = m_CenterTexel - In.texcoord;
   
   //距離を計算する
    float len = length(dir);
   
   //方向ベクトルの正規化し、１テクセル分の長さとなる方向ベクトルを計算する
    dir = normalize(dir) * float2(TU, TV);
   
   //m_BlurPower でボケ具合を調整する
   //距離を積算することにより、爆発の中心位置に近いほどブラーの影響が小さくなるようにする
    dir *= BlurPower * len;
   //合成する
    //return myTexture.Sample(mySampler, pin.texcoord) * pin.color;
    Color[0] = texture_map.Sample(sampler_states, In.texcoord) * 0.19;
    Color[1] = texture_map.Sample(sampler_states, In.texcoord + dir) * 0.17;
    Color[2] = texture_map.Sample(sampler_states, In.texcoord + dir * 2.0f) * 0.15;
    Color[3] = texture_map.Sample(sampler_states, In.texcoord + dir * 3.0f) * 0.13;
    Color[4] = texture_map.Sample(sampler_states, In.texcoord + dir * 4.0f) * 0.11;
    Color[5] = texture_map.Sample(sampler_states, In.texcoord + dir * 5.0f) * 0.09;
    Color[6] = texture_map.Sample(sampler_states, In.texcoord + dir * 6.0f) * 0.07;
    Color[7] = texture_map.Sample(sampler_states, In.texcoord + dir * 7.0f) * 0.05;
    Color[8] = texture_map.Sample(sampler_states, In.texcoord + dir * 8.0f) * 0.03;
    Color[9] = texture_map.Sample(sampler_states, In.texcoord + dir * 9.0f) * 0.01;
    return Color[0] + Color[1] + Color[2] + Color[3] + Color[4] + Color[5] + Color[6] + Color[7] + Color[8] + Color[9];
}