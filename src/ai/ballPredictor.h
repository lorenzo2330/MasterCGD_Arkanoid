#pragma once
#include "../game/ball.h"
#include "../game/level.h"
#include "../render/renderer2d.h"
#include <vector>

struct Segment { float xi, yi, xf, yf; bool isBest = false; float t = 0.0f; };  //t = posizione relativa nella traiettoria (0 inizio, 1 fine), per la dissolvenza

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
    bool IsActive() const { return !segments.empty() && mode != 0; }

    void SetMode(int trajectoryMode) { mode = trajectoryMode; } //0 -> Disattivata | 1 -> Più pericolosa | 2 -> Tutte

    //Pallina ancora salvabile: attiva, in discesa (velY > 0) e non ancora oltre la racchetta
    static bool IsThreat(const Ball& b) { return b.on && b.velY > 0.0f && b.TimeToRacket() >= 0.0f; }

    //Pallina più pericolosa = quella che raggiunge per prima la racchetta (tempo d'impatto minimo), non la più bassa (nullptr se non ce ne sono)
    //Unico punto di verità: lo usano sia il predittore sia la RacketAI
    static const Ball* FindMostDangerous(const std::vector<Ball>& balls);
    
private:
    void Simulate(Ball ball, const Level& level, bool isBest = false);

    void Predict(const std::vector<Ball>& balls, const Level& level);

    std::vector<Segment> segments;
    float landingX = -1.0f;
    float dashTimer = 0.0f;
    
    int  mode = 0;
};
