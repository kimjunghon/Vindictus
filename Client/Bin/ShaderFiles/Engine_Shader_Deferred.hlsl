#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
matrix g_ShadowLightViewMatrix, g_ShadowLightProjMatrix;
float g_fShadowLightFar;

texture2D g_Texture;

vector g_vCamPosition;

vector g_vLightDir;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

vector g_vMatrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vMatrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_DepthTexture;
texture2D g_ShadeTexture;
texture2D g_SpecularTexture;
texture2D g_LightDepthTexture;

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

struct PS_OUT_BACKBUFFER
{
    float4 vColor : SV_TARGET0;
};

struct PS_OUT_LIGHT
{
    vector vShade : SV_TARGET0;
    vector vSpecular : SV_TARGET1;
};

PS_OUT_BACKBUFFER PS_DEBUG(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;
}

PS_OUT_LIGHT PS_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;
    
    vector vNormalData = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormal = normalize(vector((vNormalData.xyz * 2.f) - 1.f, 0.f));
    vector vLightDir = normalize(g_vLightDir);
    
    float fShade = max(dot(vNormal * -1.f, vLightDir), 0.f);
    
    vector vReflect = reflect(vLightDir, vNormal);
    
    vector vDepthData = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vWorldPos;
    
    vWorldPos.x = (In.vTexcoord.x * 2.f) - 1.f;
    vWorldPos.y = (In.vTexcoord.y * -2.f) + 1.f;
    vWorldPos.z = vDepthData.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * vDepthData.y;
    
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    vector vLook = normalize(vWorldPos - g_vCamPosition);
    
    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, vLook), 0.f), 20.f);
    
    Out.vShade = g_vLightDiffuse * saturate(fShade + (g_vLightAmbient * g_vMatrlAmbient));
    Out.vSpecular = (g_vLightSpecular * g_vMatrlSpecular) * fSpecular;
    
    return Out;
}

PS_OUT_LIGHT PS_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;
    
    return Out;
}

PS_OUT_BACKBUFFER PS_COMBINED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if(vDiffuse.a == 0.f)
        discard;

    vector vShade = g_ShadeTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    
    Out.vColor = (vDiffuse * vShade) + vSpecular;
    
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vWorldPos;
    
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * vDepthDesc.y;
    
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    vector vPosition = mul(vWorldPos, g_ShadowLightViewMatrix);
    vPosition = mul(vPosition, g_ShadowLightProjMatrix);
    
    float2 vTexcoord;
    
 //   vTexcoord.x = (vPosition.x) * 0.5f + 0.5f;
 //   vTexcoord.y = (vPosition.y) * -0.5f + 0.5f;
    
    vTexcoord.x = (vPosition.x / vPosition.w) * 0.5f + 0.5f;
    vTexcoord.y = (vPosition.y / vPosition.w) * -0.5f + 0.5f;
    
    vector vLightDepth = g_LightDepthTexture.Sample(DefaultSampler, vTexcoord);
    float fViewZ = vLightDepth.x * 1000.f;// * g_fShadowLightFar;
    
    float fDistance = vPosition.w - fViewZ;
    
    if(fDistance > 0.2f)
        Out.vColor = Out.vColor * 0.3f;

//    if (vPosition.z - 0.0001f > fViewZ)
//        Out.vColor = Out.vColor * 0.3f;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Debug
    {
        SetRasterizerState(RS_DEFAULT);
        SetDepthStencilState(DSS_DEFAULT, 0);
        SetBlendState(BS_DEFAULT, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DEBUG();
    }

    pass Directional
    {
        SetRasterizerState(RS_DEFAULT);
        SetDepthStencilState(DSS_ZDISABLE, 0);
        SetBlendState(BS_DEFAULT, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DIRECTIONAL();
    }
    
    pass Point
    {
        SetRasterizerState(RS_DEFAULT);
        SetDepthStencilState(DSS_ZDISABLE, 0);
        SetBlendState(BS_DEFAULT, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_POINT();
    }
    
    pass Combined
    {
        SetRasterizerState(RS_DEFAULT);
        SetDepthStencilState(DSS_ZDISABLE, 0);
        SetBlendState(BS_DEFAULT, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_COMBINED();
    }
}
