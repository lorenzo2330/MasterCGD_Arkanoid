#define NOMINMAX
#include "ballPredictor.h"
#include "../game/utils.h"
#include "../colors.h"
#include "../data.h"
#include "../game/ball.h"
#include "../game/brick.h"
#include "../game/level.h"
#include "../render/renderer2d.h"
#include <cmath>
#include <cstdlib>
#include <limits>
#include <vector>


void BallPredictor::Update(const std::vector<Ball>& balls, const Level& level, float deltaTime)
{
    dashTimer += deltaTime;

    segments.clear();
    landingX = -1.0f;

    Predict(balls, level);
}

const Ball* BallPredictor::FindMostDangerous(const std::vector<Ball>& balls)
{
    const Ball* best = nullptr;
    float bestTime = std::numeric_limits<float>::max();

    for (const Ball& b : balls) {
        if (!IsThreat(b)) continue;
        float t = b.TimeToRacket();                     //Tempo prima che raggiunga la racchetta (tiene conto della velocità)
        if (t < bestTime) { bestTime = t; best = &b; }
    }
    return best;
}

void BallPredictor::Predict(const std::vector<Ball>& balls, const Level& level) {

    const Ball* best = FindMostDangerous(balls);

    //Mode 2: simula tutte le palline minacciose (la migliore con colore diverso)
    if (mode == 2) {
        for (const Ball& b : balls) { if (IsThreat(b)) Simulate(b, level, &b == best); }
    }
    else if (best) Simulate(*best, level, true);
}

void BallPredictor::Simulate(Ball ball, const Level& level, bool isBest)
{
    /*  Concetto: 
    *       anzichè fissare il tempo e calcolare lo spostamento al variare dello spazio
    *       fissiamo lo spazio (PREDICTOR_FIXED_STEP) e calcoliamo il tempo trascorso
    * 
    *   In questo modo, ad ogni frame sappiamo che ci saremo mossi di PREDICTOR_FIXED_STEP pixel, a prescindere dalla velocità
    */

    float speed = std::sqrt(ball.velX * ball.velX + ball.velY * ball.velY);
    if (speed < 1.0f) return;

    float dtStep = PREDICTOR_FIXED_STEP / speed;

    const std::vector<Brick>& realBricks = level.GetBricksConst();  //Copia dei mattoncini, per simularne l'interazione
    
    std::vector<bool> brickAlive(realBricks.size(), true);          //Maschera per salvarsi se il mattoncino è colpito

    bool hasReachedEndScreen = false;

    const size_t firstSegment = segments.size();    //Indice del primo segmento di questa traiettoria (con più palline, segments contiene tutte le traiettorie)

    int nBounce = 0;    //Numero di rimbalzi, se raggiunge PREDICTOR_MAX_BOUNCE interrompe la simulazione

    for (int step = 0; step < PREDICTOR_MAX_SIM && !hasReachedEndScreen && nBounce < PREDICTOR_MAX_BOUNCE; ++step)
    {
        float prevX = ball.posX;            //Salva la posizione attuale della palla
        float prevY = ball.posY;

        ball.posX += ball.velX * dtStep;    //Calcola la posizione prevista della palla
        ball.posY += ball.velY * dtStep;

        //Se rimbalza contro un bordo, si modifica la traiettoria in modo che anch'essa rispecchi il rimbalzo
        if (ball.posY - ball.r < 0.0f) { ball.posY = ball.r; ball.velY = std::abs(ball.velY); nBounce++; }  //Sopra
        if (ball.posX - ball.r < 0.0f) { ball.posX = ball.r; ball.velX = std::abs(ball.velX); nBounce++; }  //Sinistra
        if (ball.posX + ball.r > SCREEN_WIDTH) { 
            ball.posX = SCREEN_WIDTH - ball.r; ball.velX = -std::abs(ball.velX); nBounce++;                 //Destra
        }

        //Collisioni con i mattoncini
        bool hitSideX = false, hitSideY = false, hasHitted = false;
        for (size_t i = 0; i < realBricks.size() && !hasHitted; ++i)
        {
            //Se il mattoncino è attivo e collide con la pallina, lo segniamo come colpito (solo nella maschera) e rimbalziamo: la simulazione continua
            if (brickAlive[i] && realBricks[i].on && CheckAABB(ball, realBricks[i], hitSideX, hitSideY)) {
                if (hitSideX) ball.velX = -ball.velX;
                if (hitSideY) ball.velY = -ball.velY;
                brickAlive[i] = false;
                hasHitted = true;
                nBounce++;
            }
        }

        //Salva il segmento calcolato
        segments.push_back({ prevX, prevY, ball.posX, ball.posY, isBest });

        //Se ha raggiunto il fondo dello schermo, termina la simulazione
        if (ball.posY + ball.r >= SCREEN_HEIGHT)
        {
            //Interpolazione per trovare il corretto punto in mezzo al segmento con y = SCREEN_HEIGHT
            float denom = ball.posY - prevY;
            if (std::abs(denom) > 0.001f) { 
                float t = (SCREEN_HEIGHT - prevY) / denom; 
                if (isBest) landingX = prevX + t * (ball.posX - prevX);   //landingX appartiene solo alla pallina più pericolosa
            }
            else if (isBest) { landingX = ball.posX; }
            hasReachedEndScreen = true;
        }
    }

    //Posizione relativa di ogni segmento nella relativa traiettoria (0 -> inizio, 1 -> fine), usata per la dissolvenza
    const float count = static_cast<float>(segments.size() - firstSegment);
    for (size_t k = firstSegment; k < segments.size(); ++k) { segments[k].t = static_cast<float>(k - firstSegment) / count; }
}

void BallPredictor::Render(Renderer2D& r2d) const
{
    if (segments.empty()) return;

    //Offset di animazione: scorre di DASH_PERIOD pixel al secondo * DASH_SPEED
    float offset = std::fmod(dashTimer * PREDICTOR_DASH_SPEED * PREDICTOR_DASH_PERIOD, PREDICTOR_DASH_PERIOD);

    float accumulated = offset;

    for (size_t i = 0; i < segments.size(); ++i)
    {
        const Segment& s = segments[i];

        float dx = s.xf - s.xi;
        float dy = s.yf - s.yi;
        float len = std::sqrt(dx * dx + dy * dy);   //Lunghezza / Norma del vettore (i, f)

        if (len > 0.0f) {
            
            //Logica di visualizzazione del tratteggio, calcola DASH_ON (parte accesa) e disegna solo quella parte
            float phase = std::fmod(accumulated, PREDICTOR_DASH_PERIOD);
            bool  dashOn = (phase < PREDICTOR_DASH_ON);
            accumulated += len;

            if (!dashOn) {
                //Alpha diminuisce verso la fine della traiettoria (s.t in [0, 1])
                r2d.DrawLine(s.xi, s.yi, s.xf, s.yf, PREDICTOR_LINE_THICKNESS, COLOR_PREDICTOR_LINE(s.t, s.isBest));
            }
        }
    }

    //Cerchio nel punto previsto (landingX è sempre quello della pallina più pericolosa)
    if (landingX >= 0.0f)
    {
        float pulse = PREDICTOR_END_PULSE(dashTimer);   //Per variare la grandezza del cerchio ("intensità")
        r2d.DrawCircle(landingX, PREDICTOR_END_Y(pulse), PREDICTOR_END_RADIUS(pulse), COLOR_PREDICTOR_END(pulse, true));
    }
}
