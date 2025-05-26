#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>

#define WIDTH 800
#define HEIGHT 600

void desenhaAresta(SDL_Renderer *renderer, float x1, float y1, float x2, float y2){
    int xa, xb, ya, yb;
    //printf("Valores unificado: x1: %f, y1: %f, x2: %f, y2: %f\n", x1, y1, x2, y2);
    xa = WIDTH * ((x1 + 1)/2);
    xb = WIDTH * ((x2 + 1)/2);
    ya = HEIGHT * ((-y1 + 1)/2);
    yb = HEIGHT * ((-y2 + 1)/2);
    //printf("Valores dispositivo: xa: %d, ya: %d, xb: %d, yb: %d\n", xa, ya, xb, yb);
    SDL_RenderDrawLine(renderer, xa, ya, xb, yb);
}

void desenhaObjeto(SDL_Renderer *renderer, int numArestas, float escalax, float escalay, float deslx, float desly, float **pontos, int **arestas){
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

int main(int argc, char *argv[]){
    int i;
    float escalax = 0.1;
    float escalay = 0.1;
    float deslx = -0.9;
    float desly = -0.4;

    float angulo = 0.0;  // em radianos

    float **pontos = NULL;
    int **arestas = NULL;
    int numVertices = 0, numArestas = 0;

    if (!carregarObjeto("cubo.txt", &pontos, &numVertices, &arestas, &numArestas)) {
        return EXIT_FAILURE;
    }

    float **pontosTransformados = (float **) malloc(numVertices * sizeof(float *));
    for (int i = 0; i < numVertices; i++) {
        pontosTransformados[i] = (float *) malloc(2 * sizeof(float));
    }

    if(SDL_Init(SDL_INIT_EVERYTHING)<0){
        printf("Deu erro!!! SDL error %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window *window = SDL_CreateWindow("SDL Hello World!!!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
    if(window == NULL){
        printf("Deu erro na janela!!! SDL error %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Event windowEvent;
    SDL_Renderer *renderer = SDL_CreateRenderer(window,-1,0);
    while(1){
        aplicarRotacaoComCentro(pontos, pontosTransformados, numVertices, angulo);

        if( SDL_PollEvent(&windowEvent)){
            if(windowEvent.type == SDL_QUIT){
                break;
            }
            if(windowEvent.type == SDL_KEYDOWN){
                // https://youtu.be/YfGYU7wWLo8?si=ULTr7xdT59_WyaQC
                if(windowEvent.key.keysym.sym == SDLK_a){
                    deslx -= 0.01;
                }
                if(windowEvent.key.keysym.sym == SDLK_s){
                    desly -= 0.01;
                }
                if(windowEvent.key.keysym.sym == SDLK_d){
                    deslx += 0.01;
                }
                if(windowEvent.key.keysym.sym == SDLK_w){
                    desly += 0.01;
                }
            }
            if(windowEvent.type == SDL_KEYUP){
                if(windowEvent.key.keysym.sym == SDLK_a){
                    printf("Tecla a liberada\n");
                }
                if(windowEvent.key.keysym.sym == SDLK_s){
                    printf("Tecla s liberada\n");
                }
                if(windowEvent.key.keysym.sym == SDLK_d){
                    printf("Tecla d liberada\n");
                }
                if(windowEvent.key.keysym.sym == SDLK_w){
                    printf("Tecla w liberada\n");
                }
            }
            if(windowEvent.type == SDL_MOUSEBUTTONDOWN){
                if(windowEvent.button.button == SDL_BUTTON_LEFT){
                    printf("esquerdo\n");
                }
                if(windowEvent.button.button == SDL_BUTTON_RIGHT){
                    printf("direito\n");
                }
                if(windowEvent.button.button == SDL_BUTTON_MIDDLE){
                    printf("meio\n");
                }
            }
            if(windowEvent.type == SDL_MOUSEWHEEL){
                if(windowEvent.wheel.y > 0){
                    escalax += 0.01;
                    escalay += 0.01;
                }
                if(windowEvent.wheel.y < 0){
                    escalax -= 0.01;
                    escalay -= 0.01;
                }
            }
            if(windowEvent.type == SDL_MOUSEMOTION){
                int xmouse, ymouse;
                SDL_GetMouseState(&xmouse, &ymouse);
                printf("Mouse :: %3d %3d\n", xmouse, ymouse);
            }

            if ((windowEvent.key.keysym.mod & KMOD_SHIFT) && windowEvent.key.keysym.sym == SDLK_q) {
                angulo -= 0.1;  // rotaciona levemente para esquerda
            }
            if ((windowEvent.key.keysym.mod & KMOD_SHIFT) && windowEvent.key.keysym.sym == SDLK_e) {
                angulo += 0.1;  // rotaciona levemente para direita
            }

        }

        SDL_SetRenderDrawColor(renderer, 242, 242, 242, 255);
        SDL_RenderClear(renderer);

//SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        //SDL_RenderDrawLine(renderer, 0, 0, 80, 600);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        //escalax = 0.1;
        //escalay = 0.1;
        //deslx = -0.9;
        //desly = -0.4;
        //AB
        //desenhaObjeto(renderer, 8, escalax, escalay, deslx, desly, pontos);
        //desenhaObjeto(renderer, numArestas, escalax, escalay, deslx, desly, pontos, arestas);
        desenhaObjeto(renderer, numArestas, escalax, escalay, deslx, desly, pontosTransformados, arestas);


        //SDL_RenderDrawLine(renderer, 10, 100, 10, 200);
/*
        escalax = 0.05;
        escalay = 0.05;
        deslx = -0.300;
        desly = -0.200;
        //AB
        desenhaObjeto(renderer, 8, escalax, escalay, deslx, desly, pontos);

        SDL_SetRenderDrawColor(renderer, 120, 0, 0, 255);
        escalax = 0.01;
        escalay = 0.01;
        deslx = -0.450;
        desly = -0.320;
        //AB
        desenhaObjeto(renderer, 8, escalax, escalay, deslx, desly, pontos);*/

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
