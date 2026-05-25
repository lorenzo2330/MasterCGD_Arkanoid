struct VSOutput_PSInput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD0;
};

float4 main(VSOutput_PSInput input) : SV_TARGET
{
	//Restituisce il colore del pixel (se i vertici hanno colori diversi, vengono interpolati)
    return input.color;
}