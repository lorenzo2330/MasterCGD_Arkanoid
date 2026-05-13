#pragma once
#include "../colors.h"
#include "../data.h"
#include "../render/textRenderer.h"
#include <string>

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

        std::wstring scoreStr = L"SCORE  " + std::to_wstring(score);
        textRenderer->DrawCenteredText(scoreStr, HUD_SCORE_X, HUD_SCORE_Y, HUD_SCORE_WIDTH, HUD_SCORE_HEIGHT, COLOR_HUD_TEXT);

        std::wstring levelStr = L"LEVEL  " + std::to_wstring(level);
        textRenderer->DrawCenteredText(levelStr, HUD_LEVEL_X, HUD_LEVEL_Y, HUD_LEVEL_WIDTH, HUD_LEVEL_HEIGHT, COLOR_HUD_TEXT);

        textRenderer->EndDraw();
    }

private:
    TextRenderer* textRenderer = nullptr;
    int score = 0, level = 0;
};