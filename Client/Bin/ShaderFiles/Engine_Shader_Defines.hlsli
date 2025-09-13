sampler DefaultSampler = sampler_state
{
    filter = min_mag_mip_linear;
    AddressU = wrap;
    AddressV = wrap;
};

sampler PointSampler = sampler_state
{
    filter = min_mag_mip_point;
    AddressU = wrap;
    AddressV = wrap;
};

RasterizerState RS_DEFAULT
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = false;
};

RasterizerState RS_CULL_CW
{
    CullMode = Front;
};

RasterizerState RS_CULL_NONE
{
    CullMode = NONE;
};

RasterizerState RS_WIRE
{
    FillMode = Wireframe;
};

DepthStencilState DSS_DEFAULT
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = LESS_EQUAL;
};

DepthStencilState DSS_ZDISABLE
{
    DepthEnable = false;
    DepthWriteMask = zero;
};

BlendState BS_DEFAULT
{
    BlendEnable[0] = false;
};

BlendState BS_ALPHABLEND
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = Add;
};

float Random(float2 St)
{
    return frac(sin(dot(St.xy, float2(12.9898, 78.233))) * 43758.5453123);
}

float Noise(float2 St)
{
    float2 i = floor(St);
    float2 f = frac(St);
    
    float a = Random(i);
    float b = Random(i + float2(1.0, 0.0));
    float c = Random(i + float2(0.0, 1.0));
    float d = Random(i + float2(1.0, 1.0));

    float2 u = f * f * (3.0 - 2.0 * f);
    
    return lerp(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

float FBM(float2 St)
{
    float v = 0.0;
    float a = 0.5;
    
    float2x2 Rot = float2x2(cos(0.5), sin(0.5),
                            -sin(0.5), cos(0.5));
    
    int Octaves = 5;
    for (int i = 0; i < Octaves; i++)
    {
        v += a * Noise(St);
        St = mul((St * 2.0), Rot);
        a *= 0.5;
    }
    
    return v;

}