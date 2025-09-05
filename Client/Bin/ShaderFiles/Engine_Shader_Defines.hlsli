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