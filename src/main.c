#include "raylib.h"
#define MAX_LINES 5

int main(void)
{
  const int screenW = 800;
  const int screenH = 600;
  InitWindow(screenW, screenH, "Visual Novel Demo");

  // Carrega uma fonte TTF
  Font font = LoadFont("assets/font.ttf");
  Texture2D bg = LoadTexture("assets/bg0.png");

  const char *dialogue[MAX_LINES] = {
      "Você acorda em uma sala estranha...",
      "Não há portas nem janelas.",
      "Um reflexo na parede chama sua atenção.",
      "É a imagem de um personagem misterioso.",
      "O que você faz?"};
  int currentLine = 0;

  SetTargetFPS(60);

  while (!WindowShouldClose())
  {
    if (IsKeyPressed(KEY_SPACE) && currentLine < MAX_LINES - 1)
    {
      currentLine++;
    }

    BeginDrawing();
    ClearBackground(BLACK);

    DrawTexture(bg, 0, 0, WHITE);
    DrawTextEx(font, "Bem-vindo à Visual Novel!", (Vector2){50, 50},
               font.baseSize, 2, WHITE);

    DrawRectangle(0, 400, screenW, 200, Fade(BLACK, 0.7f)); // caixa de texto
    DrawTextEx(font, dialogue[currentLine], (Vector2){20, 420},
               font.baseSize, 2, WHITE);

    EndDrawing();
  }

  UnloadFont(font);
  UnloadTexture(bg);
  CloseWindow();
  return 0;
}
