struct interpolants
{
    float4 position     : SV_POSITION0;
};


float4 main(interpolants input) : SV_TARGET
{
    return float4(1.0f, 1.0f, 1.0f, 0.0f);
}