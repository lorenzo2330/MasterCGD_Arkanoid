#define NOMINMAX
#include "../colors.h"
#include "../data.h"
#include "../render/textRenderer.h"
#include "../render/renderer2d.h"
#include "inputManager.h"
#include "startScreen.h"
#include "uiHelper.h"
#include <Windows.h>
#include "../ai/racketAI.h"

void StartScreen::Init(TextRenderer* tr, Renderer2D* r2d)
{
    textRenderer = tr;
    renderer2D = r2d;

    settings.push_back({ L"Traiettoria palline", { L"Disattivata", L"Solo piu' pericolosa", L"Tutte" }, 0 });
    settings.push_back({ L"Racchetta controllata dall'AI?", { L"No", L"Si, priorita' palline", L"Si, adattiva" }, 0 });
    settings.push_back({ L"Pallina luminosa?", { L"No", L"Si"}, 0});
}

bool StartScreen::HandleInput(const InputManager& input)
{
    bool up = input.IsKeyDown(VK_UP) || input.IsKeyDown('W');
    bool down = input.IsKeyDown(VK_DOWN) || input.IsKeyDown('S');
    bool left = input.IsKeyDown(VK_LEFT) || input.IsKeyDown('A');
    bool right = input.IsKeyDown(VK_RIGHT) || input.IsKeyDown('D');
    bool enter = input.IsKeyDown(VK_RETURN);

    bool upEdge = up && !prevUp;
    bool downEdge = down && !prevDown;
    bool leftEdge = left && !prevLeft;
    bool rightEdge = right && !prevRight;
    bool enterEdge = enter && !prevEnter;

    prevUp = up;
    prevDown = down;
    prevLeft = left;
    prevRight = right;
    prevEnter = enter;

    int n = static_cast<int>(settings.size());
    if (upEdge)   selectedSetting = (selectedSetting - 1 + n) % n;
    if (downEdge) selectedSetting = (selectedSetting + 1) % n;

    if (!settings.empty())
    {
        Settings& s = settings[selectedSetting];
        int m = static_cast<int>(s.choices.size());

        if (leftEdge)  s.selectedIndex = (s.selectedIndex - 1 + m) % m;
        if (rightEdge) s.selectedIndex = (s.selectedIndex + 1) % m;

        if (input.IsClicked(LeftArrowRect(selectedSetting))) { s.selectedIndex = (s.selectedIndex - 1 + m) % m; }
        if (input.IsClicked(RightArrowRect(selectedSetting))) { s.selectedIndex = (s.selectedIndex + 1) % m; }
    }

    if (enterEdge || input.IsClicked(ConfirmRect())) return true;
    return false;
}

StartScreenResult StartScreen::GetResult() const
{
    //Invocata al click su Conferma, salva tutti i settings scelti dall'utente
    StartScreenResult r;
    if (!settings.empty()) {
        r.trajectoryMode = settings[0].selectedIndex;
        r.racketAIMode = static_cast<RacketAIMode>(settings[1].selectedIndex);
        r.isLightBallMode = settings[2].selectedIndex != 0;
    }
    return r;
}

void StartScreen::Render(const InputManager& input) const
{
    if (!renderer2D || !textRenderer) return;

    float px = SS_PANEL_LEFT;
    float py = SS_PANEL_TOP;

    //Sfondo nero
    renderer2D->DrawRect(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_GO_BACKGROUND);

    //Pannello (sfondo rosso + interno nero)
    renderer2D->DrawRect(px - 3.0f, py - 3.0f, SS_PANEL_W + 6.0f, SS_PANEL_H + 6.0f, COLOR_GO_PANEL_BACKGROUND);
    renderer2D->DrawRect(px, py, SS_PANEL_W, SS_PANEL_H, COLOR_GO_PANEL_INSIDE);

    //Titolo schermata
    DrawTitle();

    //Linea separatrice
    renderer2D->DrawRect(px + 16.0f, py + SS_OPTIONS_START_Y - 10.0f, SS_PANEL_W - 32.0f, 1.0f, COLOR_SS_SEPARATORLINE);

    //Righe per ogni settings
    for (int i = 0; i < static_cast<int>(settings.size()); ++i) { DrawOptionRow(i, i == selectedSetting); }

    //Frecce per selezionare la modalità di ogni settings
    if (!settings.empty())
    {
        UI::ButtonRect lRect = LeftArrowRect(selectedSetting);
        UI::ButtonRect rRect = RightArrowRect(selectedSetting);

        UI::ButtonStyle lBs = input.IsHover(lRect) ? UI::ArrowButtonHovered() : UI::ArrowButton();
        UI::ButtonStyle rBs = input.IsHover(rRect) ? UI::ArrowButtonHovered() : UI::ArrowButton();

        UI::DrawButton(*renderer2D, *textRenderer, lRect, L"<", lBs);
        UI::DrawButton(*renderer2D, *textRenderer, rRect, L">", rBs);
    }

    UI::DrawButton(*renderer2D, *textRenderer, ConfirmRect(), L"GIOCA  (ENTER)", input.IsHover(ConfirmRect()));
}

void StartScreen::DrawTitle() const
{
    UI::TextStyle TitleTs = { SS_TITLE_X, SS_TITLE_Y, SS_TITLE_W, SS_TITLE_FONT, COLOR_GO_TEXT_TITLE };
    UI::TextStyle SubtitleTs = { SS_SUBTITLE_X, SS_SUBTITLE_Y, SS_SUBTITLE_W, SS_SUBTITLE_FONT, COLOR_GO_TEXT_INFO };

    UI::HDrawCenteredText(*textRenderer, L"ARKANOID", TitleTs);
    UI::HDrawCenteredText(*textRenderer, L"Impostazioni partita", SubtitleTs);
}

void StartScreen::DrawOptionRow(int index, bool selected) const
{
    const Settings& opt = settings[index];

    float rowTopY = SS_ROW_TOP(index);

    //Setting selezionato
    if (selected)
    {
        float x = SS_ROW_SELECTED_X, y = rowTopY, h = SS_ROW_H - 8.0f;

        //Riquadro che evidenzia il setting selezionato
        renderer2D->DrawRect(x, y, SS_PANEL_W - 20.0f, h, COLOR_SS_HIGHLIGHTED_OPTIONROW);

        //Barra sulla sinistra del setting selezionato
        renderer2D->DrawRect(x, y, 3.0f, h, COLOR_SS_HIGHLIGHTED_OPTIONROW_LEFT);
    }

    //Prima parte: titolo setting (da px+18 a px+PANEL_W/2-8)
    UI::TextStyle titleTs = { 
        SS_ROW_TITLE_X,
        SS_ROW_TEXTY(index), 
        SS_ROW_TITLE_W,
        SS_ROW_FONTSIZE,
        selected ? COLOR_GO_TEXT_SCORE : COLOR_GO_TEXT_INFO
    };
    UI::HDrawCenteredText(*textRenderer, opt.title, titleTs);

    //Seconda parte: < opzione > 
    UI::ButtonRect lRect = LeftArrowRect(index);
    UI::ButtonRect rRect = RightArrowRect(index);

    //opzione
    UI::TextStyle optionTs = { 
        lRect.x + SS_BUTTON_ARROW_SIZE + 4.0f,
        SS_ROW_TEXTY(index),
        rRect.x - (lRect.x + SS_BUTTON_ARROW_SIZE + 4.0f) - 4.0f,
        SS_ROW_FONTSIZE,
        selected ? COLOR_GO_TEXT_SCORE : COLOR_GO_TEXT_INFO 
    };
    UI::HDrawCenteredText(*textRenderer, opt.choices[opt.selectedIndex], optionTs);

    //< > (non selezionate, quelle selezionate sono disegnate in ::Render)
    if (!selected)
    {
        UI::TextStyle lArrowTs = { lRect.x, rowTopY + 2.0f, lRect.w, SS_ROW_FONTSIZE, COLOR_SS_ARROW_UNSELECTED };
        UI::HDrawCenteredText(*textRenderer, L"<", lArrowTs);

        UI::TextStyle rArrowTs = { rRect.x, rowTopY + 2.0f, rRect.w, SS_ROW_FONTSIZE, COLOR_SS_ARROW_UNSELECTED };
        UI::HDrawCenteredText(*textRenderer, L">", rArrowTs);
    }
}

