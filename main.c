#include <stdio.h>
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

void desenhaObjeto(SDL_Renderer *renderer, int n, float escalax, float escalay, float deslx, float desly, float **pontos){
    int i;
    float x1, x2, y1, y2;
    //printf("Desenhando...\n");
    for(i = 0; i<n; i++){
        //printf("Calculando i = %d\n", i);
        //printf("Valores objeto: x1: %f, y1: %f, x2: %f, y2: %f\n", pontos[i][0], pontos[i][1], pontos[i+1][0], pontos[i+1][1]);
        x1 = deslx + (pontos[i][0] * escalax);
        y1 = desly + (pontos[i][1] * escalay);
        x2 = deslx + (pontos[i+1][0] * escalax);
        y2 = desly + (pontos[i+1][1] * escalay);
        //printf("Valores unificado: x1: %f, y1: %f, x2: %f, y2: %f\n", x1, y1, x2, y2);
        //("Desenhando i = %d\n", i);
        //SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        desenhaAresta(renderer, x1, y1, x2, y2);
    }
        x1 = deslx + pontos[i][0] * escalax;
        y1 = desly + pontos[i][1] * escalay;
        x2 = deslx + pontos[0][0] * escalax;
        y2 = desly + pontos[0][1] * escalay;
    desenhaAresta(renderer, x1, y1, x2, y2);
}

int main(int argc, char *argv[]){
    int i;
    float escalax = 0.1;
    float escalay = 0.1;
    float deslx = -0.9;
    float desly = -0.4;
    float **pontos = (float **) malloc(9*sizeof(float*));
    for(i = 0; i<9; i++) pontos[i] = (float *) malloc(2*sizeof(float));

    pontos[0][0] = 0;
    pontos[0][1] = 0;
    pontos[1][0] = 0;
    pontos[1][1] = 4;
    pontos[2][0] = 2;
    pontos[2][1] = 6;
    pontos[3][0] = 3;
    pontos[3][1] = 5;
    pontos[4][0] = 3;
    pontos[4][1] = 5.5;
    pontos[5][0] = 3.5;
    pontos[5][1] = 5.5;
    pontos[6][0] = 3.5;
    pontos[6][1] = 4.5;
    pontos[7][0] = 4;
    pontos[7][1] = 4;
    pontos[8][0] = 4;
    pontos[8][1] = 0;

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
        desenhaObjeto(renderer, 8, escalax, escalay, deslx, desly, pontos);
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
