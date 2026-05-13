struct VSInput
{
    float2 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

PSInput main(VSInput input)
{
    PSInput output;
    output.pos = float4(input.pos, 0.0f, 1.0f);
    output.color = input.color;
    output.uv = input.uv;
    return output;
}