
struct SVertInput
{
    uint idx : SV_VertexID;
};

struct SVertexOutput
{
    float4 vPos : POSITION;
    float4 vDiffuse : COLOR;
};

float4 _verts[3] = {
    float4(0.0, -0.5, 0.0, 1.0),
    float4(0.5, 0.5, 0.0, 1.0),
    float4(-0.5, 0.5, 0.0, 1.0)
};

SVertexOutput main(const SVertInput v)
{
    SVertexOutput output;
    output.vPos = _verts[v.idx];
    output.vDiffuse = clamp(output.vPos, 0.0, 1.0);
    return output;
}