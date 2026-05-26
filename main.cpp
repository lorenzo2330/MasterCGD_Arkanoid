#include "src/game.h"
#include "src/data.h"
#include "src/string.h"
#include <sal.h>
#include <Windows.h>

//Per tenere lo stato del gioco
static Game gameState;

//Intercetta tutti i comandi di windows (click, resize, chiusura, ecc.) [WM_...]
static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    //Input del mouse
    case WM_MOUSEMOVE: gameState.GetInput().OnMouseMove(lParam); return 0;
    case WM_LBUTTONDOWN: gameState.GetInput().OnMouseButtonDown(); return 0;

	//Input da tastiera
    case WM_KEYDOWN: gameState.GetInput().OnKeyDown(wParam); return 0;
    case WM_KEYUP: gameState.GetInput().OnKeyUp(wParam);   return 0;

    //Chiusura della finestra
    case WM_DESTROY: PostQuitMessage(0); return 0;
    case WM_CLOSE: DestroyWindow(hwnd); return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	//Registrazione classe finestra
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;             
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = S_MAIN_CLASSNAME;
    RegisterClassEx(&wc);

    //Aggiusta le dimensioni dello schermo in modo da avere una dimensione W * H
    RECT rc = { 0, 0, static_cast<int>(SCREEN_WIDTH), static_cast<int>(SCREEN_HEIGHT) };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    //Crea la finestra con la classe registrata sopra
    HWND hwnd = CreateWindow(
        S_MAIN_CLASSNAME,
        S_MAIN_WINDOWNAME,
        WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
        CW_USEDEFAULT, 
        CW_USEDEFAULT,
        rc.right - rc.left, 
        rc.bottom - rc.top,
        nullptr, 
        nullptr, 
        hInst, 
        nullptr
    );

    ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	//Inizializzazione del gioco
    if (!gameState.Init(hwnd)) { MessageBox(hwnd, S_ERROR_INIT, S_ERROR, MB_OK | MB_ICONERROR); return -1; }

    //Inizializzazione del timer
    LARGE_INTEGER freq, prev, now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&prev);

	//Game loop, finchè non si chiude la finestra (WM_QUIT)
    MSG msg = {};
    while (msg.message != WM_QUIT && gameState.IsRunning())
    {   
		//Riceve un messaggio, lo traduce (da tasto a comando) e lo invia a WndProc per gestirlo
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
		//Se non ci sono messaggi, avanza con il loop di gioco (quindi Update() e Render())
        else {
            QueryPerformanceCounter(&now);
            float dt = static_cast<float>(now.QuadPart - prev.QuadPart) / static_cast<float>(freq.QuadPart);
            prev = now;
            gameState.Update(dt);
            gameState.Render();
        }
    }

    //Termina il gioco
    gameState.Shutdown();

    return 0;
}