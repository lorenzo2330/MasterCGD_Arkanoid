struct VSOutput_PSInput
{
    float4 pos : SV_Position;
    float2 uv  : TEXCOORD0;
};

VSOutput_PSInput main(uint vertID : SV_VertexID)
{
    VSOutput_PSInput o;

    //Trucco bitwise: genera UV fisse per i 3 vertici
    //  0 -> (0, 0) -> Alto-sinistra
    //  1 -> (2, 0) -> Fuori-alto-destra
    //  2 -> (0, 2) -> Fuori-basso-sinistra
    o.uv = float2((vertID << 1) & 2, vertID & 2);

    //Conversione da coordinate UV [0, 2] a coordinate NDC (inizialmente [-3, +3], poi il sampler le clipperà a [-1, +1])
    o.pos = float4(o.uv.x * 2.0f - 1.0f, -o.uv.y * 2.0f + 1.0f, 0.0f, 1.0f);

    return o;
}
