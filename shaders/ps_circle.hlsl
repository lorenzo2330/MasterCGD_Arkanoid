struct PSInput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

float4 main(PSInput input) : SV_Target
{
    float2 d = input.uv - float2(0.5, 0.5);
    float dist = dot(d, d);

    if (dist > 0.25)
        discard;

    return input.color;
}