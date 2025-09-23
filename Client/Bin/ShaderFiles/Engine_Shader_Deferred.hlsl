#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
matrix g_ShadowLightViewMatrix, g_ShadowLightProjMatrix;

float g_fShadowLightFar;
float g_fWinSizeX;
float g_fWinSizeY;

texture2D g_Texture;

vector g_vCamPosition;

vector g_vLightDir;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_DepthTexture;
texture2D g_MaterialSpecularTexture;
texture2D g_MaterialAmbientTexture;
texture2D g_ShadeTexture;
texture2D g_SpecularTexture;
texture2D g_LightDepthTexture;
texture2D g_MapLightDepthTexture;
texture2D g_EmissiveTexture;
texture2D g_BlurTexture;
texture2D g_BackBufferTexture;
texture2D g_BlurEndTexture;
texture2D g_DistortionTexture;

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
    vector vMaterialAmbient = g_MaterialAmbientTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vMaterialSpecular = g_MaterialSpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    
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
    
    Out.vShade = g_vLightDiffuse * saturate(fShade + (g_vLightAmbient * vMaterialAmbient));
    Out.vSpecular = (g_vLightSpecular * vMaterialSpecular) * fSpecular;
    
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
    
    vTexcoord.x = (vPosition.x) * 0.5f + 0.5f;
    vTexcoord.y = (vPosition.y) * -0.5f + 0.5f;
    
 //   vTexcoord.x = (vPosition.x / vPosition.w) * 0.5f + 0.5f;
 //   vTexcoord.y = (vPosition.y / vPosition.w) * -0.5f + 0.5f;
    
    vector vLightDepth = g_LightDepthTexture.Sample(DefaultSampler, vTexcoord);
    float fViewZ = vLightDepth.x;// * g_fShadowLightFar;
    float fDistance = vPosition.z - fViewZ;
    
    vector vMapLightDepth = g_MapLightDepthTexture.Sample(DefaultSampler, vTexcoord);
    float fMapViewZ = vMapLightDepth.x;
    float fMapDistance = vPosition.z - fMapViewZ;
    
    if (fDistance > 0.00291123156f || fMapDistance > 0.02123156f)
        Out.vColor = Out.vColor * 0.5f;

//    if (vPosition.z - 0.0001f > fViewZ)
//        Out.vColor = Out.vColor * 0.3f;
    
    return Out;
}

float g_fWeights[13] =
{
    0.020597f, 0.037981f, 0.062950f, 0.093995f, 0.127324f, 0.153170f, 0.163967f, 0.153170f, 0.127324f, 0.093995f, 0.062950f, 0.037981f, 0.020597f
};

struct PS_OUT_BLUR
{
    vector vBlur : SV_TARGET0;
};

PS_OUT_BLUR PS_BLUR_X(PS_IN In)
{
    PS_OUT_BLUR Out = (PS_OUT_BLUR) 0;
    
    float2 vTexcoord;
    vector vColor;
    
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + i / g_fWinSizeX;
        vTexcoord.y = In.vTexcoord.y;
        
        vColor += g_fWeights[i + 6] * g_EmissiveTexture.Sample(ClampSampler, vTexcoord);
    }
    
    Out.vBlur = vColor;
    
    return Out;
}

PS_OUT_BACKBUFFER PS_BLUR_Y(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    float2 vTexcoord;
    vector vColor;
    
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + i / g_fWinSizeY;
        
        vColor += g_fWeights[i + 6] * g_BlurTexture.Sample(ClampSampler, vTexcoord);
    }
    
    vColor.a = g_BlurTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vFinalColor = g_BackBufferTexture.Sample(DefaultSampler, In.vTexcoord);
    
    Out.vColor = vFinalColor + vColor;
    
    return Out;
}


PS_OUT_BACKBUFFER PS_DISTORTION(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    float2 vTexcoord;
    float2 vWeight;
    vector vNormal;
    vector vNormalData;
    
    vNormalData = g_DistortionTexture.Sample(PointSampler, In.vTexcoord);
    
    vNormalData = vector((vNormalData.xy * 2.f) - 1.f, vNormalData.z, vNormalData.a);
    vWeight = (vNormalData.xy * vNormalData.z) * vNormalData.a;
    
    vWeight *= 0.12f;
    
    vTexcoord = In.vTexcoord + vWeight;
    
    vector vFinalColor = g_BlurEndTexture.Sample(ClampSampler, vTexcoord);
    
    Out.vColor = vFinalColor;
    
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

    pass BlurX
    {
        SetRasterizerState(RS_CULL_NONE);
        SetDepthStencilState(DSS_ZDISABLE, 0);
        SetBlendState(BS_ALPHABLEND, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_X();
    }

    pass BlurY
    {
        SetRasterizerState(RS_DEFAULT);
        SetDepthStencilState(DSS_ZDISABLE, 0);
        SetBlendState(BS_DEFAULT, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_Y();
    }

    pass Distortion
    {
        SetRasterizerState(RS_DEFAULT);
        SetDepthStencilState(DSS_ZDISABLE, 0);
        SetBlendState(BS_DEFAULT, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION();
    }
}
