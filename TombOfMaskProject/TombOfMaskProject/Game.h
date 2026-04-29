#pragma once
#include "raylib.h"
#include "Scene.h"
#include "Level.h"

#define FONT_SIZE       32
#define TEXT_SPACING    2.0f

enum GameState {
    INITIAL,
    TITLE,
    GAMEPLAY,
    GAMEOVER,
    WIN
};

class Game
{
public:
    void Init();            //Open window, load assets, and set initial game state
    void Update();          //Dispatch update to the current state each frame
    void Draw();            //Draw world first for gameplay states, then overlay the state-specific UI
    void DeInit();          //Unload all assets and close the window

    bool ShouldClose() const;   //Return true when the window close button is pressed

private:
    void UpdateInitial();           //Wait for ENTER to advance to the title screen
    void UpdateTitle();             //Wait for ENTER to start gameplay
    void UpdateGameplay(float dt);  //Check win/death transitions; number keys 1-5 load levels directly
    void UpdateGameOver();          //Wait for ENTER to return to the title screen
    void UpdateWin();               //Wait for ENTER to return to the title screen

    void DrawInitial();     //Draw the credits and project info screen
    void DrawTitle();       //Draw the main menu with background and title text
    void DrawGameplay();    //Draw HUD: star count and control hints
    void DrawGameOver();    //Draw red overlay with GAME OVER text
    void DrawWin();         //Draw green overlay with YOU WIN text and star results

    GameState currentState;

    int screenWidth;
    int screenHeight;

    Font font;
    Scene scene;
    Texture2D menu;
};
