#pragma once
#include "bonus.h"
#include "../colors.h"

enum class BrickType { Blue, Red, Green };

class Brick {
public:
    float posX, posY, w, h;
    BrickType type;
    int hp;
    bool on;
    

    Brick() = default;
    Brick(float x, float y, BrickType t);
    ~Brick() = default;

    BonusType Hit();

    Color GetColor() const;

    float Left()   const { return posX; }
    float Right()  const { return posX + w; }
    float Top()    const { return posY; }
    float Bottom() const { return posY + h; }

private:
    BonusType bonusDrop;

    BonusType GetRandomBonusType();

};