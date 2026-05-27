#include "../colors.h"
#include "../data.h"
#include "gameOverScreen.h"
#include "inputManager.h"
#include "uiHelper.h"
#include "../render/renderer2d.h"
#include "../render/textRenderer.h"
#include <windows.h>
#include "../string.h"

void GameOverScreen::Init(TextRenderer* tr, Renderer2D* r2d)
{
    textRenderer = tr;
    renderer2D = r2d;

    //Dimensione e posizione del pannello centrale
    panelW = SCREEN_WIDTH * 0.5f;
    panelH = SCREEN_HEIGHT * 0.7f;
    panelX = (SCREEN_WIDTH - panelW) * 0.5f;
    panelY = (SCREEN_HEIGHT - panelH) * 0.5f;

    //Posizione y dei pulsanti
    float btnX = panelX + (panelW - GO_BUTTON_WIDTH) * 0.5f;
    float restartY = panelY + GO_BUTTON_START_Y;
    float quickRestartY = restartY + GO_BUTTON_HEIGHT + GO_BUTTON_GAP;
    float quitY = quickRestartY + GO_BUTTON_HEIGHT + GO_BUTTON_GAP;

	//Definizione dei pulsanti
    buttonRestart = { btnX, restartY, GO_BUTTON_WIDTH, GO_BUTTON_HEIGHT };
    buttonQuickRestart = { btnX, quickRestartY, GO_BUTTON_WIDTH, GO_BUTTON_HEIGHT };
    buttonQuit = { btnX, quitY, GO_BUTTON_WIDTH, GO_BUTTON_HEIGHT };
}

InputManager::Action GameOverScreen::HandleInput(const InputManager& input) const
{
    if (input.IsKeyDown('R')) return InputManager::Action::Restart;
    if (input.IsKeyDown('Q')) return InputManager::Action::QuickRestart;
    if (input.IsKeyDown(VK_ESCAPE)) return InputManager::Action::Quit;

    if (input.IsClicked(buttonRestart)) return InputManager::Action::Restart;
    if (input.IsClicked(buttonQuickRestart)) return InputManager::Action::QuickRestart;
    if (input.IsClicked(buttonQuit)) return InputManager::Action::Quit;

    return InputManager::Action::None;
}

void GameOverScreen::Render(const InputManager& input) const
{
    if (!renderer2D || !textRenderer) return;

    //Render sfondo
    renderer2D->DrawRect(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_GO_BACKGROUND);

    //Render pannello (sfondo rosso)
    renderer2D->DrawRect(panelX - 3.0f, panelY - 3.0f, panelW + 6.0f, panelH + 6.0f, COLOR_GO_PANEL_BACKGROUND);
    
	//Render pannello (interno nero)
    renderer2D->DrawRect(panelX, panelY, panelW, panelH, COLOR_GO_PANEL_INSIDE);
    
    //Inizio render testo
    textRenderer->BeginDraw();
    
    //Titolo
    textRenderer->DrawCenteredText(S_GAMEOVER, panelX, panelY + 22.0f, panelW, GO_FONTSIZE_TITLE, COLOR_GO_TEXT_TITLE);

    //Info partita
    textRenderer->DrawCenteredText(S_SCORE(finalScore), panelX, panelY + 90.0f, panelW, GO_FONTSIZE_SCORE, COLOR_GO_TEXT_SCORE);

    //Riassunto tasti rapidi
    textRenderer->DrawCenteredText(S_GAMEOVER_RAPIDCOMMAND, panelX, panelY + 128.0f, panelW, GO_FONTSIZE_INFO, COLOR_GO_TEXT_INFO);

    //Fine render testo
    textRenderer->EndDraw();

	//Render pulsanti
    UI::DrawButton(*renderer2D, *textRenderer, buttonRestart, S_RESTART, input.IsHover(buttonRestart));
    UI::DrawButton(*renderer2D, *textRenderer, buttonQuickRestart, S_QUICKRESTART, input.IsHover(buttonQuickRestart));
    UI::DrawButton(*renderer2D, *textRenderer, buttonQuit, S_QUIT, input.IsHover(buttonQuit));
}