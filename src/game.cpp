#include "colors.h"
#include "data.h"
#include "game.h"
#include "game/ball.h"
#include "game/bonus.h"
#include "game/brick.h"
#include "game/utils.h"
#include "ui/gameOverScreen.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <Windows.h>

bool Game::Init(HWND hwnd)
{
    srand(static_cast<unsigned>(time(nullptr)));    //Randomizza il seed per i numeri casuali

    //Inizializzazione del renderer principale (crea device, deviceContext, swapChain, renderTargetView, viewport)
    if (!renderer.Init(hwnd)) { OutputDebugStringA("FAIL: Renderer DX11\n"); return false; }

	//Inizializzazione del renderer per il gioco (usa output di renderer, crea shader, inputLayout e vertexBuffer)
    if (!renderer2D.Init(renderer.GetDevice(), renderer.GetContext(), L"shaders\\")) { OutputDebugStringA("FAIL: Renderer2D (shader?)\n"); return false; }

    //Inizializzazione del renderer per le scritte a schermo
    if (!textRenderer.Init(renderer.GetDevice(), renderer.GetSwapChain())) { OutputDebugStringA("FAIL: TextRenderer (D2D)\n"); return false; }

    hud.Init(&textRenderer);
    
    gameOverScreen.Init(&textRenderer, &renderer2D);

    NewLevel();
    
    return true;
}

void Game::Shutdown()
{
	//Rilascia le risorse
    textRenderer.Shutdown();
    renderer2D.Shutdown();
    renderer.Shutdown();
}

void Game::SpawnBall() { balls.emplace_back(racket.CenterX(), racket.Top() - BALL_START_DISTANCE, BALL_START_SPEED * speedMultiplier); }

void Game::NewLevel()
{
    if (gameOver) { gameOver = false; currentLevel = 0; hud.SetScore(0); }
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
                    }

                    if (drop != BonusType::None) bonuses.emplace_back(brick.posX, brick.posY, brick.w, drop);

                    hitBrick = true;
                }
            }
        }
    }
}

void Game::HandleGameOverInput()
{
    GameOverScreen::Action action = gameOverScreen.HandleInput(input);

    if (action == GameOverScreen::Action::Restart) { NewLevel(); return; }
    if (action == GameOverScreen::Action::Quit) { isRunning = false; return; }
}

void Game::Update(float deltaTime)
{
    if (!isRunning) return;

    if (gameOver) { HandleGameOverInput(); input.EndFrame(); return; }

    if (input.IsKeyDown(VK_SPACE)) { levelHasToStart = false; }

    //Update della racchetta
    racket.Update(deltaTime, GetInput());

    //Update su ogni palla
    bool anyHitBottom = false;
    for (Ball& ball : balls) {
        bool hitBottom = false;
        if (levelHasToStart) { ball.UpdateBeforeStart(racket); } else { ball.Update(deltaTime, hitBottom); }
        if (hitBottom) anyHitBottom = true;
    }

    //Sposta le palle "morte" in fondo al vettore, restituisce iteratore alla prima "morta"
    std::vector<Ball>::iterator lastBall = std::remove_if(balls.begin(), balls.end(), [](const Ball& b) { return !b.on; });

    //Rimuove dal vettore tutto ciò che va da last alla fine del vettore
    balls.erase(lastBall, balls.end());

    //Se tutte le palline sono "morte" -> game over
    if (balls.empty() && anyHitBottom) { gameOver = true; gameOverScreen.SetFinalScore(hud.GetScore()); return; }

	//Controlla collisioni
    UpdateCollisions();

    //Update bonus che stanno cadendo
    for (BonusItem& bonus : bonuses) {
        bonus.Update(deltaTime);

		bool tmp;   //Serve giusto per passarlo alla funzione, non viene realmente usato
        if (bonus.on && CheckAABB(bonus, racket, tmp, tmp)) { ApplyBonus(bonus.type); bonus.on = false; }
    }

    //Sposta i bonus "morti" in fondo al vettore, restituisce iteratore al primo "morto"
    std::vector<BonusItem>::iterator lastBonus = std::remove_if(bonuses.begin(), bonuses.end(), [](const BonusItem& b) { return !b.on; });

    //Rimuove dal vettore tutto ciò che va da last alla fine del vettore
    bonuses.erase(lastBonus, bonuses.end());

    //Tutti i blocchi distrutti -> nuovo livello
    if (level.AllDestroyed()) NewLevel();

    ballPredictor.Update(balls, level, deltaTime);

    input.EndFrame();   //Consuma eventuali input rimanenti (click o altro)

}

void Game::Render()
{
    renderer.BeginFrame();

    if (!gameOver) {
        //Renderizza i mattoncini
        for (const Brick& b : level.GetBricks()) { if (b.on) { renderer2D.DrawRect(b.posX, b.posY, b.w, b.h, b.GetColor()); } }

		//Renderizza i bonus che stanno cadendo
        for (const BonusItem& b : bonuses) { if (b.on) { renderer2D.DrawRect(b.posX, b.posY, b.w, b.h, b.GetColor()); } }

        //Renderizza le traiettorie (prima del rendering delle palline, così appare "sotto")
        ballPredictor.Render(renderer2D);

        //Renderizza le palle
        for (const Ball& ball : balls) { if (ball.on) { renderer2D.DrawCircle(ball.posX, ball.posY, ball.r, COLOR_BALL); } }

		//Renderizza la raccchetta
        renderer2D.DrawRect(racket.posX, racket.posY, racket.w, racket.h, COLOR_RACKET);

        //Renderizza HUD
        hud.Render();
    }
    else { 
        //Renderizza schermata di game over
        gameOverScreen.Render(input); 
    }

    renderer.EndFrame();
}


