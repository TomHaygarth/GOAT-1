
struct SFragInput
{
    [[vk::location(0)]] float4 vPos : SV_Position;
    [[vk::location(1)]] float4 vDiffuse : COLOR;
};

struct SFragOutput
{
    [[vk::location(0)]] float4 colour : SV_Target;
};

SFragOutput main(const SFragInput input)
{
    SFragOutput output;
    output.colour = input.vDiffuse;
    return output;
}