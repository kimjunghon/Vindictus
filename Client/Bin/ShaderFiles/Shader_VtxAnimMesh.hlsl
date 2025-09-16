#include "Engine_Shader_Defines.hlsli"

float4x4    g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_DiffuseTexture;
texture2D g_NormalTexture;

float3 g_vColor_R;
float3 g_vColor_G;
float3 g_vColor_B;

bool g_HasNormal = false;

matrix g_BoneMatrices[512];

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal   : NORMAL;
    float3 vTangent  : TANGENT;
    float3 vBinormal : BINORMAL;
    uint4  vBlendIndex : BLENDINDEX;
    float4 vBlendWeight : BLENDWEIGHT;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
    
    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
                        g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
                        g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
                        g_BoneMatrices[In.vBlendIndex.w] * fWeightW;
    

    vector vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);
    
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = normalize(mul(float4(In.vBinormal, 0.f), g_WorldMatrix));
    
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if(vDiffuse.a < 0.3f)
        discard;
    
    Out.vDiffuse = vDiffuse;
    
    float3 vNormal;
    if (g_HasNormal)
    {
        vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
        vNormal = mul(vNormal, WorldMatrix);
    }
    else
        vNormal = In.vNormal.xyz;
    
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    
    Out.vDepth = vector((In.vProjPos.z / In.vProjPos.w), In.vProjPos.w, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_COLOR_MASKING(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vDiffuseR = vDiffuse.r;
    vector vDiffuseG = vDiffuse.g;
    vector vDiffuseB = vDiffuse.b;
    
    vDiffuse = float4((vDiffuseR * g_vColor_R) + (vDiffuseG * g_vColor_G) + (vDiffuseB * g_vColor_B), 1.f);
    
    
    Out.vDiffuse = vDiffuse;
    
    float3 vNormal;
    if (g_HasNormal)
    {
        vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
        vNormal = mul(vNormal, WorldMatrix);
    }
    else
        vNormal = In.vNormal.xyz;
    
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    
    Out.vDepth = vector((In.vProjPos.z / In.vProjPos.w), In.vProjPos.w, 0.f, 0.f);
    
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

    pass WirePass
    {
        SetRasterizerState(RS_WIRE);
        SetDepthStencilState(DSS_DEFAULT, 0);
        SetBlendState(BS_DEFAULT, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass ColorMaskingPass
    {
        SetRasterizerState(RS_DEFAULT);
        SetDepthStencilState(DSS_DEFAULT, 0);
        SetBlendState(BS_DEFAULT, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR_MASKING();
    }
}