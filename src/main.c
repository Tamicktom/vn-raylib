#include "raylib.h"

int main(void)
{
  const int screenW = 800;
  const int screenH = 600;
  InitWindow(screenW, screenH, "Visual Novel Demo");

  // Carrega uma fonte TTF
  Font font = LoadFont("assets/font.ttf");
  Texture2D bg = LoadTexture("assets/bg0.png");

  SetTargetFPS(60);

  while (!WindowShouldClose())
  {
    BeginDrawing();
    ClearBackground(BLACK);

    DrawTexture(bg, 0, 0, WHITE);
    DrawTextEx(font, "Bem-vindo à Visual Novel!", (Vector2){50, 50},
               font.baseSize, 2, WHITE);

    EndDrawing();
  }

  UnloadFont(font);
  UnloadTexture(bg);
  CloseWindow();
  return 0;
}
