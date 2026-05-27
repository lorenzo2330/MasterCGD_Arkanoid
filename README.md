# Arkanoid Project

Project developed for the class **Graphics Programming** of the *Master in Computer Game Development* of the *University of Verona (Italy)*

## Main functionalities

### Ball trajectory (RayCasting Predictor)
Display the ball's trajectory
- **Off:** no mode
- **Most dangerous:** display only the trajectory of the most dangerous ball (with highest y-value)
- **All:** shows all ball trajectories

### AI racket
Allows the paddle to move autonomously, thanks to (basic) AI
- **Off:** no mode
- **Only balls:** the AI focuses only about don't missing the ball
- **Adaptive:** the AI has priorities (ball > bonus > brick)

### Light ball mode
The whole screen becomes dark and only balls, paddle and bonuses glow
- **Off:** no mode
- **On:** mode active

---

## Technical Stack
- **Language:** C++
- **Graphics API:** DirectX 11 (with Direct2D / DirectWrite interop for text rendering)
- **Audio:** Custom WAV parser and player

---

## Structure of the projects (ita)

- src
    - main.cpp
    - data.h: Contiene tutti i parametri del gioco (dimensione schermo, velocita' palline, ...)
    - colors.h: Contiene tutti i colori degli oggetti di gioco
    - game.h/cpp: Coordina il game, possiede tutti gli oggetti (renderer, racchetta, palline, livello, bonus, hud, ...)
        - Update(): Invoca i vari singoli update
        - UpdateCollisions(): gestisce le collisioni tra mattoncini
        - Render(): invoca i vari renderer
    - ai
        - ballPredictor.h/cpp: Mostra una linea che predice dove andra' la palla (tramite RayCasting)
            - PickDangerousBall(): trova la palla piu' vicina al bordo e mostra la sua traiettoria
        - racketAI.h/cpp: Implementa il ragionamento e stabilisce dove si deve muovere la racchetta
            - ComputeTarget(): restituisce, in base alla modalita' di gioco, la posizione x da raggiungere secondo l'IA
            - AdaptiveTarget(): modalita' adaptive, stabilisce la x dando priorita' a pallina > bonus > posizione strategica 
    - game
        - ball.h/cpp: Definisce Ball
            - Update(): muove la pallina
        - bonus.h/cpp: Definisce BonusItem (il bonus che cade dopo aver rotto un mattoncino)
            - Update(): fa cadere il bonus e lo disattiva se supera la racchetta
        - brick.h/cpp: Definsice BrickType, BonusType e la struct Brick
            - Hit(): gestisce la collisione con un mattoncino
        - level.h/cpp: Gestisce la griglia di mattoncini
        - racket.h/cpp: Definisce Racket
            - Update(): Legge gli input (freccette / AD) e muove la racchetta di conseguenza
        - utils.h: Contiene CheckAABB, template per la gestione delle collisioni tra rettangoli e altre funzioni utili
    - render
        - lightBallRender.h/cpp: Sfrutta un "backbuffer intermedio" per mascherare la scena e illuminare alcuni punti
            - BeginFrame(): Anziche' direttamente sul backbuffer, reindirizza il rendering su un backbuffer intermedio
            - EndFrame(): Aggiorna il constant buffer (verra' poi letto dal ps, che calcolera' i colori dei vari pixel)
        - render.h/cpp: Crea e gestisce DX11 (Device,  Swap chain, rtw, viewport, ...)
            - BeginFrame(): Pulisce lo schermo
            - EndFrame(): Chiama Present() (quando il frame e' pronto per essere mostrato)
        - render2d.h/cpp: Si appoggia su render, aggiunge la capacita' di disegnare i rettangoli
            - DrawRect(x, y, w, h, r, g, b, a): Disegna un rettangolo
        - shader.h/cpp: Compila i .hlsl, crea vertex e pixel shader
            - Bind(): Attiva gli shader
            - GetVSBlob(): costruisce l'input layout
        - textRender.h/cpp: Aggiunge il testo sopra DX11 tramite interop D2D1 + DirectWrite
            - DrawText(): Prepara il testo e la posizione, per poi farlo disegnare tramite d2dContext->DrawText()
    - sound
        - soundBank.h: Contiene l'elenco dei path ai file musicali
        - soundClip.h/cpp: Rappresenta un singolo file audio caricato in memoria
            - Load(): Apre il file .wav e lo parsa
        - soundManager.h/cpp: Espone il metodo per riprodurre i suoni
            - Play(): Riproduce il suono relativo all'id ricevuto
    - ui
        - gameOverScreen.h/cpp: Mostra schermata finale con pulsanti restart / quit
        - hud.h: Disegna punteggio e livello in overlay durante la partita
        - inputManager.h: Tiene traccia dello stato del mouse (posizione, click, ...)
        - pauseScreen.h/cpp: Definisce la schermata di pausa, con i vari pulsanti per ricominciare, continuare e terminare la partita
        - startScreen.h/cpp: Definisce la schermata iniziale, con le varie impostazioni customizzabili
        - uiHepler.h: Contiene utils per disegnare testo o pulsanti
- shaders
    - vs.hlsl: Vertex shader generico usato durante tutto il gameplay
    - vs_fullscreen.hlsl: Vertex shader per coprire l'intero schermo e scorrerlo con ps_lightball
    - ps.hlsl: Pixel shader per i rettangoli
    - ps_circle.hlsl: Pixel shader per i cerchi
    - ps_lightball.hlsl: Pixel shader per calcolare la luminosita' di un pixel in modalita' lightball

---

## Credits & Attribuzioni

Tutti gli asset audio utilizzati in questo progetto sono stati distribuiti gratuitamente sulla piattaforma [Freesound.org](https://freesound.org/) e sono rilasciati sotto licenza **Creative Commons (CC)**.