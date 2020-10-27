
struct SFragInput
{
    float4 vPos : SV_POSITION;
    float4 vDiffuse : COLOR;
};

float4 main(SFragInput input) : SV_TARGET
{
    return input.vDiffuse;;
}
