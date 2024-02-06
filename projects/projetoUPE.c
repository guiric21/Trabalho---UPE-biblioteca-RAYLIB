#include <raylib.h>

#define LARGURA_TELA 800
#define ALTURA_TELA 600
#define GRAVIDADE 0.25
#define FORCA_DE_FLAP -7
#define LARGURA_CANO 80
#define ESPACO_ENTRE_CANOS 200
#define VELOCIDADE_SCROLL_CANOS 2

typedef struct Passaro {
    Vector2 posicao;
    Vector2 velocidade;
    Vector2 pontos[3]; // Triângulo
} Passaro;

typedef struct Cano {
    Rectangle topo;
    Rectangle base;
    bool ativo;
} Cano;

void CarregarCanos(Cano canos[]) {
    for (int i = 0; i < 3; i++) {
        canos[i].topo = (Rectangle){LARGURA_TELA + i * 300, 0, LARGURA_CANO, GetRandomValue(200, 400)};
        canos[i].base = (Rectangle){LARGURA_TELA + i * 300, canos[i].topo.height + ESPACO_ENTRE_CANOS, LARGURA_CANO, ALTURA_TELA - canos[i].topo.height - ESPACO_ENTRE_CANOS};
        canos[i].ativo = true;
    }
}

void AtualizarPassaro(Passaro *passaro) {
    passaro->velocidade.y += GRAVIDADE;
    passaro->posicao.y += passaro->velocidade.y;
    
    // Atualiza a posição dos pontos do triângulo
    passaro->pontos[0] = (Vector2){passaro->posicao.x + 10, passaro->posicao.y}; // Direita
    passaro->pontos[1] = (Vector2){passaro->posicao.x - 10, passaro->posicao.y - 15}; // Superior
    passaro->pontos[2] = (Vector2){passaro->posicao.x - 10, passaro->posicao.y + 15}; // Inferior

    if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        passaro->velocidade.y = FORCA_DE_FLAP;
        PlaySound(LoadSound("jump3.wav"));
    }
}

void AtualizarCanos(Cano canos[]) {
    for (int i = 0; i < 3; i++) {
        canos[i].topo.x -= VELOCIDADE_SCROLL_CANOS;
        canos[i].base.x -= VELOCIDADE_SCROLL_CANOS;

        if (canos[i].topo.x + LARGURA_CANO < 0) {
            canos[i].topo.x = canos[(i + 2) % 3].topo.x + 300;
            canos[i].base.x = canos[(i + 2) % 3].base.x + 300;
            canos[i].topo.height = GetRandomValue(200, 400);
            canos[i].base.y = canos[i].topo.height + ESPACO_ENTRE_CANOS;
            canos[i].base.height = ALTURA_TELA - canos[i].topo.height - ESPACO_ENTRE_CANOS;
            canos[i].ativo = true; // Ativar cano para reaparecer
        }
    }
}

bool ChecarColisao(Passaro passaro, Cano canos[], bool isGameOver) {
    for (int i = 0; i < 3; i++) {
        if (canos[i].ativo && !isGameOver) {
            if ((CheckCollisionPointRec(passaro.pontos[0], canos[i].topo) || // Colisão com o ponto da direita do triângulo
                 CheckCollisionPointRec(passaro.pontos[1], canos[i].topo) || // Colisão com o ponto superior do triângulo
                 CheckCollisionPointRec(passaro.pontos[2], canos[i].topo) || // Colisão com o ponto inferior do triângulo
                 CheckCollisionPointRec(passaro.pontos[0], canos[i].base) || // Colisão com o ponto da direita do triângulo
                 CheckCollisionPointRec(passaro.pontos[1], canos[i].base) || // Colisão com o ponto superior do triângulo
                 CheckCollisionPointRec(passaro.pontos[2], canos[i].base)) && // Colisão com o ponto inferior do triângulo
                passaro.posicao.x < canos[i].topo.x + LARGURA_CANO) {
                return true;
            }
        }
    }
    return false;
}

void DesenharPassaro(Passaro passaro) {
    DrawTriangle(passaro.pontos[0], passaro.pontos[1], passaro.pontos[2], YELLOW);
}

void DesenharCanos(Cano canos[]) {
    for (int i = 0; i < 3; i++) {
        if (canos[i].ativo) {
            DrawRectangleRec(canos[i].topo, GREEN);
            DrawRectangleRec(canos[i].base, GREEN);
        }
    }
}

int main() {
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Passarinho Pulante");
    SetTargetFPS(60);

    Passaro passaro;
    passaro.posicao = (Vector2){LARGURA_TELA / 4, ALTURA_TELA / 2};
    passaro.velocidade = (Vector2){0, 0};
    passaro.pontos[0] = (Vector2){passaro.posicao.x + 10, passaro.posicao.y}; // Direita
    passaro.pontos[1] = (Vector2){passaro.posicao.x - 10, passaro.posicao.y - 15}; // Superior
    passaro.pontos[2] = (Vector2){passaro.posicao.x - 10, passaro.posicao.y + 15}; // Inferior

    Cano canos[3];
    CarregarCanos(canos);

    int pontuacao = 0;
    bool isGameOver = false;

    while (!WindowShouldClose()) {
        if (!isGameOver) {
            AtualizarPassaro(&passaro);
            AtualizarCanos(canos);

            if (ChecarColisao(passaro, canos, isGameOver)) {
                isGameOver = true;
            }

            for (int i = 0; i < 3; i++) {
                if (canos[i].ativo && passaro.posicao.x > canos[i].topo.x + LARGURA_CANO) {
                    pontuacao++;
                    canos[i].ativo = false;
                }
            }
        } else {
            // Lógica de fim de jogo
            if (IsKeyPressed(KEY_R)) {
                // Reiniciar jogo
                passaro.posicao = (Vector2){LARGURA_TELA / 4, ALTURA_TELA / 2};
                passaro.velocidade = (Vector2){0, 0};
                passaro.pontos[0] = (Vector2){passaro.posicao.x + 10, passaro.posicao.y}; // Direita
                passaro.pontos[1] = (Vector2){passaro.posicao.x - 10, passaro.posicao.y - 15}; // Superior
                passaro.pontos[2] = (Vector2){passaro.posicao.x - 10, passaro.posicao.y + 15}; // Inferior
                CarregarCanos(canos);
                pontuacao = 0;
                isGameOver = false;
            }
        }

        BeginDrawing();
        ClearBackground(SKYBLUE);

        DesenharCanos(canos);
        DesenharPassaro(passaro);

        // Desenhar pontuação
        DrawText(TextFormat("Pontuação: %d", pontuacao), 10, 10, 20, BLACK);

        if (isGameOver) {
            DrawText("Fim de jogo! Pressione 'R' para reiniciar.", LARGURA_TELA / 2 - MeasureText("Fim de jogo! Pressione 'R' para reiniciar.", 20) / 2, ALTURA_TELA / 2 - 10, 20, RED);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
