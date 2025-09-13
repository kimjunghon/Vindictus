#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float g_fTime;

texture2D g_Texture;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
     
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;

};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
    
};


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 Test = float2(0.1f, 0.1f);
    float2 St = In.vTexcoord / Test;
    
    float3 vColor = 0.f;
    
    float2 q = 0.f;
    q.x = FBM(St);
    q.y = FBM(St + float2(1.0, 1.0));
    
    float2 r = 0.f;
    r.x = FBM(St + (q * 1.0) + float2(1.7f, 9.2f) + 0.15 * g_fTime);
    r.y = FBM(St + (q * 1.0) + float2(8.3, 2.8) + 0.126 * g_fTime);
    
    float f = FBM(St + r);
    
    vColor = lerp(float3(0.101961, 0.619608, 0.666667), float3(0.666667, 0.666667, 0.698039), clamp((f * f) * 4.0, 0.0, 1.0));
    
    vColor = lerp(vColor, float3(0.f, 0.f, 0.164706), clamp(length(q), 0.f, 1.f));
    
    vColor = lerp(vColor, float3(0.666667, 1.f, 1.f), clamp(length(r.x), 0.f, 1.f));
    
    Out.vColor = float4((f * f * f + 0.6f * f * f + 0.5f * f)*vColor, 1.f); //g_Texture.Sample(PointSampler, In.vTexcoord);

    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_DEFAULT);
        SetDepthStencilState(DSS_DEFAULT, 0);
        SetBlendState(BS_DEFAULT, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}
