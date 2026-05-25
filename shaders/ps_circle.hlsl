struct VSOutput_PSInput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

//Rende il quadrato un cerchio
float4 main(VSOutput_PSInput input) : SV_Target
{
    //Trasliamo la coordinata in riferimento al centro del quadrato (e centro del cerchio che vogliamo disegnare)
    float2 newUV = input.uv - float2(0.5, 0.5);
    
    //Calcoliamo la distanza al quadrato (uguale a fare (newUV.x * newUV.x) + (newUV.y * newUV.y))
    float distance = dot(newUV, newUV);

    //Trucco matematico, se la distanza calcolata è maggiore di 0.25 (=0.5 * 0.5), sarà fuori dal cerchio
    if (distance > 0.25) { discard; }   //Scarta tutti i pixel che non sono nel raggio del cerchio

    return input.color;                 //Restituisce il colore dei pixel interni al cerchio
}