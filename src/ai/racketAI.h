#define NOMINMAX
#pragma once
#include "../ai/ballPredictor.h"
#include "../data.h"
#include "../game/ball.h"
#include "../game/bonus.h"
#include "../game/brick.h"
#include "../game/level.h"
#include "../game/racket.h"
#include <cstdlib>
#include <limits>
#include <vector>

enum class RacketAIMode { Off, BallPriority, Adaptive };

class RacketAI
{
public:
    RacketAI() = default;
    ~RacketAI() = default;

    // Non copiabile né spostabile (possiede stato interno del timer)
    RacketAI(const RacketAI&) = delete;
    RacketAI& operator=(const RacketAI&) = delete;
    RacketAI(RacketAI&&) = delete;
    RacketAI& operator=(RacketAI&&) = delete;

    void SetMode(RacketAIMode m) { mode = m; }
    RacketAIMode GetMode() const { return mode; }
    bool IsActive() const { return mode != RacketAIMode::Off; }

    //Entry point, chiamato ogni frame da Game, restituisce la x alla quale deve muoversi (-1 se non deve muoversi)
    float ComputeTarget(const Level& level, const std::vector<Ball>& balls, const std::vector<BonusItem>& bonuses, const BallPredictor& predictor, const Racket& racket, float deltaTime);

private:
    
    RacketAIMode mode = RacketAIMode::Off;  //Modalità della racchetta, per variare le priorità

    //Mira: offset d'impatto desiderato sulla racchetta, in frazione della mezza racchetta (0 = centro, +-1 = bordo).
    //Il rimbalzo ha velX = rel * |velY| * BALL_DEVIATION: colpendo sempre al centro velX = 0 e la pallina rimbalza in verticale all'infinito.
    //Viene riestratto una volta per discesa (non a ogni frame: sarebbe rumore, non mira)
    static constexpr float AIM_MAX_REL = 0.4f;     //Massimo offset cercato (0.4 -> colpo di circa 19 gradi dalla verticale). 0 = disattiva la mira
    float aimRel = 0.0f;
    float lastDangerTime = (std::numeric_limits<float>::max)();   //Tempo d'impatto della pallina più pericolosa al frame precedente (max = nessuna)

    //Restituisce la pallina più pericolosa: quella che raggiunge per prima la racchetta (tempo d'impatto minimo), non la più bassa
    const Ball* FindMostDangerousBall(const std::vector<Ball>& balls) const;

    //Calcola la pericolosità di una pallina (0 := pallina lontana dalla racchetta, 1 := pallina vicina alla racchetta)
    float ComputeUrgency(const Ball* dangerousBall, const Racket& racket) const;

    //Gerarchia d'importanza tra i bonus (Ball > Racket > Speed)
    float BonusWeight(BonusType t) const;

    //Gerarchia d'importanza tra i mattoncini (Green > Red > Orange = Blue)
    float BrickWeight(BrickType t) const;

    //Restituisce la x del bonus più conveniente (importanza / distanza) o -1 se non ci sono bonus (/non sono raggiungibili)
    float BestBonusTargetX(const std::vector<BonusItem>& bonuses, const Racket& racket) const;

    //Restituisce la x del mattoncino più conveniente o -1 se non ci sono mattoncini.
    //score = pesoTipo * (0.5 + 0.3 * altezza + 0.2 * vicinanza): il tipo MOLTIPLICA, altezza e vicinanza lo correggono
    float BestBrickTargetX(const Level& level, const Racket& racket) const;

    //Bilancia le priorità (palla > bonus > posizione strategica)
    float AdaptiveTarget(float landingX, const std::vector<BonusItem>& bonuses, const std::vector<Ball>& balls, const Level& level, const Racket& racket) const;

    //Random uniforme in [-1, +1]
    float RandomSigned() const { return 2.0f * static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 1.0f; }

    //Random error uniforme in [-AI_ERROR_RANGE, +AI_ERROR_RANGE]
    float RandomError() const { return RandomSigned() * AI_ERROR_RANGE; }

    //x verso cui portare il centro della racchetta per intercettare la pallina in landingX con l'offset d'impatto scelto (aimRel).
    //Dal rimbalzo: rel = (ballX - centro) / (w/2)  ->  centro = landingX - rel * w/2
    float AimAt(float landingX, const Racket& racket) const { return landingX - aimRel * racket.w * 0.5f + RandomError(); }
};
