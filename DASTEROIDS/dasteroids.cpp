#pragma warning(disable : 4996)

#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>
#include<iostream>
#include<cmath>
#include<ctime>
#include<cstdlib>
#undef main

const float PI = 3.141492;
const int WIN_HEIGHT = 600;
const int WIN_WIDTH = 800;
const int PLR_HEIGHT = 30;
const int PLR_WIDTH = 30;
const int BLLT_HEIGHT = 30;
const int BLLT_WIDTH = 30;
const int speed = 8;
const int speed_bullet = 25;

bool start = 0, hard = 0;

float x = WIN_WIDTH / 2, y = WIN_HEIGHT / 2, angle = 0;
float x_gl, y_gl, angle_gl;

int score = 0, nrasteroids = 20;

float velocity = 0.0f;
bool up = 0, down = 0, right = 0, left = 0;
bool show_bullet = 0;

char scoretext[100];

struct asteroid {
    SDL_Texture* asteroid;
    int x, y;
    int ASTER_HEIGHT, ASTER_WIDTH;
    int angle_aster, speed;
    bool show;
}asteroid[20];

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Surface* surface = nullptr;
SDL_Texture* player = nullptr;
SDL_Texture* glont = nullptr;
SDL_Texture* text = nullptr;
SDL_Texture* deadtext = nullptr;
SDL_Texture* welcome = nullptr;
SDL_Texture* indications = nullptr;
SDL_Rect playerRect;
SDL_Rect bulletRect;
SDL_Rect asteroidRect;
Mix_Chunk* music;
Mix_Music* back;

void check_angle() {
    if (angle > 360) angle -= 360;
    if (angle < 0) angle += 360;
}

float to_radians() {
    return angle * PI / 180.0f;
}

void velocity_reducer() {
    if (velocity >= 0.2f)
        velocity -= 0.1f;
    if (velocity <= 0.2f) velocity = 0.0f, left = 0, right = 0, up = 0, down = 0;
}

int nrcifre() {
    int aux = score, c = 0;
    do {
        c++;
        aux /= 10;
    } while (aux);
    return c;
}

void init();
void play();
void key(bool& quit, bool& restart);
void move(bool& restart);
void render(bool& restart);
void quit();

int main()
{
    init();
    play();
    quit();
    return 0;
}

void init() {
    srand(time(NULL));

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Init(SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);
    music = Mix_LoadWAV("shoot.mp3");
    back = Mix_LoadMUS("background.mp3");
    Mix_PlayMusic(back, -1);


    window = SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    surface = IMG_Load("player.png");
    if (surface == nullptr) {
        std::cout << "Unable to load player texture. SDL_image Error: " << IMG_GetError() << std::endl;
        // Handle error appropriately
    }
    player = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("glont.png");
    if (surface == nullptr) {
        std::cout << "Unable to load bullet texture. SDL_image Error: " << IMG_GetError() << std::endl;
        // Handle error appropriately
    }
    glont = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("asteroid.png");
    if (surface == nullptr) {
        std::cout << "Unable to load asteroid texture. SDL_image Error: " << IMG_GetError() << std::endl;
        // Handle error appropriately
    }
    for (int i = 0; i < 20; i++) {
        asteroid[i].asteroid = SDL_CreateTextureFromSurface(renderer, surface);
        asteroid[i].x = rand() % 800;
        asteroid[i].y = rand() % 600;
        asteroid[i].ASTER_HEIGHT = rand() % 40 + 20;
        asteroid[i].ASTER_WIDTH = rand() % 40 + 20;
        asteroid[i].angle_aster = rand() % 360;
        asteroid[i].speed = rand() % 4 + 4;
        asteroid[i].show = 1;
        if (asteroid[i].x + asteroid[i].ASTER_WIDTH >= WIN_WIDTH / 2 && asteroid[i].x + asteroid[i].ASTER_WIDTH <= WIN_WIDTH / 2 + 50) asteroid[i].x -= 200;
        if (asteroid[i].y + asteroid[i].ASTER_HEIGHT >= WIN_HEIGHT / 2 && asteroid[i].y + asteroid[i].ASTER_HEIGHT <= WIN_HEIGHT / 2 + 50) asteroid[i].y -= 200;
    }
    SDL_FreeSurface(surface);

    sprintf(scoretext, "%d", score);
    TTF_Font* font = TTF_OpenFont("pixel_fundal.ttf", 20);
    surface = TTF_RenderText_Solid(font, scoretext, { 255, 255, 255, 255 });
    if (surface == nullptr) {
        std::cout << "Unable to load font. SDL_ttf Error: " << TTF_GetError() << std::endl;
    }
    text = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = TTF_RenderText_Solid(font, "Press 'r' to restart or 'q' to quit.", { 255, 255, 255, 255 });
    if (surface == nullptr) {
        std::cout << "Unable to load font. SDL_ttf Error: " << TTF_GetError() << std::endl;
    }
    deadtext = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = TTF_RenderText_Solid(font, "DASTEROIDS", { 255, 255, 255, 255 });
    if (surface == nullptr) {
        std::cout << "Unable to load font. SDL_ttf Error: " << TTF_GetError() << std::endl;
    }
    welcome = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = TTF_RenderText_Solid(font, "Press 'e' for EASY, 'a' for HARDCORE or 'q' to QUIT.", { 255, 255, 255, 255 });
    if (surface == nullptr) {
        std::cout << "Unable to load font. SDL_ttf Error: " << TTF_GetError() << std::endl;
    }
    indications = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

void play() {
    bool quit = false, restart = false;
    while (!quit) {
        //Key detection
        key(quit, restart);

        if (!restart) {
            //Movement & Detection
            if (start)
                move(restart);

            //Rendering
            render(restart);
        }
    }
}

void key(bool& quit, bool& restart) {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT)
            quit = 1;
        else if (e.type == SDL_KEYDOWN) {
            if (!start) {
                switch (e.key.keysym.sym) {
                case SDLK_e:
                    Mix_PlayChannel(-1, music, 0);
                    start = 1;
                    hard = 0;
                    for (int i = 10; i < 20; i++)
                        asteroid[i].show = 0;
                    nrasteroids = 10;
                    break;
                case SDLK_a:
                    Mix_PlayChannel(-1, music, 0);
                    start = 1;
                    hard = 1;
                    break;
                case SDLK_q:
                    quit = 1;
                    break;
                }
            }
            else {
                if (hard)
                    switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        up = 1;
                        down = 0;
                        left = 0;
                        right = 0;
                        x += cos(angle * PI / 180.0f) * speed;
                        y += sin(angle * PI / 180.0f) * speed;
                        velocity = 5.0f;
                        break;
                    case SDLK_DOWN:
                        up = 0;
                        down = 1;
                        left = 0;
                        right = 0;
                        x -= cos(angle * PI / 180.0f) * speed;
                        y -= sin(angle * PI / 180.0f) * speed;
                        velocity = 5.0f;
                        break;
                    case SDLK_LEFT:
                        //angle -= 10;
                        //check_angle();
                        left = 1;
                        right = 0;
                        up = 0;
                        down = 0;
                        x -= sin(angle * PI / 180.0f) * speed;
                        y -= cos(angle * PI / 180.0f) * speed;
                        velocity = 5.0f;
                        break;
                    case SDLK_RIGHT:
                        //angle += 10;
                        //check_angle();
                        left = 0;
                        right = 1;
                        up = 0;
                        down = 0;
                        x += sin(angle * PI / 180.0f) * speed;
                        y += cos(angle * PI / 180.0f) * speed;
                        velocity = 5.0f;
                        break;
                    case SDLK_z:
                        angle -= 10;
                        check_angle();
                        break;
                    case SDLK_x:
                        angle += 10;
                        check_angle();
                        break;
                    case SDLK_SLASH:
                        angle += 180;
                        check_angle();
                        break;
                    case SDLK_SPACE:
                        if (show_bullet == 0 && restart == 0) {
                            Mix_PlayChannel(-1, music, 0);
                            x_gl = x;
                            y_gl = y;
                            angle_gl = angle;
                            show_bullet = 1;
                        }
                        break;
                    case SDLK_q:
                        quit = 1;
                        break;
                    case SDLK_r:
                        if (restart) {
                            restart = 0;
                            x = WIN_WIDTH / 2, y = WIN_HEIGHT / 2, angle = 0;
                            score -= 1000;
                            sprintf(scoretext, "%d", score);
                            TTF_Font* font = TTF_OpenFont("pixel_fundal.ttf", 20);
                            surface = TTF_RenderText_Solid(font, scoretext, { 255, 255, 255, 255 });
                            text = SDL_CreateTextureFromSurface(renderer, surface);
                            for (int i = 0; i < 20; i++) {
                                asteroid[i].x = rand() % 800;
                                asteroid[i].y = rand() % 600;
                                asteroid[i].ASTER_HEIGHT = rand() % 40 + 20;
                                asteroid[i].ASTER_WIDTH = rand() % 40 + 20;
                                asteroid[i].angle_aster = rand() % 360;
                                asteroid[i].speed = rand() % 4 + 4;
                                asteroid[i].show = 1;
                                if (asteroid[i].x + asteroid[i].ASTER_WIDTH >= WIN_WIDTH / 2 && asteroid[i].x + asteroid[i].ASTER_WIDTH <= WIN_WIDTH / 2 + 50) asteroid[i].x -= 200;
                                if (asteroid[i].y + asteroid[i].ASTER_HEIGHT >= WIN_HEIGHT / 2 && asteroid[i].y + asteroid[i].ASTER_HEIGHT <= WIN_HEIGHT / 2 + 50) asteroid[i].y -= 200;
                            }
                            nrasteroids = 20;
                            SDL_Delay(500);
                            render(restart);
                        }
                    }
                else {
                    switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        up = 1;
                        down = 0;
                        //left = 0;
                        //right = 0;
                        x += cos(angle * PI / 180.0f) * speed;
                        y += sin(angle * PI / 180.0f) * speed;
                        velocity = 5.0f;
                        break;
                    case SDLK_DOWN:
                        up = 0;
                        down = 1;
                        //left = 0;
                        //right = 0;
                        x -= cos(angle * PI / 180.0f) * speed;
                        y -= sin(angle * PI / 180.0f) * speed;
                        velocity = 5.0f;
                        break;
                    case SDLK_LEFT:
                        angle -= 10;
                        check_angle();
                        break;
                    case SDLK_RIGHT:
                        angle += 10;
                        check_angle();
                        break;
                    case SDLK_SLASH:
                        angle += 180;
                        check_angle();
                        break;
                    case SDLK_SPACE:
                        if (show_bullet == 0 && restart == 0) {
                            Mix_PlayChannel(-1, music, 0);
                            x_gl = x;
                            y_gl = y;
                            angle_gl = angle;
                            show_bullet = 1;
                        }
                        break;
                    case SDLK_q:
                        quit = 1;
                        break;
                    case SDLK_r:
                        if (restart) {
                            restart = 0;
                            x = WIN_WIDTH / 2, y = WIN_HEIGHT / 2, angle = 0;
                            score -= 1000;
                            sprintf(scoretext, "%d", score);
                            TTF_Font* font = TTF_OpenFont("pixel_fundal.ttf", 20);
                            surface = TTF_RenderText_Solid(font, scoretext, { 255, 255, 255, 255 });
                            text = SDL_CreateTextureFromSurface(renderer, surface);
                            for (int i = 0; i < 20; i++) {
                                asteroid[i].x = rand() % 800;
                                asteroid[i].y = rand() % 600;
                                asteroid[i].ASTER_HEIGHT = rand() % 40 + 20;
                                asteroid[i].ASTER_WIDTH = rand() % 40 + 20;
                                asteroid[i].angle_aster = rand() % 360;
                                asteroid[i].speed = rand() % 4 + 4;
                                asteroid[i].show = 1;
                                if (asteroid[i].x + asteroid[i].ASTER_WIDTH >= WIN_WIDTH / 2 && asteroid[i].x + asteroid[i].ASTER_WIDTH <= WIN_WIDTH / 2 + 50) asteroid[i].x -= 200;
                                if (asteroid[i].y + asteroid[i].ASTER_HEIGHT >= WIN_HEIGHT / 2 && asteroid[i].y + asteroid[i].ASTER_HEIGHT <= WIN_HEIGHT / 2 + 50) asteroid[i].y -= 200;
                            }
                            if (hard == 0) {
                                for (int i = 10; i < 19; i++)
                                    asteroid[i].show = 0;
                                nrasteroids = 10;
                            }
                            else nrasteroids = 20;
                            SDL_Delay(500);
                            render(restart);
                        }
                    }
                }
            }
        }
    }
}

void move(bool& restart) {
    if (start) {
        if (up)
            x += cos(angle * PI / 180.0f) * velocity, y += sin(angle * PI / 180.0f) * velocity;
        if (down)
            x -= cos(angle * PI / 180.0f) * velocity, y -= sin(angle * PI / 180.0f) * velocity;
        if (hard) {
            if (left)
                x += sin(angle * PI / 180.0f) * velocity, y += cos(angle * PI / 180.0f) * velocity;
            if (right)
                x -= sin(angle * PI / 180.0f) * velocity, y -= cos(angle * PI / 180.0f) * velocity;
        }
        velocity_reducer();

        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x > WIN_WIDTH - PLR_WIDTH) x = WIN_WIDTH - PLR_WIDTH;
        if (y > WIN_HEIGHT - PLR_HEIGHT) y = WIN_HEIGHT - PLR_HEIGHT;

        for (int i = 0; i < 20; i++) {
            if (asteroid[i].show) {
                asteroidRect = { asteroid[i].x, asteroid[i].y, asteroid[i].ASTER_HEIGHT, asteroid[i].ASTER_WIDTH };
                if (SDL_HasIntersection(&playerRect, &asteroidRect))
                    restart = 1;
            }
        }

        if (show_bullet) {
            x_gl += cos(angle_gl * PI / 180.0f) * speed_bullet;
            y_gl += sin(angle_gl * PI / 180.0f) * speed_bullet;
            if (x_gl <= -15 || y_gl <= -15 || x_gl + BLLT_WIDTH >= WIN_WIDTH + 15 || y_gl + BLLT_HEIGHT >= WIN_HEIGHT + 15)
                show_bullet = 0;
            else
                for (int i = 0; i < 20; i++) {
                    if (asteroid[i].show) {
                        asteroidRect = { asteroid[i].x, asteroid[i].y, asteroid[i].ASTER_HEIGHT, asteroid[i].ASTER_WIDTH };
                        if (SDL_HasIntersection(&bulletRect, &asteroidRect)) {
                            x_gl = x;
                            y_gl = y;
                            show_bullet = 0;
                            score += 200;
                            sprintf(scoretext, "%d", score);
                            TTF_Font* font = TTF_OpenFont("pixel_fundal.ttf", 20);
                            surface = TTF_RenderText_Solid(font, scoretext, { 255, 255, 255, 255 });
                            text = SDL_CreateTextureFromSurface(renderer, surface);
                            asteroid[i].show = 0;
                            nrasteroids--;
                            break;
                        }
                    }
                }
        }

        for (int i = 0; i < 20; i++) {
            if (asteroid[i].show) {
                asteroid[i].x += cos(asteroid[i].angle_aster * PI / 180.0f) * asteroid[i].speed;
                asteroid[i].y += sin(asteroid[i].angle_aster * PI / 180.0f) * asteroid[i].speed;

                if (asteroid[i].x <= -40 || asteroid[i].x + asteroid[i].ASTER_WIDTH >= WIN_WIDTH + 40 || asteroid[i].y <= -40 || asteroid[i].y + asteroid[i].ASTER_HEIGHT >= WIN_HEIGHT + 40) {
                    asteroid[i].angle_aster = rand() % 360;
                    asteroid[i].speed = rand() % 4 + 4;
                }
            }
        }

        if (nrasteroids == 0) {
            x = WIN_WIDTH / 2, y = WIN_HEIGHT / 2, angle = 0;
            score += 1000;
            sprintf(scoretext, "%d", score);
            TTF_Font* font = TTF_OpenFont("pixel_fundal.ttf", 20);
            surface = TTF_RenderText_Solid(font, scoretext, { 255, 255, 255, 255 });
            text = SDL_CreateTextureFromSurface(renderer, surface);
            for (int i = 0; i < 20; i++) {
                asteroid[i].x = rand() % 800;
                asteroid[i].y = rand() % 600;
                asteroid[i].ASTER_HEIGHT = rand() % 40 + 20;
                asteroid[i].ASTER_WIDTH = rand() % 40 + 20;
                asteroid[i].angle_aster = rand() % 360;
                asteroid[i].speed = rand() % 4 + 4;
                asteroid[i].show = 1;
                if (asteroid[i].x + asteroid[i].ASTER_WIDTH >= WIN_WIDTH / 2 && asteroid[i].x + asteroid[i].ASTER_WIDTH <= WIN_WIDTH / 2 + 50) asteroid[i].x -= 200;
                if (asteroid[i].y + asteroid[i].ASTER_HEIGHT >= WIN_HEIGHT / 2 && asteroid[i].y + asteroid[i].ASTER_HEIGHT <= WIN_HEIGHT / 2 + 50) asteroid[i].y -= 200;
            }
            if (hard == 0) {
                for (int i = 10; i < 20; i++)
                    asteroid[i].show = 0;
                nrasteroids = 10;
            }
            else nrasteroids = 20;
            SDL_Delay(500);
        }
    }
}

void render(bool& restart) {
    SDL_RenderClear(renderer);

    if (!start) {
        SDL_Rect welcometext = { 300, 200, 200, 20 };
        SDL_RenderCopy(renderer, welcome, NULL, &welcometext);

        SDL_Rect destext = { 20, 400, 760, 20 };
        SDL_RenderCopy(renderer, indications, NULL, &destext);
    }
    else {
        playerRect = { (int)x, (int)y, PLR_HEIGHT, PLR_WIDTH };
        SDL_RenderCopyEx(renderer, player, NULL, &playerRect, angle, NULL, SDL_FLIP_NONE);

        if (show_bullet) {
            bulletRect = { (int)x_gl, (int)y_gl, BLLT_HEIGHT, BLLT_WIDTH };
            SDL_RenderCopyEx(renderer, glont, NULL, &bulletRect, angle_gl, NULL, SDL_FLIP_NONE);
        }

        for (int i = 0; i < 20; i++) {
            if (asteroid[i].show) {
                asteroidRect = { asteroid[i].x, asteroid[i].y, asteroid[i].ASTER_HEIGHT, asteroid[i].ASTER_WIDTH };
                SDL_RenderCopyEx(renderer, asteroid[i].asteroid, NULL, &asteroidRect, asteroid[i].angle_aster, NULL, SDL_FLIP_NONE);
            }
        }

        SDL_Rect texttext = { 10, 10, 20 * nrcifre(), 20 };
        SDL_RenderCopy(renderer, text, NULL, &texttext);

        if (restart) {
            SDL_Rect textdead = { 40, WIN_HEIGHT / 2 - 20, 720, 20 };
            SDL_RenderCopy(renderer, deadtext, NULL, &textdead);
        }
    }

    SDL_RenderPresent(renderer);


    SDL_Delay(30);
}

void quit() {
    for (int i = 0; i < 20; i++)
        SDL_DestroyTexture(asteroid[i].asteroid);
    SDL_DestroyTexture(text);
    SDL_DestroyTexture(deadtext);
    SDL_DestroyTexture(welcome);
    SDL_DestroyTexture(indications);
    SDL_DestroyTexture(player);
    SDL_DestroyTexture(glont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    std::cout << "Score: " << score;
}
