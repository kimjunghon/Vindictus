#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float3 g_vSourceColor = float3(1.f, 1.f, 1.f);

texture2D g_Texture;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
     
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vLifeTime = In.vLifeTime;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;

};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
    
};

struct PS_EMISSIVE_OUT
{
    float4 vEmissiveColor : SV_TARGET0;
    float4 vBackBufferColor : SV_TARGET1;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMask = g_Texture.Sample(PointSampler, In.vTexcoord);
    
    vector vSourColor = float4(g_vSourceColor, 1.f) * vMask;
    
    Out.vColor = vSourColor * vMask;
    
    Out.vColor.a = 1.f * vMask.x;
    
    float fAlpha = (In.vLifeTime.x / In.vLifeTime.y);
    
    Out.vColor.a -= fAlpha;
    
    if (Out.vColor.a <= 0.f)
        discard;
    
    return Out;
}


PS_EMISSIVE_OUT PS_EMISSIVE(PS_IN In)
{
    PS_EMISSIVE_OUT Out = (PS_EMISSIVE_OUT) 0;
    
    vector vMask = g_Texture.Sample(PointSampler, In.vTexcoord);
    
    vector vSourColor = float4(g_vSourceColor, 1.f) * vMask;
    
    vector vFinalColor = vSourColor * vMask;
    
    vFinalColor.a = 1.f * vMask.x;
    
    float fDecreaseAlpha = (In.vLifeTime.x / In.vLifeTime.y);
    
    vFinalColor.a -= fDecreaseAlpha;
    
    if (vFinalColor.a <= 0.f)
        discard;
    
    Out.vBackBufferColor = vFinalColor;
    
    float fLuminance = Luminance(vFinalColor.rgb);
    
    vector vEmissiveColor = 0.f;
    
    if(fLuminance > 0.4)
    {
      vEmissiveColor = vector(vFinalColor.rgb * 3.f, vFinalColor.a);
    }

    Out.vEmissiveColor = vEmissiveColor;
  
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_CULL_NONE);
        SetDepthStencilState(DSS_DEFAULT, 0);
        SetBlendState(BS_ALPHABLEND, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass EmissivePass
    {
        SetRasterizerState(RS_CULL_NONE);
        SetDepthStencilState(DSS_DEFAULT, 0);
        SetBlendState(BS_ALPHABLEND, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EMISSIVE();
    }


}
