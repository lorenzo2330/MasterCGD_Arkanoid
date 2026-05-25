//Stessa struttura BallLightData di lightBallRenderer.h, questi dati vengono passati ad ogni frame
cbuffer BallLightDataBuffer : register(b0)
{
    float4 ballPositions[8];    //Posizioni in NDC [-1, +1] del centro di ogni pallina (max 8)
    float4 bonusPositions[8];   //Posizioni in NDC [-1, +1] del centro di ogni bonus (max 8)
    float ballCount;            //Numero di palline attualmente attive
    float bonusCount;           //Numero di bonus attualmente attivi
    float ballLightRadius;      //Raggio di luce delle palline
    float racketLightRadius;    //Raggio di luce della racchetta
    float bonusLightRadius;     //Raggio di luce dei bonus
    float ballPower;            //Intensità luminosa delle palline
    float racketPower;          //Intensità luminosa della racchetta
    float bonusPower;           //Intensità luminosa dei bonus
    float4 racketPosition;      //Posizione della racchetta
};

Texture2D bbBuffer : register(t0);          //"BackBackBuffer", texture di supporto (contenente il gioco con illuminazione normale)
SamplerState linearSampler : register(s0);  //Sampler (indica come leggere la texture)

struct VSOutput_PSInput
{
    float4 pos : SV_Position;
    float2 uv  : TEXCOORD0;
};

//Resituisce il contributo luminoso di un oggetto su un pixel
//  v := vettore tra oggetto e pixel
//  r := raggio di luminosità dell'oggetto
//  p := potenza luminosa dell'oggetto
float light(float2 v, float r, float p)
{
    //Luce sfumata in base alla distanza tra il pixel e l'oggetto
    float baseLight = saturate(1.0f - (length(v) / r));

    //Considera la potenza luminosa dell'oggetto
    return pow(baseLight, p);
}

float4 main(VSOutput_PSInput input) : SV_Target
{
    
    //Salva il colore originale del gioco (quello che avrebbe a "luminosità normale")
    float4 sceneColor = bbBuffer.Sample(linearSampler, input.uv);

    //Converte da UV [0, 1] a NDC [-1, +1]
    float2 screenNDC = float2(input.uv.x * 2.0f - 1.0f, -input.uv.y * 2.0f + 1.0f);

    //Calcola la luminosità del pixel sommando le varie sorgenti luminose (palline + bonus + racchette)
    float totalLight = 0.0f;

    //Contributo luminoso delle palline
    for (int i = 0; i < (int) ballCount; i++) { totalLight += light(screenNDC - ballPositions[i].xy, ballLightRadius, ballPower); }
    
    //Contributo luminoso dei bonus
    for (int j = 0; j < (int) bonusCount; j++) { totalLight += light(screenNDC - bonusPositions[j].xy, bonusLightRadius, bonusPower); }

    //Contributo luminoso della racchetta
    float2 racketDelta = screenNDC - racketPosition.xy;     //Calcola il centro della racchetta
    racketDelta.x /= 2.5f;                                  //Schiaccia orizzontalmente, la luce diventa un'elisse anzichè un cerchio
    totalLight += light(racketDelta, racketLightRadius, racketPower);
    
    
    //Moltiplica il vecchio colore (salvato inizialmente) con la luminosità calcolata
    return float4(sceneColor.rgb * totalLight, sceneColor.a);
}
