#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>

#define WIDTH 800
#define HEIGHT 600
#define MAX_OBJETOS 5

int objetoSelecionado = -1;

typedef struct {
    float **pontosOriginais;
    float **pontosTransformados;
    int numVertices;

    int **arestas;
    int numArestas;

    float escalax, escalay;
    float deslx, desly;
    float angulo;

    int arrastando;
    int mouseAnteriorX, mouseAnteriorY;
} Objeto;

void desenhaAresta(SDL_Renderer *renderer, float x1, float y1, float x2, float y2);

void desenhaObjeto(SDL_Renderer *renderer, int numArestas, float escalax, float escalay, float deslx, float desly,
                   float **pontos, int **arestas);

int carregarObjeto(const char *filename, float ***pontos, int *numVertices, int ***arestas, int *numArestas);

void aplicarRotacaoComCentro(float **pontosOriginais, float **pontosTransformados, int numVertices, float angulo);

int pontoDentroDoObjeto(float **pontos, int numVertices, float escalax, float escalay, float deslx, float desly,
                        int mouseX, int mouseY);

int pontoProximoDaAresta(float **pontos, int **arestas, int numArestas, float escalax, float escalay, float deslx,
                         float desly, int mouseX, int mouseY);

void liberaObjetos(objetos, numObjetos);


int main(int argc, char *argv[]) {
    Objeto objetos[MAX_OBJETOS];
    int numObjetos = 2;
    const char *arquivos[] = {"cubo.txt", "casa.txt"};

    for (int i = 0; i < numObjetos; i++) {
        Objeto *obj = &objetos[i];

        if (!carregarObjeto(arquivos[i], &obj->pontosOriginais, &obj->numVertices,
                            &obj->arestas, &obj->numArestas)) {
            return EXIT_FAILURE;
        }

        obj->pontosTransformados = malloc(obj->numVertices * sizeof(float *));
        for (int j = 0; j < obj->numVertices; j++) {
            obj->pontosTransformados[j] = malloc(2 * sizeof(float));
        }

        obj->escalax = 0.1f;
        obj->escalay = 0.1f;
        obj->deslx = -0.9f + i * 0.5f;
        obj->desly = -0.4f;
        obj->angulo = 0.0f;
        obj->arrastando = 0;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("Deu erro!!! SDL error %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window *window = SDL_CreateWindow("SDL Hello World!!!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH,
                                          HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
    if (window == NULL) {
        printf("Deu erro na janela!!! SDL error %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Event windowEvent;
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    printf("Menu\n");
    printf("Utilize wasd para mover o objeto selecionado\n");
    printf("Utilize qe para rotacionar o objeto selecionado\n");

    while (1) {
        for (int i = 0; i < numObjetos; i++) {
            aplicarRotacaoComCentro(objetos[i].pontosOriginais, objetos[i].pontosTransformados,
                                    objetos[i].numVertices, objetos[i].angulo);
        }


        if (SDL_PollEvent(&windowEvent)) {
            if (windowEvent.type == SDL_QUIT) {
                break;
            }
            if (windowEvent.type == SDL_KEYDOWN) {
                // https://youtu.be/YfGYU7wWLo8?si=ULTr7xdT59_WyaQC
                if (objetoSelecionado != -1) {
                    if (windowEvent.key.keysym.sym == SDLK_a || windowEvent.key.keysym.sym == SDLK_LEFT) {
                        objetos[objetoSelecionado].deslx -= 0.01;
                    }
                    if (windowEvent.key.keysym.sym == SDLK_d || windowEvent.key.keysym.sym == SDLK_RIGHT) {
                        objetos[objetoSelecionado].deslx += 0.01;
                    }
                    if (windowEvent.key.keysym.sym == SDLK_w || windowEvent.key.keysym.sym == SDLK_UP) {
                        objetos[objetoSelecionado].desly += 0.01;
                    }
                    if (windowEvent.key.keysym.sym == SDLK_s || windowEvent.key.keysym.sym == SDLK_DOWN) {
                        objetos[objetoSelecionado].desly -= 0.01;
                    }
                    if (windowEvent.key.keysym.sym == SDLK_q) {
                        objetos[objetoSelecionado].angulo -= 0.1;
                    }
                    if (windowEvent.key.keysym.sym == SDLK_e) {
                        objetos[objetoSelecionado].angulo += 0.1;
                    }
                    if (windowEvent.key.keysym.sym == SDLK_EQUALS || windowEvent.key.keysym.sym == SDLK_KP_PLUS) {
                        objetos[objetoSelecionado].escalax += 0.01;
                        objetos[objetoSelecionado].escalay += 0.01;
                    }
                    if (windowEvent.key.keysym.sym == SDLK_MINUS || windowEvent.key.keysym.sym == SDLK_KP_MINUS) {
                        objetos[objetoSelecionado].escalax -= 0.01;
                        objetos[objetoSelecionado].escalay -= 0.01;
                    }
                }
            }

            if (windowEvent.type == SDL_MOUSEBUTTONDOWN && windowEvent.button.button == SDL_BUTTON_LEFT) {
                int mx = windowEvent.button.x;
                int my = windowEvent.button.y;
                objetoSelecionado = -1;

                for (int i = 0; i < numObjetos; i++) {
                    if (pontoDentroDoObjeto(objetos[i].pontosTransformados, objetos[i].numVertices,
                                            objetos[i].escalax, objetos[i].escalay,
                                            objetos[i].deslx, objetos[i].desly, mx, my)) {
                        objetoSelecionado = i;
                        printf("Objeto Selecionado: %d\n", i+1);
                        objetos[i].arrastando = 1;
                        objetos[i].mouseAnteriorX = mx;
                        objetos[i].mouseAnteriorY = my;
                        break;
                    }
                }
            }

            if (windowEvent.type == SDL_MOUSEBUTTONUP && windowEvent.button.button == SDL_BUTTON_LEFT) {
                if (objetoSelecionado != -1) {
                    objetos[objetoSelecionado].arrastando = 0;
                }
            }


            if (objetoSelecionado != -1 && windowEvent.type == SDL_MOUSEWHEEL) {
                if (windowEvent.wheel.y > 0) {
                    objetos[objetoSelecionado].escalax += 0.01;
                    objetos[objetoSelecionado].escalay += 0.01;
                }
                if (windowEvent.wheel.y < 0) {
                    objetos[objetoSelecionado].escalax -= 0.01;
                    objetos[objetoSelecionado].escalay -= 0.01;
                }
            }

            if (windowEvent.type == SDL_MOUSEMOTION) {
                int xmouse, ymouse;
                SDL_GetMouseState(&xmouse, &ymouse);
                //printf("Mouse :: %3d %3d\n", xmouse, ymouse);
            }

            if (windowEvent.type == SDL_MOUSEMOTION && objetoSelecionado != -1 && objetos[objetoSelecionado].
                    arrastando) {
                int mx = windowEvent.motion.x;
                int my = windowEvent.motion.y;

                float dx = (float) (mx - objetos[objetoSelecionado].mouseAnteriorX) / (WIDTH / 2.0f);
                float dy = -(float) (my - objetos[objetoSelecionado].mouseAnteriorY) / (HEIGHT / 2.0f);

                objetos[objetoSelecionado].deslx += dx;
                objetos[objetoSelecionado].desly += dy;

                objetos[objetoSelecionado].mouseAnteriorX = mx;
                objetos[objetoSelecionado].mouseAnteriorY = my;
            }
        }

        SDL_SetRenderDrawColor(renderer, 242, 242, 242, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        for (int i = 0; i < numObjetos; i++) {
            desenhaObjeto(renderer, objetos[i].numArestas, objetos[i].escalax, objetos[i].escalay,
                          objetos[i].deslx, objetos[i].desly,
                          objetos[i].pontosTransformados, objetos[i].arestas);
        }


        SDL_RenderPresent(renderer);
    }


    liberaObjetos(objetos, numObjetos);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

void desenhaAresta(SDL_Renderer *renderer, float x1, float y1, float x2, float y2) {
    int xa, xb, ya, yb;
    //printf("Valores unificado: x1: %f, y1: %f, x2: %f, y2: %f\n", x1, y1, x2, y2);
    xa = WIDTH * ((x1 + 1) / 2);
    xb = WIDTH * ((x2 + 1) / 2);
    ya = HEIGHT * ((-y1 + 1) / 2);
    yb = HEIGHT * ((-y2 + 1) / 2);
    //printf("Valores dispositivo: xa: %d, ya: %d, xb: %d, yb: %d\n", xa, ya, xb, yb);
    SDL_RenderDrawLine(renderer, xa, ya, xb, yb);
}

void desenhaObjeto(SDL_Renderer *renderer, int numArestas, float escalax, float escalay, float deslx, float desly,
                   float **pontos, int **arestas) {
    for (int i = 0; i < numArestas; i++) {
        int idx1 = arestas[i][0];
        int idx2 = arestas[i][1];
        float x1 = deslx + (pontos[idx1][0] * escalax);
        float y1 = desly + (pontos[idx1][1] * escalay);
        float x2 = deslx + (pontos[idx2][0] * escalax);
        float y2 = desly + (pontos[idx2][1] * escalay);
        desenhaAresta(renderer, x1, y1, x2, y2);
    }
}

int carregarObjeto(const char *filename, float ***pontos, int *numVertices, int ***arestas, int *numArestas) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        return 0;
    }

    fscanf(file, "%d", numVertices);
    *pontos = (float **) malloc(*numVertices * sizeof(float *));
    for (int i = 0; i < *numVertices; i++) {
        (*pontos)[i] = (float *) malloc(2 * sizeof(float));
        fscanf(file, "%f %f", &(*pontos)[i][0], &(*pontos)[i][1]);
    }

    fscanf(file, "%d", numArestas);
    *arestas = (int **) malloc(*numArestas * sizeof(int *));
    for (int i = 0; i < *numArestas; i++) {
        (*arestas)[i] = (int *) malloc(2 * sizeof(int));
        fscanf(file, "%d %d", &(*arestas)[i][0], &(*arestas)[i][1]);
    }

    fclose(file);
    return 1;
}

void aplicarRotacaoComCentro(float **pontosOriginais, float **pontosTransformados, int numVertices, float angulo) {
    // 1. Calcula o centroide (média dos pontos)
    float cx = 0.0, cy = 0.0;
    for (int i = 0; i < numVertices; i++) {
        cx += pontosOriginais[i][0];
        cy += pontosOriginais[i][1];
    }
    cx /= numVertices;
    cy /= numVertices;

    // 2. Aplica rotação em torno do centro
    for (int i = 0; i < numVertices; i++) {
        float x = pontosOriginais[i][0] - cx;
        float y = pontosOriginais[i][1] - cy;

        pontosTransformados[i][0] = cx + (x * cos(angulo) - y * sin(angulo));
        pontosTransformados[i][1] = cy + (x * sin(angulo) + y * cos(angulo));
    }
}

int pontoDentroDoObjeto(float **pontos, int numVertices, float escalax, float escalay, float deslx, float desly,
                        int mouseX, int mouseY) {
    float x = ((float) mouseX / WIDTH) * 2 - 1;
    float y = -(((float) mouseY / HEIGHT) * 2 - 1);

    int dentro = 0;
    for (int i = 0, j = numVertices - 1; i < numVertices; j = i++) {
        float xi = deslx + pontos[i][0] * escalax;
        float yi = desly + pontos[i][1] * escalay;
        float xj = deslx + pontos[j][0] * escalax;
        float yj = desly + pontos[j][1] * escalay;

        if (((yi > y) != (yj > y)) &&
            (x < (xj - xi) * (y - yi) / (yj - yi + 0.000001f) + xi)) {
            dentro = !dentro;
        }
    }
    return dentro;
}

int pontoProximoDaAresta(float **pontos, int **arestas, int numArestas, float escalax, float escalay, float deslx,
                         float desly, int mouseX, int mouseY) {
    float mx = ((float) mouseX / WIDTH) * 2 - 1;
    float my = -(((float) mouseY / HEIGHT) * 2 - 1);

    float margem = 0.02f;

    for (int i = 0; i < numArestas; i++) {
        int idx1 = arestas[i][0];
        int idx2 = arestas[i][1];

        float x1 = deslx + pontos[idx1][0] * escalax;
        float y1 = desly + pontos[idx1][1] * escalay;
        float x2 = deslx + pontos[idx2][0] * escalax;
        float y2 = desly + pontos[idx2][1] * escalay;

        float dx = x2 - x1;
        float dy = y2 - y1;
        float len2 = dx * dx + dy * dy;
        if (len2 == 0.0f) continue;

        float t = ((mx - x1) * dx + (my - y1) * dy) / len2;
        t = fmax(0, fmin(1, t));

        float projx = x1 + t * dx;
        float projy = y1 + t * dy;

        float dist2 = (mx - projx) * (mx - projx) + (my - projy) * (my - projy);
        if (dist2 < margem * margem) {
            return 1;
        }
    }
    return 0;
}

void liberaObjetos(Objeto *objetos, int numObjetos) {
    for (int i = 0; i < numObjetos; i++) {
        free(objetos[i].pontosOriginais);
        free(objetos[i].pontosTransformados);
        free(objetos[i].arestas);
    }
    free(objetos);
}
