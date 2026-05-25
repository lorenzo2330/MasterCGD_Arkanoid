#define NOMINMAX
#include "colors.h"
#include "data.h"
#include "game.h"
#include "game/ball.h"
#include "game/bonus.h"
#include "game/brick.h"
#include "game/utils.h"
#include "render/lightBallRenderer.h"
#include "sound/soundBank.h"
#include "sound/soundManager.h"
#include "ui/gameOverScreen.h"
#include "ui/startScreen.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <Windows.h>

bool Game::Init(HWND hwnd)
{
    srand(static_cast<unsigned>(time(nullptr)));    //Randomizza il seed per i numeri casuali

    //Inizializzazione del renderer principale (crea device, deviceContext, swapChain, renderTargetView, viewport)
    if (!renderer.Init(hwnd)) { return ERR("FAIL: Renderer DX11\n"); }

	//Inizializzazione del renderer per il gioco (usa output di renderer, crea shader, inputLayout e vertexBuffer)
    if (!renderer2D.Init(renderer.GetDevice(), renderer.GetContext(), L"shaders\\")) { return ERR("FAIL: Renderer2D (shader?)\n"); }

    //Inizializzazione del renderer per le scritte a schermo
    if (!textRenderer.Init(renderer.GetDevice(), renderer.GetSwapChain())) { return ERR("FAIL: TextRenderer (D2D)\n"); }

    //Inizializzazione render modalità lightBall
    if (!lightBallRenderer.Init(renderer, L"shaders\\")) { ERR("FAIL: LightBallMode non inizializzata\n"); }
    
    //Inizializzazione audio
    if (SoundManager::Get().Init()) { ERR("FAIL: AudioManager \n"); }

    hud.Init(&textRenderer);
    gameOverScreen.Init(&textRenderer, &renderer2D);
    startScreen.Init(&textRenderer, &renderer2D);

    phase = GamePhase::StartScreen;
    
    return true;
}

void Game::Shutdown()
{
	//Rilascia le risorse
    SoundManager::Get().Shutdown();
    lightBallRenderer.Shutdown();
    textRenderer.Shutdown();
    renderer2D.Shutdown();
    renderer.Shutdown();
}

void Game::ApplySettings(const StartScreenResult& result)
{
    ballPredictor.SetMode(result.trajectoryMode);
    racketAI.SetMode(result.racketAIMode);
    lightBallMode = result.isLightBallMode;
}

void Game::SpawnBall() { balls.emplace_back(racket.CenterX(), racket.Top() - BALL_START_DISTANCE, BALL_START_SPEED * speedMultiplier); }

void Game::NewLevel()
{
    hud.SetLevel(++currentLevel);
    level.GenerateRandomGrid(currentLevel);
    bonuses.clear();
    balls.clear();
    racket.Reset();
    speedMultiplier = 1.0f + SPEED_INCREASER * currentLevel;
    SpawnBall();
    levelHasToStart = true;
}

void Game::ApplyBonus(BonusType t)
{
	if (t == BonusType::Speed) BonusIncreaseSpeed();
    if (t == BonusType::Racket) BonusLargerRacket();
    if (t == BonusType::Ball) BonusDuplicateBalls();
}

void Game::BonusDuplicateBalls() {
    int current = (int)balls.size();
    for (int i = 0; i < current; i++) {
        Ball clone = balls[i];
        clone.velX = -clone.velX;   //Rimbalza in maniera speculare
        balls.push_back(clone);
    }
}

void Game::BonusIncreaseSpeed() { speedMultiplier *= BONUS_SPEED_MULTIPLIER; for (Ball& b : balls) { b.IncreaseSpeed(); } }

void Game::BonusLargerRacket() { racket.WidthBonus(); }

void Game::HandleGameOverInput()
{
    GameOverScreen::Action action = gameOverScreen.HandleInput(input);

    if (action == GameOverScreen::Action::Restart) { 
        //Torna alla StartScreen per ri-scegliere le impostazioni
        phase = GamePhase::StartScreen;
        currentLevel = 0;
        hud.SetScore(0);
        return;
    }
    if (action == GameOverScreen::Action::Quit) { isRunning = false; return; }
}

void Game::UpdateCollisions()
{
    std::vector<Brick>& bricks = level.GetBricks();
    bool hitSideX, hitSideY;

    for (Ball& ball : balls) {
        if (ball.on) {
            //Controllo se collide con la racchetta
            if (CheckAABB(ball, racket, hitSideX, hitSideY)) {
                ball.velY = -std::abs(ball.velY);   //Rimbalza verticalmente

                //Direzione e intensità del colpo (in base alla distanza dal centro della racchetta)
                float rel = (ball.posX - racket.CenterX()) / (racket.w * 0.5f);

                //(Direzione e Intensità) * Velocità verticale * Fattore di deviazione
                ball.velX = rel * std::abs(ball.velY) * BALL_DEVIATION;

                SoundManager::Get().Play(SoundID::BallHitRacket);
            }

            //Controllo di collisione con ogni mattoncino attivo
			bool hitBrick = false;              //Colpisce solo un mattincino per frame
            for (Brick& brick : bricks) {
                if (!hitBrick && brick.on && CheckAABB(ball, brick, hitSideX, hitSideY)) {
                    if (hitSideX) ball.Bounce(true);
                    if (hitSideY) ball.Bounce(false);

                    BonusType drop = brick.Hit();

                    //Aggiorna punteggio
                    if (!brick.on) {
                        if (brick.type == BrickType::Blue) hud.AddScore(SCORE_BLUE);
                        if (brick.type == BrickType::Red) hud.AddScore(SCORE_RED);
                        if (brick.type == BrickType::Green) hud.AddScore(SCORE_GREEN);
                        SoundManager::Get().Play(SoundID::BrickDestroyed);
                    }
                    else { SoundManager::Get().Play(SoundID::BrickRedHitted); }

                    if (drop != BonusType::None) bonuses.emplace_back(brick.posX, brick.posY, brick.w, drop);

                    hitBrick = true;
                }
            }
        }
    }
}

void Game::UpdateRacket(float deltaTime) {
    if (racketAI.IsActive())
    {
        //L'AI calcola il centro X verso cui spostarsi
        float targetCenterX = racketAI.ComputeTarget(level, balls, bonuses, ballPredictor, racket, deltaTime);
        
        //TODO vedere se serve la dead zone
        if (targetCenterX >= 0.0f)
        {
            float diff = targetCenterX - racket.CenterX();
            float maxMove = RACKET_SPEED * deltaTime;

            constexpr float DEAD_ZONE = 10.0f;

            // Dead zone: se siamo già abbastanza vicini, non oscillare 
            if (std::abs(diff) > DEAD_ZONE)
            {
                if (std::abs(diff) <= maxMove)
                    racket.posX = targetCenterX - racket.w * 0.5f;
                else
                    racket.posX += (diff > 0.0f ? 1.0f : -1.0f) * maxMove;

                racket.posX = std::max(0.0f, std::min(racket.posX, SCREEN_WIDTH - racket.w));
            }
        }
    }
    else
    {
        //Controllo manuale normale
        racket.Update(deltaTime, input);
    }
}

void Game::Update(float deltaTime)
{
    GamePhase framePhase = phase;   //Per evitare che si entri in più if a causa del variare di phase durante l'update

    if (!isRunning) return;

    if (framePhase == GamePhase::StartScreen)
    {
        if (startScreen.HandleInput(input))
        {
            //L'utente ha premuto Conferma: legge le impostazioni e avvia il gioco
            ApplySettings(startScreen.GetResult());
            NewLevel();
            phase = GamePhase::Playing;
        }
    }

    if (framePhase == GamePhase::GameOver) { HandleGameOverInput(); }

    if (framePhase == GamePhase::Playing) {
        if (input.IsKeyDown(VK_SPACE) || racketAI.IsActive()) { levelHasToStart = false; }

        //Update della racchetta
        //racket.Update(deltaTime, GetInput());
        UpdateRacket(deltaTime);

        //Update su ogni palla
        bool anyHitBottom = false;
        for (Ball& ball : balls) {
            bool hitBottom = false;
            if (levelHasToStart) { ball.UpdateBeforeStart(racket); }
            else { ball.Update(deltaTime, hitBottom); }
            if (hitBottom) anyHitBottom = true;
        }

        //Sposta le palle "morte" in fondo al vettore, restituisce iteratore alla prima "morta"
        std::vector<Ball>::iterator lastBall = std::remove_if(balls.begin(), balls.end(), [](const Ball& b) { return !b.on; });

        //Rimuove dal vettore tutto ciò che va da last alla fine del vettore
        balls.erase(lastBall, balls.end());

        //Se tutte le palline sono "morte" -> game over
        if (balls.empty() && anyHitBottom) { 
            phase = GamePhase::GameOver; 
            SoundManager::Get().Play(SoundID::Lose);
            gameOverScreen.SetFinalScore(hud.GetScore()); 
            return; 
        }

        //Controlla collisioni
        UpdateCollisions();

        //Update bonus che stanno cadendo
        for (BonusItem& bonus : bonuses) {
            bonus.Update(deltaTime);

            bool tmp;   //Serve giusto per passarlo alla funzione, non viene realmente usato
            if (bonus.on && CheckAABB(bonus, racket, tmp, tmp)) { 
                ApplyBonus(bonus.type); 
                SoundManager::Get().Play(SoundID::Bonus);
                bonus.on = false; 
            }
        }

        //Sposta i bonus "morti" in fondo al vettore, restituisce iteratore al primo "morto"
        std::vector<BonusItem>::iterator lastBonus = std::remove_if(bonuses.begin(), bonuses.end(), [](const BonusItem& b) { return !b.on; });

        //Rimuove dal vettore tutto ciò che va da last alla fine del vettore
        bonuses.erase(lastBonus, bonuses.end());

        //Tutti i blocchi distrutti -> nuovo livello
        if (level.AllDestroyed()) { SoundManager::Get().Play(SoundID::LevelUp); NewLevel(); }

        ballPredictor.Update(balls, level, deltaTime);
    }

    input.EndFrame();   //Consuma eventuali input rimanenti (click o altro)
}

void Game::Render()
{
    GamePhase framePhase = phase;   //Per evitare che si entri in più if a causa del variare di phase durante il render

    bool isLightBall = lightBallMode && lightBallRenderer.IsReady() && framePhase == GamePhase::Playing;

    //Invoca il render.BeginFrame() relativo alla modalità di gioco
    if (isLightBall) { lightBallRenderer.BeginFrame(renderer); } else { renderer.BeginFrame(); }

    if (framePhase == GamePhase::StartScreen) { startScreen.Render(input); }

    if (framePhase == GamePhase::Playing) {
        //Renderizza i mattoncini
        for (const Brick& b : level.GetBricks()) { if (b.on) { renderer2D.DrawRect(b.posX, b.posY, b.w, b.h, b.GetColor()); } }

        //Renderizza i bonus che stanno cadendo
        for (const BonusItem& b : bonuses) { if (b.on) { renderer2D.DrawRect(b.posX, b.posY, b.w, b.h, b.GetColor()); } }

        //Renderizza le traiettorie (prima del rendering delle palline, così appare "sotto")
        if (ballPredictor.IsActive()){ ballPredictor.Render(renderer2D); }

        //Renderizza le palle
        for (const Ball& ball : balls) { if (ball.on) { renderer2D.DrawCircle(ball.posX, ball.posY, ball.r, COLOR_BALL); } }

        //Renderizza la raccchetta
        renderer2D.DrawRect(racket.posX, racket.posY, racket.w, racket.h, COLOR_RACKET);

        //Renderizza HUD
        hud.Render();
    }

    if (framePhase == GamePhase::GameOver) { gameOverScreen.Render(input); }    //Renderizza schermata di game over
        
    //Invoca il render.EndFrame() aggiuntivo nel caso di LightBallMode
    if (isLightBall) { BallLightData ld = BuildLightData(); lightBallRenderer.EndFrame(renderer, ld); }

    renderer.EndFrame();
}

BallLightData Game::BuildLightData() const
{
    //Struttura con i dati necessari per il corretto rendering in lightBallRenderer

    BallLightData ld = {};
    ld.ballLightRadius = LIGHTBALL_BALL_LIGHT_RADIUS;
    ld.bonusLightRadius = LIGHTBALL_BONUS_LIGHT_RADIUS;
    ld.racketLightRadius = LIGHTBALL_RACKET_LIGHT_RADIUS;
    ld.ballPower = LIGHTBALL_BALL_POWER;
    ld.racketPower = LIGHTBALL_RACKET_POWER;
    ld.bonusPower = LIGHTBALL_BONUS_POWER;

    //Palline, memorizza le posizioni in coordinate NDC (usate dallo shader)
    int count = std::min(static_cast<int>(balls.size()), LIGHTBALL_MAX_BALL);
    ld.ballCount = static_cast<float>(count);
    for (int i = 0; i < count; i++) { 
        ld.ballPositions[i].x = ToNDC_X(balls[i].posX); 
        ld.ballPositions[i].y = ToNDC_Y(balls[i].posY); 
    }

    //Bonus, memorizza le posizioni in coordinate NDC (usate dallo shader)
    int bonusCount = std::min(static_cast<int>(bonuses.size()), LIGHTBALL_MAX_BONUS);
    ld.bonusCount = static_cast<float>(bonusCount);
    for (int i = 0; i < bonusCount; i++) { 
        ld.bonusPositions[i].x = ToNDC_X(bonuses[i].GetCenterX()); 
        ld.bonusPositions[i].y = ToNDC_Y(bonuses[i].GetCenterY()); 
    }

    //Racchetta, memorizza il centro in coordinate NDC (usate dallo shader)
    ld.racketPosition.x = ToNDC_X(racket.CenterX());
    ld.racketPosition.y = ToNDC_Y(racket.CenterY());

    return ld;
}



