#include "raylib.h"
#include <stdlib.h>

#define FIRST_CHAR 32
#define LAST_CHAR 255
#define MAX_LINES 5

typedef struct
{
  const char *text;
  int nextLine;
} Choice;

typedef enum
{
  SCENE_INTRO,
  SCENE_ROOM,
  SCENE_END
} Scene;

int main(void)
{
  const int screenW = 800;
  const int screenH = 600;
  InitWindow(screenW, screenH, "Visual Novel Demo");

  // Prepara array de glifos de 32 a 255
  int glyphCount = LAST_CHAR - FIRST_CHAR + 1;
  int *glyphs = malloc(glyphCount * sizeof(int));
  for (int i = 0; i < glyphCount; i++)
  {
    glyphs[i] = FIRST_CHAR + i;
  }

  // Carrega uma fonte TTF
  Font font = LoadFontEx("assets/font.ttf", 32, glyphs, glyphCount);
  free(glyphs); // Libera memória alocada para os glifos

  Texture2D bg = LoadTexture("assets/bg0.png");

  const char *dialogue[MAX_LINES] = {
      "Você acorda em uma sala estranha...",
      "Não há portas nem janelas.",
      "Um reflexo na parede chama sua atenção.",
      "É a imagem de um personagem misterioso.",
      "O que você faz?"};

  Choice choices[] = {
      {"Investigar o reflexo", 1},
      {"Tentar abrir a porta", 2},
      {"Gritar por ajuda", 3},
      {"Fugir da sala", 4},
      {"Esperar por alguém", 0}};

  int currentLine = 0;
  int numChoices = 2;
  bool inChoice = false;
  int selected = 0;

  Scene activeScene = SCENE_INTRO;

  SetTargetFPS(60);

  while (!WindowShouldClose())
  {
    if (IsKeyPressed(KEY_SPACE) && currentLine < MAX_LINES - 1)
    {
      currentLine++;
    }

    if (currentLine == 4)
      inChoice = true;

    BeginDrawing();
    ClearBackground(BLACK);

    DrawTexture(bg, 0, 0, WHITE);
    DrawTextEx(font, "Bem-vindo à Visual Novel!", (Vector2){50, 50},
               font.baseSize, 2, WHITE);

    if (inChoice)
    {
      if (IsKeyPressed(KEY_DOWN))
        selected = (selected + 1) % numChoices;
      if (IsKeyPressed(KEY_UP))
        selected = (selected - 1 + numChoices) % numChoices;
      if (IsKeyPressed(KEY_ENTER))
      {
        currentLine = choices[selected].nextLine;
        inChoice = false;
      }

      for (int i = 0; i < numChoices; i++)
      {
        Color col = (i == selected) ? YELLOW : WHITE;
        DrawTextEx(font, choices[i].text,
                   (Vector2){50, 450 + i * 30},
                   font.baseSize, 2, col);
      }
    }
    else
    {
      DrawRectangle(0, 400, screenW, 200, Fade(BLACK, 0.7f)); // caixa de texto
      DrawTextEx(
          font,
          dialogue[currentLine],
          (Vector2){20, 420},
          font.baseSize,
          2,
          WHITE);
    }

    EndDrawing();
  }

  UnloadFont(font);
  UnloadTexture(bg);
  CloseWindow();
  return 0;
}
