#pragma once
#include "../colors.h"

enum class BonusType { None, Racket, Ball, Speed };

class BonusItem {
public:
    float posX, posY, w, h, velY;
    BonusType type;
    bool on;

    BonusItem() = default;
    BonusItem(float brickX, float brickY, float brickW, BonusType t);
    ~BonusItem() = default;

    void Update(float deltaTime);

    //Utile per la gestione della sovrapposizione in CheckAABB(), definisce le dimensioni di un bonus
    float Left()   const { return posX; }
    float Right()  const { return posX + w; }
    float Top()    const { return posY; }
    float Bottom() const { return posY + h; }

    Color GetColor() const;

    float GetCenterX() const { return posX + w * 0.5f; }
    float GetCenterY() const { return posY + h * 0.5f; }
};