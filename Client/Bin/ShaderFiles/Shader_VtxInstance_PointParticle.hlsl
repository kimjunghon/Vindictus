#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_DistortionTexture;

float3 g_vSourceColor = float3(1.f, 1.f, 1.f);

vector g_vCamPosition;
bool g_IsEmissive;

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

VS_ROTATE_OUT VS_BIG(VS_IN In)
{
    VS_ROTATE_OUT Out = (VS_ROTATE_OUT) 0;
    
    float4x4 TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
    
    vector vUp = mul(float4(g_WorldMatrix[1].xyz, 0.f), TransformMatrix);
    
    Out.vPosition = mul(vPosition, g_WorldMatrix);
    Out.fSize = length(In.vRight);
    Out.vLifeTime = In.vLifeTime;
    Out.vUp = vUp;
    
    float fRatio = saturate(In.vLifeTime.x / In.vLifeTime.y);
    
    float fSize = length(In.vRight);
    
    Out.vPosition = mul(vPosition, g_WorldMatrix);
    Out.fSize = fSize + (fSize * fRatio);
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

[maxvertexcount(6)]
void GS_SPRITE(point GS_IN In[1], inout TriangleStream<GS_OUT> Vertices)
{
    GS_OUT Out[4];
    
    vector vRight;
    vector vUp;
    vector vLook;
    
    vLook = g_vCamPosition - In[0].vPosition;
    vRight = normalize(vector(cross(float3(0.f, 1.f, 0.f), vLook.xyz), 0.f)) * In[0].fSize * 0.5f;
    vUp = normalize(vector(cross(vLook.xyz, vRight.xyz), 0.f)) * In[0].fSize * 0.5f;
    
    matrix matrVP = mul(g_ViewMatrix, g_ProjMatrix);
        
    int iCurrentTime = (In[0].vLifeTime.x / In[0].vLifeTime.y) * 25;
    
    float fU = (iCurrentTime % 5) * 0.2f;
    float fV = (iCurrentTime / 5) * 0.2f;
    
    Out[0].vPosition = mul(In[0].vPosition + vRight + vUp, matrVP);
    Out[0].vTexcoord = float2(fU, fV);
    Out[0].vLifeTime = In[0].vLifeTime;
    
    Out[1].vPosition = mul(In[0].vPosition - vRight + vUp, matrVP);
    Out[1].vTexcoord = float2(fU + 0.2f, fV);
    Out[1].vLifeTime = In[0].vLifeTime;
    
    Out[2].vPosition = mul(In[0].vPosition - vRight - vUp, matrVP);
    Out[2].vTexcoord = float2(fU + 0.2f, fV + 0.2f);
    Out[2].vLifeTime = In[0].vLifeTime;
    
    Out[3].vPosition = mul(In[0].vPosition + vRight - vUp, matrVP);
    Out[3].vTexcoord = float2(fU, fV + 0.2f);
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

[maxvertexcount(6)]
void GS_BLOOD_SPRITE(point GS_ROTATE_IN In[1], inout TriangleStream<GS_OUT> Vertices)
{
    GS_OUT Out[4];
    
    vector vRight;
    vector vUp;
    vector vLook;
    
    vLook = g_vCamPosition - In[0].vPosition;
    vRight = normalize(vector(cross(In[0].vUp.xyz, vLook.xyz), 0.f)) * In[0].fSize * 0.5f;
    vUp = normalize(vector(cross(vLook.xyz, vRight.xyz), 0.f)) * In[0].fSize * 0.5f;
    
    matrix matrVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    int iCurrentTime = (In[0].vLifeTime.x / In[0].vLifeTime.y) * 16;
    
    int iX = (iCurrentTime % 4);
    int iY = (iCurrentTime / 4);
    float fU = iX * 0.25f;
    float fV = iY * 0.25f;
    
    Out[0].vPosition = mul(In[0].vPosition + vRight + vUp, matrVP);
    Out[0].vTexcoord = float2(fU, fV);
    Out[0].vLifeTime = In[0].vLifeTime;
    
    Out[1].vPosition = mul(In[0].vPosition - vRight + vUp, matrVP);
    Out[1].vTexcoord = float2(fU + 0.25f, fV);
    Out[1].vLifeTime = In[0].vLifeTime;
    
    Out[2].vPosition = mul(In[0].vPosition - vRight - vUp, matrVP);
    Out[2].vTexcoord = float2(fU + 0.25f, fV + 0.25f);
    Out[2].vLifeTime = In[0].vLifeTime;
    
    Out[3].vPosition = mul(In[0].vPosition + vRight - vUp, matrVP);
    Out[3].vTexcoord = float2(fU, fV + 0.25f);
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
    float4 vBackBufferColor : SV_TARGET0;
    float4 vEmissiveColor : SV_TARGET1;
};

struct PS_DISTORTION_OUT
{
    float4 vDistortionColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_DEFAULT_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMask = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    
    vector vSourColor = float4(g_vSourceColor, 1.f);
    
    vector vFinalColor = vSourColor * vMask;
    
    float vDestAlpha = max(max(vMask.r, vMask.g), vMask.b);
    
    vFinalColor.a = 1.f * vDestAlpha;
    
    float fDecreaseAlpha = (In.vLifeTime.x / In.vLifeTime.y);
    
    vFinalColor.a -= fDecreaseAlpha;
    
    if (vFinalColor.a <= 0.f)
        discard;
    
    Out.vBackBufferColor = vFinalColor;
    
    if(g_IsEmissive)
    {
        float fLuminance = Luminance(vFinalColor.rgb);
    
        vector vEmissiveColor = 0.f;
    
        if (fLuminance > 0.3f)
        {
            vEmissiveColor = vector(vFinalColor.rgb * 3.f, vFinalColor.a);
        }

        Out.vEmissiveColor = vEmissiveColor;
    }
    
    return Out;
}

PS_OUT PS_RIGHTLOOP(PS_DEFAULT_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMask = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    
    vector vSourColor = float4(g_vSourceColor, 1.f);
    
    vector vFinalColor = vSourColor * vMask;
    
    float vDestAlpha = max(max(vMask.r, vMask.g), vMask.b);
    
    vFinalColor.a = 1.f * vDestAlpha;
    
    float fDecreaseAlpha = min((In.vLifeTime.x / In.vLifeTime.y), 0.5f);
    
    vFinalColor.a -= fDecreaseAlpha;
    
    if(vFinalColor.a <= 0.f)
        discard;
    
    Out.vBackBufferColor = vFinalColor;
    
    if (g_IsEmissive)
    {
        float fLuminance = Luminance(vFinalColor.rgb);
    
        vector vEmissiveColor = 0.f;
    
        if (fLuminance > 0.4f)
        {
            vEmissiveColor = vector(vFinalColor.rgb * 3.f, vFinalColor.a);
        }

        Out.vEmissiveColor = vEmissiveColor;
    }
    
    return Out;
}

PS_OUT PS_DUST(PS_DEFAULT_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMask = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    
    vector vSourColor = float4(g_vSourceColor, 1.f);
    
    vector vFinalColor = vSourColor * vMask;
    
    float vDestAlpha = max(max(vMask.r, vMask.g), vMask.b);
    
    vFinalColor.a = 1.f * vDestAlpha;
    
    float fDecreaseAlpha = (In.vLifeTime.x / In.vLifeTime.y);
    
    vFinalColor.a -= fDecreaseAlpha;
    
    if (vFinalColor.a <= 0.f)
        discard;
    
    vFinalColor.a = min(vFinalColor.a, 0.5f);
    
    Out.vBackBufferColor = vFinalColor;
    
    if (g_IsEmissive)
    {
        float fLuminance = Luminance(vFinalColor.rgb);
    
        vector vEmissiveColor = 0.f;
    
        if (fLuminance > 0.3f)
        {
            vEmissiveColor = vector(vFinalColor.rgb * 3.f, vFinalColor.a);
        }

        Out.vEmissiveColor = vEmissiveColor;
    }
    
    return Out;
}

PS_OUT PS_BLOOD(PS_DEFAULT_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMask = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    
    vector vSourColor = float4(g_vSourceColor, 1.f);
    
    vector vFinalColor = vSourColor * vMask;
    
    float vDestAlpha = vMask.a; //max(max(vMask.r, vMask.g), vMask.b);
    
    vFinalColor.a = 1.f * vDestAlpha;
    
    float fDecreaseAlpha = (In.vLifeTime.x / In.vLifeTime.y);
    
    if (fDecreaseAlpha >= 0.75f)
    {   
        float fLastAlpha = 1.f - fDecreaseAlpha;
        
        vFinalColor.a -= (fLastAlpha * 4.f);
    }
    
    if (vFinalColor.a <= 0.f)
        discard;
    
    Out.vBackBufferColor = vFinalColor;
    
    if (g_IsEmissive)
    {
        float fLuminance = Luminance(vFinalColor.rgb);
    
        vector vEmissiveColor = 0.f;
    
        if (fLuminance > 0.3f)
        {
            vEmissiveColor = vector(vFinalColor.rgb * 3.f, vFinalColor.a);
        }

        Out.vEmissiveColor = vEmissiveColor;
    }
    
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
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass SpritePass
    {
        SetRasterizerState(RS_CULL_NONE);
        SetDepthStencilState(DSS_DEFAULT, 0);
        SetBlendState(BS_ALPHABLEND, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_SPRITE();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass RotatePass
    {
        SetRasterizerState(RS_CULL_NONE);
        SetDepthStencilState(DSS_DEFAULT, 0);
        SetBlendState(BS_ALPHABLEND, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_ROTATE();
        GeometryShader = compile gs_5_0 GS_ROTATE();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass LoopPass
    {
        SetRasterizerState(RS_CULL_NONE);
        SetDepthStencilState(DSS_DEFAULT, 0);
        SetBlendState(BS_ALPHABLEND, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_RIGHTLOOP();
    }

    pass SamllSpritePass
    {
        SetRasterizerState(RS_CULL_NONE);
        SetDepthStencilState(DSS_DEFAULT, 0);
        SetBlendState(BS_ALPHABLEND, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_SMALL();
        GeometryShader = compile gs_5_0 GS_SPRITE();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass SamllPass
    {
        SetRasterizerState(RS_CULL_NONE);
        SetDepthStencilState(DSS_DEFAULT, 0);
        SetBlendState(BS_ALPHABLEND, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_SMALL();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass SlashPointPass
    {
        SetRasterizerState(RS_CULL_NONE);
        SetDepthStencilState(DSS_DEFAULT, 0);
        SetBlendState(BS_ALPHABLEND, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_BIG();
        GeometryShader = compile gs_5_0 GS_ROTATE();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
        
    pass DustPass
    {
        SetRasterizerState(RS_CULL_NONE);
        SetDepthStencilState(DSS_DEFAULT, 0);
        SetBlendState(BS_ALPHABLEND, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_BIG();
        GeometryShader = compile gs_5_0 GS_ROTATE();
        PixelShader = compile ps_5_0 PS_DUST();
    }

    pass BloodPass
    {
        SetRasterizerState(RS_CULL_NONE);
        SetDepthStencilState(DSS_DEFAULT, 0);
        SetBlendState(BS_ALPHABLEND, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_ROTATE();
        GeometryShader = compile gs_5_0 GS_BLOOD_SPRITE();
        PixelShader = compile ps_5_0 PS_BLOOD();
    }
}