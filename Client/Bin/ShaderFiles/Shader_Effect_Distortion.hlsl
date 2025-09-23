#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_MaskTexture;
texture2D g_DistortionTexture;

vector g_vCamPosition;

float g_fRadius = 10.f;

struct VS_IN
{
    float3 vPosition : POSITION;
    
    float4 vRight : TEXCOORD0;
    float4 vUp : TEXCOORD1;
    float4 vLook : TEXCOORD2;
    float4 vTranslation : TEXCOORD3;
    
    float2 vLifeTime : TEXCOORD4;
};

struct VS_DEFAULT_OUT
{
    float4 vPosition : SV_POSITION;
    float fSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
};


VS_DEFAULT_OUT VS_MAIN(VS_IN In)
{
    VS_DEFAULT_OUT Out = (VS_DEFAULT_OUT) 0;
    
    float4x4 TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
    
    Out.vPosition = mul(vPosition, g_WorldMatrix);
    Out.fSize = length(In.vRight);
    Out.vLifeTime = In.vLifeTime;

    return Out;
}

struct VS_ROTATE_OUT
{
    float4 vPosition : SV_POSITION;
    float fSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
    float4 vUp : TEXCOORD1;
};

VS_ROTATE_OUT VS_ROTATE(VS_IN In)
{
    VS_ROTATE_OUT Out = (VS_ROTATE_OUT) 0;
    
    float4x4 TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
    
    vector vUp = mul(float4(g_WorldMatrix[1].xyz, 0.f), TransformMatrix);
    
    Out.vPosition = mul(vPosition, g_WorldMatrix);
    Out.fSize = length(In.vRight);
    Out.vLifeTime = In.vLifeTime;
    Out.vUp = vUp;
   
    return Out;
}

VS_DEFAULT_OUT VS_SMALL(VS_IN In)
{
    VS_DEFAULT_OUT Out = (VS_DEFAULT_OUT) 0;
    
    float4x4 TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
     
    float fRatio = 1.f - (In.vLifeTime.x / In.vLifeTime.y);
    
    Out.vPosition = mul(vPosition, g_WorldMatrix);
    Out.fSize = length(In.vRight) * fRatio;
    Out.vLifeTime = In.vLifeTime;
   
    return Out;
}

struct VS_NONBILL_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vLifeTime : TEXCOORD0;
    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
};

VS_NONBILL_OUT VS_NONBILL(VS_IN In)
{
    VS_NONBILL_OUT Out = (VS_NONBILL_OUT) 0;
    
    float4x4 TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
 
    vector vRight = mul(In.vRight, g_WorldMatrix) * 0.5f;
    vector vUp = mul(In.vUp, g_WorldMatrix) * 0.5f;
  
    
    Out.vPosition = mul(vPosition, g_WorldMatrix);
    Out.vLifeTime = In.vLifeTime;
    Out.vRight = vRight;
    Out.vUp = vUp;
   
    return Out;
}

VS_NONBILL_OUT VS_BIG(VS_IN In)
{
    VS_NONBILL_OUT Out = (VS_NONBILL_OUT) 0;
    
    float4x4 TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
  
    vector vRight = mul(In.vRight, g_WorldMatrix) * 0.5f;
    vector vUp = mul(In.vUp, g_WorldMatrix) * 0.5f;
  
    Out.vPosition = mul(vPosition, g_WorldMatrix);
    Out.vLifeTime = In.vLifeTime;
  
    float fRatio = saturate(In.vLifeTime.x / In.vLifeTime.y);
    
    Out.vPosition = mul(vPosition, g_WorldMatrix);
    Out.vLifeTime = In.vLifeTime;
    Out.vRight = vRight + (vRight * (fRatio * g_fRadius));
    Out.vUp = vUp + (vUp * (fRatio * g_fRadius));
    
    return Out;
}

VS_NONBILL_OUT VS_SHIELD(VS_IN In)
{
    VS_NONBILL_OUT Out = (VS_NONBILL_OUT) 0;
    
    float4x4 TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
  
    vector vRight = mul(In.vRight, g_WorldMatrix) * 0.5f;
    vector vUp = mul(In.vUp, g_WorldMatrix) * 0.5f;
  
    Out.vPosition = mul(vPosition, g_WorldMatrix);
    Out.vLifeTime = In.vLifeTime;
  
    float fRatio = saturate(In.vLifeTime.x / In.vLifeTime.y);
  
    if(fRatio < 0.5f)
    {
        vRight = vRight - (vRight * fRatio);
        vUp = vUp - (vUp * fRatio);
    }
    else
    {
        vector vSmallRight = vRight - (vRight * 0.5f);
        vector vSmallvUp = vUp - (vUp * 0.5f);    
        
        float fBigRatio = (fRatio - 0.5f) * 2.f;
        
        vRight = vSmallRight + (vRight * fBigRatio);
        vUp = vSmallvUp + (vUp * fBigRatio);
    }
    
    
    Out.vPosition = mul(vPosition, g_WorldMatrix);
    Out.vLifeTime = In.vLifeTime;
    Out.vRight = vRight;
    Out.vUp = vUp;
    
    return Out;
}

VS_DEFAULT_OUT VS_BLAZE(VS_IN In)
{
    VS_DEFAULT_OUT Out = (VS_DEFAULT_OUT) 0;
    
    float4x4 TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
     
    float fRatio = (In.vLifeTime.x / In.vLifeTime.y);
    
    float fSize = length(In.vRight);
    
    fSize = fSize + (fSize * fRatio * 0.5f);
    
    Out.vPosition = mul(vPosition, g_WorldMatrix);
    Out.fSize = fSize;
    Out.vLifeTime = In.vLifeTime;
   
    return Out;
}

struct GS_IN
{
    float4 vPosition : SV_POSITION;
    float fSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Vertices)
{
    GS_OUT Out[4];
    
    vector vRight;
    vector vUp;
    vector vLook;
    
    vLook = g_vCamPosition - In[0].vPosition;
    vRight = normalize(vector(cross(float3(0.f, 1.f, 0.f), vLook.xyz), 0.f)) * In[0].fSize * 0.5f;
    vUp = normalize(vector(cross(vLook.xyz, vRight.xyz), 0.f)) * In[0].fSize * 0.5f;
    
    matrix matrVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    Out[0].vPosition = mul(In[0].vPosition + vRight + vUp, matrVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    
    Out[1].vPosition = mul(In[0].vPosition - vRight + vUp, matrVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    
    Out[2].vPosition = mul(In[0].vPosition - vRight - vUp, matrVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    
    Out[3].vPosition = mul(In[0].vPosition + vRight - vUp, matrVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;
    
    Vertices.Append(Out[0]);
    Vertices.Append(Out[1]);
    Vertices.Append(Out[2]);
    Vertices.RestartStrip();
    
    Vertices.Append(Out[0]);
    Vertices.Append(Out[2]);
    Vertices.Append(Out[3]);
    Vertices.RestartStrip();
}

struct GS_NONBILL_IN
{
    float4 vPosition : SV_POSITION;
    float2 vLifeTime : TEXCOORD0;
    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
};


[maxvertexcount(6)]
void GS_NONBILL(point GS_NONBILL_IN In[1], inout TriangleStream<GS_OUT> Vertices)
{
    GS_OUT Out[4];
    
    matrix matrVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    Out[0].vPosition = mul(In[0].vPosition + In[0].vRight + In[0].vUp, matrVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    
    Out[1].vPosition = mul(In[0].vPosition - In[0].vRight + In[0].vUp, matrVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    
    Out[2].vPosition = mul(In[0].vPosition - In[0].vRight - In[0].vUp, matrVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    
    Out[3].vPosition = mul(In[0].vPosition + In[0].vRight - In[0].vUp, matrVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;
    
    Vertices.Append(Out[0]);
    Vertices.Append(Out[1]);
    Vertices.Append(Out[2]);
    Vertices.RestartStrip();
    
    Vertices.Append(Out[0]);
    Vertices.Append(Out[2]);
    Vertices.Append(Out[3]);
    Vertices.RestartStrip();
}

struct GS_ROTATE_IN
{
    float4 vPosition : SV_POSITION;
    float fSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
    float4 vUp : TEXCOORD1;
};

[maxvertexcount(6)]
void GS_ROTATE(point GS_ROTATE_IN In[1], inout TriangleStream<GS_OUT> Vertices)
{
    GS_OUT Out[4];
    
    vector vRight;
    vector vUp;
    vector vLook;
    
    vLook = g_vCamPosition - In[0].vPosition;
    vRight = normalize(vector(cross(In[0].vUp.xyz, vLook.xyz), 0.f)) * In[0].fSize * 0.5f;
    vUp = normalize(vector(cross(vLook.xyz, vRight.xyz), 0.f)) * In[0].fSize * 0.5f;
    
    matrix matrVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    Out[0].vPosition = mul(In[0].vPosition + vRight + vUp, matrVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    
    Out[1].vPosition = mul(In[0].vPosition - vRight + vUp, matrVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    
    Out[2].vPosition = mul(In[0].vPosition - vRight - vUp, matrVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    
    Out[3].vPosition = mul(In[0].vPosition + vRight - vUp, matrVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;
    
    Vertices.Append(Out[0]);
    Vertices.Append(Out[1]);
    Vertices.Append(Out[2]);
    Vertices.RestartStrip();
    
    Vertices.Append(Out[0]);
    Vertices.Append(Out[2]);
    Vertices.Append(Out[3]);
    Vertices.RestartStrip();
}

struct PS_DEFAULT_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

struct PS_OUT
{
    float4 vDistortionColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_DEFAULT_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDistortion = g_DistortionTexture.Sample(PointSampler, In.vTexcoord);
    
    float fDecreaseAlpha = (In.vLifeTime.x / In.vLifeTime.y);
    
    vDistortion.a -= fDecreaseAlpha;
    
    if (vDistortion.a <= 0.f)
        discard;
    
    Out.vDistortionColor = vDistortion;
    
    return Out;
}

PS_OUT PS_MASK(PS_DEFAULT_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMask = g_MaskTexture.Sample(PointSampler, In.vTexcoord);
    
    vector vDistortion = g_DistortionTexture.Sample(PointSampler, In.vTexcoord);
    
    float fDestAlpha = max(max(vMask.r, vMask.g), vMask.b);
    
    float fDecreaseAlpha = (In.vLifeTime.x / In.vLifeTime.y);
    
    float vFinalAlpha = fDestAlpha - fDecreaseAlpha;
        
    if(vFinalAlpha <= 0.f)
        discard;
    
    vector vFinal = vDistortion * vFinalAlpha;
   
    Out.vDistortionColor = vFinal;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_CULL_NONE);
        SetDepthStencilState(DSS_DEFAULT, 0);
        SetBlendState(BS_DEFAULT, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass RotatePass
    {
        SetRasterizerState(RS_CULL_NONE);
        SetDepthStencilState(DSS_DEFAULT, 0);
        SetBlendState(BS_DEFAULT, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_ROTATE();
        GeometryShader = compile gs_5_0 GS_ROTATE();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass DefaultMaskPass
    {
        SetRasterizerState(RS_CULL_NONE);
        SetDepthStencilState(DSS_DEFAULT, 0);
        SetBlendState(BS_DEFAULT, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MASK();
    }

    pass RotateMaskPass
    {
        SetRasterizerState(RS_CULL_NONE);
        SetDepthStencilState(DSS_DEFAULT, 0);
        SetBlendState(BS_DEFAULT, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_ROTATE();
        GeometryShader = compile gs_5_0 GS_ROTATE();
        PixelShader = compile ps_5_0 PS_MASK();
    }

    pass NonBillPass
    {
        SetRasterizerState(RS_CULL_NONE);
        SetDepthStencilState(DSS_DEFAULT, 0);
        SetBlendState(BS_DEFAULT, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_NONBILL();
        GeometryShader = compile gs_5_0 GS_NONBILL();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    
    pass NonBillMaskPass
    {
        SetRasterizerState(RS_CULL_NONE);
        SetDepthStencilState(DSS_DEFAULT, 0);
        SetBlendState(BS_DEFAULT, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_NONBILL();
        GeometryShader = compile gs_5_0 GS_NONBILL();
        PixelShader = compile ps_5_0 PS_MASK();
    }

    pass BluntPass
    {
        SetRasterizerState(RS_CULL_NONE);
        SetDepthStencilState(DSS_DEFAULT, 0);
        SetBlendState(BS_DEFAULT, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_BIG();
        GeometryShader = compile gs_5_0 GS_NONBILL();
        PixelShader = compile ps_5_0 PS_MASK();
    }
    
    pass ShieldPass
    {
        SetRasterizerState(RS_CULL_NONE);
        SetDepthStencilState(DSS_DEFAULT, 0);
        SetBlendState(BS_DEFAULT, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_SHIELD();
        GeometryShader = compile gs_5_0 GS_NONBILL();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass BlazePass
    {
        SetRasterizerState(RS_CULL_NONE);
        SetDepthStencilState(DSS_DEFAULT, 0);
        SetBlendState(BS_DEFAULT, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_BLAZE();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}