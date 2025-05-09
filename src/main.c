#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cjson/cJSON.h>

typedef struct
{
  char *text;
  char *nextId; // guarda temporariamente o id do next
  int nextNode; // índice real, só preenchido na fase 2
} Option;

typedef struct
{
  char *id;
  char *text;
  Option *options;
  int optionCount;
} Node;

typedef struct
{
  Node *nodes;
  int nodeCount;
  int currentIndex;
} Dialogue;

// encontra o índice de um node dado seu id (ou -1 se não achar)
static int findNodeIndex(const Dialogue *dlg, const char *id)
{
  for (int i = 0; i < dlg->nodeCount; i++)
    if (strcmp(dlg->nodes[i].id, id) == 0)
      return i;
  return -1;
}

// inicializa Dialogue a partir de um JSON no caminho filePath
static bool initializeDialogue(const char *filePath, Dialogue *dlg)
{
  // --- carrega arquivo como antes ---
  FILE *f = fopen(filePath, "rb");
  if (!f)
    return false;
  fseek(f, 0, SEEK_END);
  long len = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *data = malloc(len + 1);
  fread(data, 1, len, f);
  data[len] = '\0';
  fclose(f);

  cJSON *root = cJSON_Parse(data);
  free(data);
  if (!root)
    return false;

  // lê start_node
  cJSON *start = cJSON_GetObjectItem(root, "start_node");
  if (!cJSON_IsString(start))
  {
    cJSON_Delete(root);
    return false;
  }

  // pega array de nodes
  cJSON *nodesArr = cJSON_GetObjectItem(root, "nodes");
  int n = cJSON_GetArraySize(nodesArr);
  dlg->nodes = calloc(n, sizeof(Node));
  dlg->nodeCount = n;

  // --- FASE 1: lê id, text e options->text/nextId ---
  for (int i = 0; i < n; i++)
  {
    cJSON *obj = cJSON_GetArrayItem(nodesArr, i);
    cJSON *jid = cJSON_GetObjectItem(obj, "id");
    cJSON *jtx = cJSON_GetObjectItem(obj, "text");
    cJSON *jop = cJSON_GetObjectItem(obj, "options");

    dlg->nodes[i].id = strdup(jid->valuestring);
    dlg->nodes[i].text = strdup(jtx->valuestring);
    dlg->nodes[i].optionCount = cJSON_IsArray(jop) ? cJSON_GetArraySize(jop) : 0;
    dlg->nodes[i].options = dlg->nodes[i].optionCount > 0
                                ? calloc(dlg->nodes[i].optionCount, sizeof(Option))
                                : NULL;

    for (int j = 0; j < dlg->nodes[i].optionCount; j++)
    {
      cJSON *optObj = cJSON_GetArrayItem(jop, j);
      dlg->nodes[i].options[j].text = strdup(
          cJSON_GetObjectItem(optObj, "text")->valuestring);
      dlg->nodes[i].options[j].nextId = strdup(
          cJSON_GetObjectItem(optObj, "next")->valuestring);
      dlg->nodes[i].options[j].nextNode = -1; // ainda não resolvido
    }
  }

  // --- FASE 2: resolve todos os nextId para nextNode ---
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < dlg->nodes[i].optionCount; j++)
    {
      char *nid = dlg->nodes[i].options[j].nextId;
      int idx = findNodeIndex(dlg, nid);
      dlg->nodes[i].options[j].nextNode = idx;
      free(nid);
    }
  }

  // posiciona currentIndex no start_node
  int startIdx = findNodeIndex(dlg, start->valuestring);
  dlg->currentIndex = startIdx >= 0 ? startIdx : 0;

  cJSON_Delete(root);
  return true;
}

int main(void)
{
  const int screenW = 800;
  const int screenH = 600;
  InitWindow(screenW, screenH, "Visual Novel Demo");

// Carrega fonte
#define FIRST_CHAR 32
#define LAST_CHAR 255
  int glyphCount = LAST_CHAR - FIRST_CHAR + 1;
  int *glyphs = malloc(glyphCount * sizeof(int));
  for (int i = 0; i < glyphCount; i++)
    glyphs[i] = FIRST_CHAR + i;
  Font font = LoadFontEx("assets/font.ttf", 32, glyphs, glyphCount);
  free(glyphs);

  Texture2D bg = LoadTexture("assets/bg0.png");

  // Inicializa dialogue
  Dialogue dlg;
  if (!initializeDialogue("assets/dialogue.json", &dlg))
  {
    fprintf(stderr, "Erro ao carregar dialogue.json\n");
    return 1;
  }

  bool inChoice = false;
  int selected = 0;

  SetTargetFPS(60);
  while (!WindowShouldClose())
  {
    Node *cur = &dlg.nodes[dlg.currentIndex];

    // se não temos opções, avançar com SPACE
    if (!inChoice && cur->optionCount == 0 && IsKeyPressed(KEY_SPACE))
    {
      // assume próximo nó como index+1 (pode ajustar conforme necessidade)
      dlg.currentIndex = (dlg.currentIndex + 1) % dlg.nodeCount;
    }

    // se chegamos num nó com opções, entramos em modo escolha
    if (cur->optionCount > 0)
      inChoice = true;

    BeginDrawing();
    ClearBackground(BLACK);

    // draw background
    DrawTexture(bg, 0, 0, WHITE);

    // draw dialogue box
    DrawRectangle(0, 400, screenW, screenH - 400, (Color){0, 0, 0, 200});
    DrawRectangleLines(0, 400, screenW, screenH - 400, WHITE);

    // draw text
    DrawTextEx(font, cur->text, (Vector2){20, 420},
               font.baseSize, 2, WHITE);

    if (inChoice && cur->optionCount > 0)
    {
      if (IsKeyPressed(KEY_DOWN))
        selected = (selected + 1) % cur->optionCount;
      if (IsKeyPressed(KEY_UP))
        selected = (selected - 1 + cur->optionCount) % cur->optionCount;
      if (IsKeyPressed(KEY_ENTER))
      {
        // pula para o node escolhido
        dlg.currentIndex = cur->options[selected].nextNode;
        inChoice = false;
        selected = 0;
      }
      // desenha opções
      for (int i = 0; i < cur->optionCount; i++)
      {
        Color col = (i == selected) ? YELLOW : WHITE;
        DrawTextEx(font, cur->options[i].text,
                   (Vector2){50, 450 + i * 30},
                   font.baseSize, 2, col);
      }
    }

    EndDrawing();
  }

  // cleanup
  for (int i = 0; i < dlg.nodeCount; i++)
  {
    free(dlg.nodes[i].id);
    free(dlg.nodes[i].text);
    for (int j = 0; j < dlg.nodes[i].optionCount; j++)
      free(dlg.nodes[i].options[j].text);
    free(dlg.nodes[i].options);
  }
  free(dlg.nodes);

  UnloadFont(font);
  UnloadTexture(bg);
  CloseWindow();
  return 0;
}
