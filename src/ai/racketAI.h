#pragma once
#include "../ai/ballPredictor.h"
#include "../data.h"
#include "../game/ball.h"
#include "../game/bonus.h"
#include "../game/brick.h"
#include "../game/level.h"
#include "../game/racket.h"
#include <cstdlib>
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

    //Restituisce la pallina cadente (velY > 0) e più vicina al bordo inferiore (posY maggiore)
    const Ball* FindMostDangerousBall(const std::vector<Ball>& balls) const;

    //Calcola la pericolosità di una pallina (0 := pallina lontana dalla racchetta, 1 := pallina vicina alla racchetta)
    float ComputeUrgency(const Ball* dangerousBall, const Racket& racket) const;

    //Gerarchia d'importanza tra i bonus (Ball > Racket > Speed)
    float BonusWeight(BonusType t) const;

    //Gerarchia d'importanza tra i mattoncini (Green > Red > Orange = Blue)
    float BrickWeight(BrickType t) const;

    //Restituisce la x del bonus più conveniente (importanza / distanza) o -1 se non ci sono bonus (/non sono raggiungibili)
    float BestBonusTargetX(const std::vector<BonusItem>& bonuses, const Racket& racket) const;

    //Restituisce la x del mattoncino più conveniente (50% bonus, 30% altezza, 20% distanza) o -1 se non ci sono mattoncini
    float BestBrickTargetX(const Level& level, const Racket& racket) const;

    //Bilancia le priorità (palla > bonus > posizione strategica)
    float AdaptiveTarget(float landingX, const std::vector<BonusItem>& bonuses, const std::vector<Ball>& balls, const Level& level, const Racket& racket) const;

    //Random error tra [-AI_ERROR_RANGE, AI_ERROR_RANGE]
    float RandomError() const { return (static_cast<float>(rand() % 2) - 1.0f) * AI_ERROR_RANGE; }
};