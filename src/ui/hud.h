#pragma once
#include "../colors.h"
#include "../data.h"
#include "../render/textRenderer.h"
#include "../string.h"

class HUD
{
public:
    HUD() = default;
    ~HUD() = default;

    void Init(TextRenderer* tr) { textRenderer = tr; }

    //Funzioni per aggiornare l'hud
    void SetScore(int score) { this->score = score; }
    void SetLevel(int level) { this->level = level; }
    void AddScore(int points) { score += points; }
    
    int GetScore() const { return score; }

    void Render() const
    {
        if (!textRenderer) return;

        textRenderer->BeginDraw();

        //DrawCenteredText per evitare di calcolare ogni volta la posizione dinamicamente 

        textRenderer->DrawCenteredText(S_SCORE(score), HUD_SCORE_X, HUD_SCORE_Y, HUD_SCORE_WIDTH, HUD_SCORE_HEIGHT, COLOR_HUD_TEXT);

        textRenderer->DrawCenteredText(S_LEVEL(level), HUD_LEVEL_X, HUD_LEVEL_Y, HUD_LEVEL_WIDTH, HUD_LEVEL_HEIGHT, COLOR_HUD_TEXT);

        textRenderer->EndDraw();
    }

private:
    TextRenderer* textRenderer = nullptr;
    int score = 0, level = 0;
};