#pragma once
#include "../game/ball.h"
#include "../game/level.h"
#include "../render/renderer2d.h"
#include <vector>

struct Segment { float xi, yi, xf, yf; };

class BallPredictor
{
public:
    BallPredictor() = default;
    ~BallPredictor() = default;

    //Rule of five: se definisco distruttore, devo definire anche copy e move (FC++, lezione 7)
    BallPredictor(const BallPredictor&) = delete;
    BallPredictor& operator=(const BallPredictor&) = delete;
    BallPredictor(BallPredictor&&) = delete;
    BallPredictor& operator=(BallPredictor&&) = delete;

    void Update(const std::vector<Ball>& balls, const Level& level, float deltaTime);
    void Render(Renderer2D& r2d) const;

    float GetLandingX() const { return landingX; }
    bool IsActive() const { return !segments.empty(); }

private:
    void Simulate(Ball ball, const Level& level);

    const void Predict(const std::vector<Ball>& balls, const Level& level);

    std::vector<Segment> segments;
    float landingX = -1.0f;
    float dashTimer = 0.0f;
    int mode = 0;   //0 = all, 1 = solo la più pericolosa   //TODO: da settare tramite schermata iniziale
};