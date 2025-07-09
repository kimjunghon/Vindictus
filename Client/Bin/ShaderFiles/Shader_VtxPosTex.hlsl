
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float g_fStartX, g_fSizeX, g_fProgressBarRatio;
float g_fWinSizeX = 1280.f;
float g_fWinSizeY = 720.f;

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

// Default Pass Start ------------------------------------------------------------------------------------------

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
    
    if(Out.vColor.a <= 0.3f)
        discard;
    
    return Out;
}

// Default Pass End ------------------------------------------------------------------------------------------------

// ProgressBar Pass Start ------------------------------------------------------------------------------------------

PS_OUT PS_PROGRESSBAR(PS_DEFAULT_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (In.vPosition.x <= g_fStartX + (g_fSizeX * g_fProgressBarRatio))
        Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    else
        discard;
    
    if (Out.vColor.a <= 0.3f)
        discard;

    return Out;
}
// ProgressBar Pass End -------------------------------------------------------------------------------------------

// LoadingPoint Pass Start ----------------------------------------------------------------------------------------

struct VS_LOADINGPOINT_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float3 vLocalPos : TEXCOORD1;
};


VS_LOADINGPOINT_OUT VS_LOADINGPOINT(VS_IN In)
{
    VS_LOADINGPOINT_OUT Out = (VS_LOADINGPOINT_OUT) 0;

    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vLocalPos = In.vPosition;
    
    return Out;
}

struct PS_LOADINGPOINT_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float3 vLocalPos : TEXCOORD1;
};


PS_OUT PS_LOADINGPOINT(PS_LOADINGPOINT_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 vPosition = { In.vLocalPos.x, In.vLocalPos.y };
    float2 vCenter = 0.f;
    float2 vLength = length(vPosition - vCenter);
    float Length = vLength;
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    if (abs(In.vLocalPos.x) <= 0.05f)
    {
        if (abs(In.vLocalPos.y) >= 0.3f)    
            Out.vColor.a = (0.5f - Length);
    }
    else
        Out.vColor.a = (0.5f - Length);
    
    if(Out.vColor.a <= 0.3f)
        discard;
    
    return Out;
}

// LoadingPoint Pass End ---------------------------------------------------------------------------

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass ProgressBarPass
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_PROGRESSBAR();
    }

    pass LoadingPointPass
    {
        VertexShader = compile vs_5_0 VS_LOADINGPOINT();
        PixelShader = compile ps_5_0 PS_LOADINGPOINT();
    }
}