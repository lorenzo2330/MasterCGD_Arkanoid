#define NOMINMAX
#include "ballPredictor.h"
#include "racketAI.h"
#include "../data.h"
#include "../game/ball.h"
#include "../game/bonus.h"
#include "../game/brick.h"
#include "../game/level.h"
#include "../game/racket.h"
#include <algorithm>
#include <cstdlib>
#include <vector>

const Ball* RacketAI::FindMostDangerousBall(const std::vector<Ball>& balls) const
{
    const Ball* best = nullptr;
    float bestY = -1.0f;

    for (const Ball& b : balls) { if (b.on && b.velY > 0.0f && b.posY > bestY) { bestY = b.posY; best = &b; } }

    return best;
}

float RacketAI::ComputeUrgency(const Ball* dangerousBall, const Racket& racket) const
{
    if (!dangerousBall) return 0.0f;   //Se non ci sono palline pericolose (es tutte stanno "salendo")

    //Calcola la distanza tra la racchetta e la posizione della pallina più pericolosa
    float distance = racket.posY - dangerousBall->posY;
    
    //Clampa tra 0 e 1 il rapporto tra distanza della pallina dalla racchetta e soglia di urgenza 
    float urgency = 1.0f - std::clamp(distance / AI_URGENCY_DISTANCE, 0.0f, 1.0f);

    return urgency;
}

float RacketAI::BonusWeight(BonusType t) const
{
    if (t == BonusType::Ball) { return AI_BONUS_BALL_WEIGHT; }
    if (t == BonusType::Racket) { return AI_BONUS_RACKET_WEIGHT; }
    if (t == BonusType::Speed) { return AI_BONUS_SPEED_WEIGHT; }

    return AI_BONUS_DEFAULT_WEIGHT;
}

float RacketAI::BrickWeight(BrickType t) const
{
    if (t == BrickType::Green) { return AI_BRICK_GREEN_WEIGHT; }
    if (t == BrickType::Red) { return AI_BRICK_RED_WEIGHT; }

    return AI_BRICK_DEFAULT_WEIGHT;
}

float RacketAI::BestBonusTargetX(const std::vector<BonusItem>& bonuses, const Racket& racket) const
{
    float bestScore = -1.0f;
    float bestX = -1.0f;

    for (const BonusItem& b : bonuses)
    {
        if (!b.on) continue;

        //Distanza tra bonus e racchetta
        float bonusX = (b.posX + b.w * 0.5f);
        float dist = std::abs(bonusX - racket.CenterX());

        //Calcola il tempo rimanente prima che il bonus superi la racchetta (prima che diventi non più raggiungibile)
        float time = (RACKET_POSY - b.posY) / b.velY;

        //Calcola la massima distanza raggiungibile nel lasso di tempo rimanente (0.5 per la "metà racchetta più vicina")
        float reachability = RACKET_SPEED * time + racket.w * 0.5f;

        //Se è raggiungibile prima che venga perso
        if (time >= 0.0f && dist < reachability) {

            //Importanza del bonus
            float bonusWeight = BonusWeight(b.type);

            //Punteggio finale (importanza / distanza) (o importanza e basta se distanza = 0 (già sotto))
            float score;

            if (dist == 0) { score = bonusWeight; } else { score = bonusWeight / dist; }

            //Se è migliore dei precedenti, aggiorniamo il best
            if (score > bestScore) { bestScore = score; bestX = bonusX; }
        }
    }
    return bestX;
}

float RacketAI::BestBrickTargetX(const Level& level, const Racket& racket) const
{
    float bestScore = -1.0f;
    float bestX = -1.0f;

    for (const Brick& b : level.GetBricksConst())
    {
        if (!b.on) continue;

        //Importanza del mattoncino
        float typeWeight = BrickWeight(b.type);

        //Altezza del mattoncino (meglio più basso)
        float heightWeight = b.posY / SCREEN_HEIGHT;

        //Punteggio sulla distanza
        float posBrick = b.posX + b.w * 0.5f;
        float distanceRacketBrick = std::abs(posBrick - racket.CenterX());
        float distanceWeight = 1.0f - std::clamp(distanceRacketBrick * 3.0f / SCREEN_WIDTH, 0.0f, 1.0f);    //* 3 per penalizzare quelli lontani

        //Punteggio finale (50% al tipo di mattoncino, poi 30% alla sua altezza e 20% alla distanza con la racchetta)
        float score = typeWeight * (0.5f + 0.3f * heightWeight + 0.2f * distanceWeight);

        //Se è migliore dei precedenti, aggiorniamo il best
        if (score > bestScore) { bestScore = score; bestX = posBrick; }
    }
    return bestX;
}

float RacketAI::AdaptiveTarget(float landingX, const std::vector<BonusItem>& bonuses, const std::vector<Ball>& balls, const Level& level, const Racket& racket) const
{
    //-----------------------------------------Priorità 1: colpire la pallina

    //Calcola la pallina più pericolosa (più bassa)
    const Ball* dangerousBall = FindMostDangerousBall(balls);

    //Calcola l'urgenza della pallina più pericolosa (distanza dalla racchetta)
    const float urgency = ComputeUrgency(dangerousBall, racket);

    //Se "l'urgenza" è sopra la soglia, si adopera per colpirla (muovendosi al landingX (se non c'è, rimaniamo fermi))
    if (urgency >= AI_URGENCY_THRESHOLD) { if (landingX >= 0.0f) { return landingX + RandomError(); } return -1.0f; }

    //(la pallina è abbastanza distante, prossima priorità)

    //-----------------------------------------Priorità 2: raccogliere i bonus

    //Calcola il bonus più conveniente
    float bonusX = BestBonusTargetX(bonuses, racket);
    if (bonusX >= 0.0f)
    {
        //Se c'è una pallina che sta cadendo, controlliamo che il prendere il bonus non precluda il prendere la pallina
        if (landingX >= 0.0f)
        {
            float safeTime = AI_URGENCY_DISTANCE / RACKET_SPEED; //Stima sul tempo necessario per raggiungere una pallina

            //Se c'è una pallina, il safeTime diventa il tempo prima che essa cada
            if (dangerousBall) { safeTime = (SCREEN_HEIGHT - dangerousBall->posY) / dangerousBall->velY; }

            //Calcola la distanza necessaria per prendere il bonus e poi raggiungere la pallina
            float distanceRacketBonus = std::abs(bonusX - racket.CenterX());
            float distanceBonusBall = std::abs(landingX - bonusX);
            float totalDistance = distanceRacketBonus + distanceBonusBall;

            //Se la distanza è copribile entro il tempo necessario per prendere la pallina, prendiamo il bonus
            if (totalDistance / RACKET_SPEED <= safeTime * AI_BONUS_TIME_MARGIN) { return bonusX; }

            //Se la distanza non è raggiungibile, andiamo direttamente a prendere la pallina
            return landingX + RandomError();;
        }

        //Se non ci sono palline che stanno cadendo, andiamo direttamente a prendere il bonus
        return bonusX;
    }

    //(la pallina è abbastanza distante && non ci sono bonus (/non ci sono bonus raggiungibili)

    //-----------------------------------------Priorità 3: colpire i mattoncini

    //Calcola la posizione del mattoncino più conveniente e si posiziona a metà tra la posizione del mattoncino e della racchetta
    float brickX = BestBrickTargetX(level, racket);
    if (brickX >= 0.0f) { return brickX; }

    //Nessuna delle priorità è rispettata (pallina che sale * no bonus * no mattoncini (-> fine livello)) -> rimane ferma
    return -1.0f;
}

float RacketAI::ComputeTarget(const Level& level, const std::vector<Ball>& balls, const std::vector<BonusItem>& bonuses, const BallPredictor& predictor, const Racket& racket, float deltaTime)
{
    if (mode == RacketAIMode::Off) return -1.0f;

    const float landingX = predictor.GetLandingX(); //Dove il ballPredictor stima che atterrerà la pallina più pericolosa

    float targetX = -1.0f;

    //Segue solo la previsione della pallina
    if (mode == RacketAIMode::BallPriority) { if (landingX >= 0.0f) { targetX = landingX + RandomError(); } }
    
    //Bilancia pallina e bonus
    if (mode == RacketAIMode::Adaptive) { targetX = AdaptiveTarget(landingX, bonuses, balls, level, racket); }

    //Clamp per evitare che il centro della racchetta esca dallo schermo
    if (targetX >= 0.0f) { targetX = std::clamp(targetX, racket.w * 0.5f, SCREEN_WIDTH - racket.w * 0.5f); }

    return targetX;
}

