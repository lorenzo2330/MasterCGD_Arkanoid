#pragma once
#include "brick.h"
#include <vector>

class Level {
public:

    Level() = default;
    ~Level() = default;

    void GenerateRandomGrid();

    std::vector<Brick>& GetBricks() { return bricks; }
    bool AllDestroyed() const;

private:
    std::vector<Brick> bricks;

    BrickType RandomType() const;
};