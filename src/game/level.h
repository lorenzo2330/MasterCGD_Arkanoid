#pragma once
#include "brick.h"
#include <vector>

class Level {
public:

    Level() = default;
    ~Level() = default;

    void GenerateRandomGrid(int currentLevel);

    std::vector<Brick>& GetBricks() { return bricks; }

    const std::vector<Brick>& GetBricksConst() const { return bricks; } //Usato dal BallPredictor per la previsione di traiettoria

    bool AllDestroyed() const;

private:
    std::vector<Brick> bricks;

    BrickType RandomType() const;
};