#pragma once
#include "../data.h"
#include "racket.h"

class Ball {
public:
    float posX, posY, velX, velY, r;
    
    bool on;

    Ball() = default;
    Ball(float startX, float startY, float speed);
    ~Ball() = default;

    void Update(float deltaTime, bool& hitBottom);  //Se hitBottom = true -> game over
    void UpdateBeforeStart(Racket racket);          //Prima che inizi il livello (si muove sopra la racchetta)

    void Bounce(bool onX);

    void IncreaseSpeed() { velX *= BONUS_SPEED_MULTIPLIER; velY *= BONUS_SPEED_MULTIPLIER; }

    //Utile per la gestione della sovrapposizione in CheckAABB(), definisce le dimensioni di una pallina
    float Left()   const { return posX - r; }
    float Right()  const { return posX + r; }
    float Top()    const { return posY - r; }
    float Bottom() const { return posY + r; }

};