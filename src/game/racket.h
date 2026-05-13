#pragma once
#include "../ui/inputManager.h"

class Racket {
public:
	float posX, posY, w, h, speed;

    Racket();
    ~Racket() = default;

    void Update(float deltaTime, InputManager input);
    void WidthBonus();
    void Reset();

    //Utile per la gestione della sovrapposizione in CheckAABB(), definisce le dimensioni della racchetta
    float Left()   const { return posX; }
    float Right()  const { return posX + w; }
    float Top()    const { return posY; }
    float Bottom() const { return posY + h; }

    //Utile per capire l'intensità del rimbalzo (maggiore se collide lontano dal centro)
    float CenterX() const { return posX + w * 0.5f; }
};