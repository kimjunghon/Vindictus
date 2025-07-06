
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float g_ProgressBarStartX, g_ProgressBarSizeX ,g_ProgressBarRatio;

texture2D g_Texture;

sampler DefaultSampler = sampler_state
{
    filter = min_mag_mip_linear;
    
};


struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

// Default Pass Start

struct VS_DEFAULT_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};


VS_DEFAULT_OUT VS_MAIN(VS_IN In)
{
    VS_DEFAULT_OUT Out = (VS_DEFAULT_OUT) 0;

    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

struct PS_DEFAULT_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};


PS_OUT PS_MAIN(PS_DEFAULT_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;
}

// Default Pass End

// ProgressBar Pass Start

struct VS_PROGRESS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

VS_PROGRESS_OUT VS_PROGRESSBAR(VS_IN In)
{
    VS_PROGRESS_OUT Out = (VS_PROGRESS_OUT) 0;

    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    
    return Out;
}

struct P_PROGRESS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float3 vWorldPos : TEXCOORD1;
};

PS_OUT PS_PROGRESSBAR(P_PROGRESS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (In.vWorldPos.x <= g_ProgressBarStartX + (g_ProgressBarSizeX * g_ProgressBarRatio))
        Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    else
        discard;
    
    return Out;
}
// ProgressBar Pass End

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass ProgressBarPass
    {
        VertexShader = compile vs_5_0 VS_PROGRESSBAR();
        PixelShader = compile ps_5_0 PS_PROGRESSBAR();
    }
}