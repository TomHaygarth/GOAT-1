
struct SVertInput
{
    [[vk::location(0)]] uint idx : SV_VertexID;
};

struct SVertexOutput
{
    [[vk::location(0)]] float4 vPos : SV_Position;
    [[vk::location(1)]] float4 vDiffuse : COLOR;
};

static const float4 _verts[] = {
    float4(0.0, -0.5, 0.0, 1.0),
    float4(0.5, 0.5, 0.0, 1.0),
    float4(-0.5, 0.5, 0.0, 1.0)
};

static const float4 colors[] = {
    float4(1.0, 0.0, 0.0, 1.0),
    float4(0.0, 1.0, 0.0, 1.0),
    float4(0.0, 0.0, 1.0, 1.0)
};

SVertexOutput main(in const SVertInput v)
{
    SVertexOutput output;
    output.vPos = _verts[v.idx];

    output.vDiffuse = colors[v.idx];

    return output;
}