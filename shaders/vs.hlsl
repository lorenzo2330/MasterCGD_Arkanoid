//Struttura dati passati dalla CPU (vertex buffer)  //Slide 73
struct VSInput
{
    float2 pos : POSITION;  //Posizione del vertice in coordinate 2D (x, y)
    float4 color : COLOR;   //Colore del vertice
    float2 uv : TEXCOORD0;  //Coordinate della texture
};

//Struttura dati output del VertexShader (quindi input del PixelShader) //Slide 73
struct VSOutput_PSInput
{
    float4 pos : SV_POSITION;   //Poisizione vertice in coordinate NDC (x, y, z, w)
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

//Struttura del Vertex Shader, inoltra (dopo aver eventualmente convertito) l'input ricevuto al pixel shader
VSOutput_PSInput main(VSInput input)
{
    VSOutput_PSInput output;
    output.pos = float4(input.pos, 0.0f, 1.0f); //Converte da (x, y) a (x, y, z, w)
    output.color = input.color;
    output.uv = input.uv;
    return output;
}