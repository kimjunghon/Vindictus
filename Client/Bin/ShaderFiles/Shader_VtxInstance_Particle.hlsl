#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_DiffuseTexture;

float3 g_vSourceColor = float3(1.f, 1.f, 1.f);

float g_Radius = 10.f;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
    
    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook : TEXCOORD3;
    float4 vTranslation : TEXCOORD4;
    float2 vLifeTime : TEXCOORD5;
};

struct VS_DEFAULT_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};


VS_DEFAULT_OUT VS_MAIN(VS_IN In)
{
    VS_DEFAULT_OUT Out = (VS_DEFAULT_OUT) 0;

    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    float4x4 TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vLifeTime = In.vLifeTime;
    
    return Out;
}

VS_DEFAULT_OUT VS_RING(VS_IN In)
{
    VS_DEFAULT_OUT Out = (VS_DEFAULT_OUT) 0;
    
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    float fRatio = saturate(In.vLifeTime.x / In.vLifeTime.y);
    
    float4 vRight = In.vRight + (In.vRight * (g_Radius * fRatio));
    float4 vUp = In.vUp + (In.vUp * (g_Radius * fRatio));
    
    float4x4 TransformMatrix = float4x4(vRight, vUp, In.vLook, In.vTranslation);
    
    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
    
    float2x2 Rotation = float2x2(cos(fRatio), -sin(fRatio),
                                sin(fRatio), cos(fRatio));
    
    float2 RotateXY = mul(vPosition.xy, Rotation);
    vPosition.xy = RotateXY;
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vLifeTime = In.vLifeTime;
    
    return Out;
}

struct PS_DEFAULT_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};


PS_OUT PS_MAIN(PS_DEFAULT_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    
    if(Out.vColor.a <= 0.3f)
        discard;
    
    float fAlpha = saturate(In.vLifeTime.y - In.vLifeTime.x);
    
    Out.vColor.a = fAlpha;
    
    if (Out.vColor.a <= 0.f)
        discard;
    
    return Out;
}

PS_OUT PS_RING(PS_DEFAULT_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMask = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    
    vector vSourColor = 1.f;
    
    Out.vColor = vSourColor * vMask;
    
    Out.vColor.a = 1.f * vMask.x;
    
    if (Out.vColor.a <= 0.3f)
        discard;
    
    float fAlpha = saturate(In.vLifeTime.y - In.vLifeTime.x);
    
    Out.vColor.a = fAlpha;
    
    if(Out.vColor.a <= 0.f)
        discard;
    
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

    pass RingPass
    {
        SetRasterizerState(RS_CULL_NONE);
        SetDepthStencilState(DSS_DEFAULT, 0);
        SetBlendState(BS_ALPHABLEND, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_RING();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RING();
    }
}