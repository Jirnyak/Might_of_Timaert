#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <unistd.h>
#include <chrono>
#include <string>
#include <tuple>
#include <math.h>  
using namespace std;

#define WORLD_WIDTH 1000
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 750
#define SIDE_MENU (WINDOW_WIDTH-WINDOW_HEIGHT)/2
#define MAXA 10000
#define DECAY 10

#define RELIEF_TEST 1000

#define PERCENT 100

#define TICK_INTERVAL 30

#define MAX_SPEED 1000

#define ISLANDS 100

/* */

/*

cd /Users/jirnyak/Mirror/DND

makefile 

game:
    CXX=g++-12 CC=gcc-12
    g++-12 rogue.cpp -o test -I /Library/Frameworks/SDL2.framework/Headers -F /Library/Frameworks -framework SDL2 -I /Library/Frameworks/SDL2_image.framework/Headers -F /Library/Frameworks -framework SDL2_image -I /Library/Frameworks/SDL2_ttf.framework/Headers -F /Library/Frameworks -framework SDL2_ttf -fopenmp -std=c++11 
*/

class object
{
    public:

        int x;  
        int y; 

        float x_loc;
        float y_loc;

        float v_x;
        float v_y;

        bool plant = false;

        bool player = false;

        bool mob = false; 

        bool dead;

        int hp;

        float speed;

        int aim;

        object(int x, int y, int type) 
        { 

            this->x = x;
            this->y = y;

            this->x_loc = x + 0.5;
            this->y_loc = y + 0.5;

            this->speed = 0.1;

            this->v_x = 0.0;
            this->v_y = 0.0;

            this->hp = 1000;

            this->dead = false;

            this->aim = -1;

            if (type == 0)
                this->player = true;

            if (type == 1)
                this->plant = true;

            if (type == 2)
                this->mob = true;
        }

        int cell_number()
        {
            return x*WORLD_WIDTH + y;
        }
};

class NPC: public object
{
public:

    double poo;
    double pee;

};

class cell 
{
    private: 
        int x;  
        int y;   
        int number;
        cell *address;
    public:      
        cell *sosed_up;   
        cell *sosed_left; 
        cell *sosed_down; 
        cell *sosed_right; 
        cell *sosed_upleft;
        cell *sosed_upright;
        cell *sosed_downleft;
        cell *sosed_downright; 

        vector<object> inside;

        int R;
        int G;
        int B;

        bool dead;

        bool dead1;

        bool terra;

        bool sand;

        bool river;

        int direction;

        bool darwin;

        bool water;

        cell(int x, int y) 
        { 
            this->x = x;
            this->y = y;

            this->number = x*WORLD_WIDTH + y;

            this->terra = 0;

            this->darwin = 0;

            this->dead = 0;

            this->dead1 = 0;

            this->sand = 0;

            this->water = 0;

            vector<object> inside;
        }
        void gen_address()
        {
            this->address = this;
        }
        void info()
        {
            cout << number << " " << x << " " << y << " " << sosed_up->x << sosed_up->y << "\n";
        }
        int get_x()
        {
            return x;
        }
        int get_y()
        {
            return y;
        }
        cell* get_address()
        {
            return address;
        }
        void up(cell *c)
        {
            sosed_up = c;
        }
        void down(cell *c)
        {
            sosed_down = c;
        }
        void left(cell *c)
        {
            sosed_left = c;
        }
        void right(cell *c)
        {
            sosed_right = c;
        }
        void upright(cell *c)
        {
            sosed_upright = c;
        }
        void upleft(cell *c)
        {
            sosed_upleft = c;
        }
        void downright(cell *c)
        {
            sosed_downright = c;
        }
        void downleft(cell *c)
        {
            sosed_downleft = c;
        }
        int get_n()
        {
            return number;
        }
        cell* side(int d)
        {
            if (d == 0)
                return sosed_up;
            if (d == 1)
                return sosed_upleft;
            if (d == 2)
                return sosed_left;
            if (d == 3)
                return sosed_downleft;
            if (d == 4)
                return sosed_down;
            if (d == 5)
                return sosed_downright;
            if (d == 6)
                return sosed_right;
            if (d == 7)
                return sosed_upright;
            else
                return address;
        }
        cell* side_spiral(int d)
        {
            if (d == 0)
                return sosed_up;
            if (d == 1)
                return sosed_left;
            if (d == 2)
                return sosed_down;
            if (d == 3)
                return sosed_right;
            else
                return address;
        }
};

/*
class mob: public object
{
    public:

        std::string name;

        int speed;

};

std::tuple<int, int, int> RGB(int R, int G, int B) 
{
    return std::make_tuple(R%255, G%255, B%255); 
}
*/

static Uint32 next_time;

Uint32 time_left(void)
{
    Uint32 now;

    now = SDL_GetTicks();
    if(next_time <= now)
        return 0;
    else
        return next_time - now;
}

using rng_t = std::mt19937;

std::random_device dev;

std::mt19937 rng(dev());

uint32_t randomer(rng_t& rng, uint32_t range) 
{
    range += 1;
    uint32_t x = rng();
    uint64_t m = uint64_t(x) * uint64_t(range);
    uint32_t l = uint32_t(m);
    if (l < range) {
        uint32_t t = -range;
        if (t >= range) {
            t -= range;
            if (t >= range) 
                t %= range;
        }
        while (l < t) {
            x = rng();
            m = uint64_t(x) * uint64_t(range);
            l = uint32_t(m);
        }
    }
    return m >> 32;
}

int tor_cord(int x)
{
    if (x < 0)
    {
        x = WORLD_WIDTH + x%WORLD_WIDTH;
    }
    else if (x >= WORLD_WIDTH)
    {
        x = x%WORLD_WIDTH;
    }
    return x;
}

void spiral(vector<cell> world, int size, int start)
{
    cell* pos = &world[start];
    int use = 1;
    int k = 0;
    for (int i = 0; i < size; i++)
    {
        for (int a = 0; a < use; a++)   
        {
            pos = pos->side_spiral(k%4); 
        }
        k += 1;
        for (int b = 0; b < use; b++)   
        {
            pos = pos->side_spiral(k%4); 
        }
        use += 1;
    }
}

int tile_spiral(int d)
        {
            if (d == 0)
                return -1;
            if (d == 1)
                return -1;
            if (d == 2)
                return +1;
            if (d == 3)
                return +1;
            else
                return 0;
        }

int main()
{
    int TILE_SIZE = 10;

    int tiles_in_window = WINDOW_HEIGHT/TILE_SIZE;

    int center_tile = tiles_in_window/2;

    vector<cell> world;
    vector<cell>::iterator it;

    vector<object> objects_buff;
    vector<object>::iterator it_obj;
    vector<object>::iterator it_inside;

    for (int i=0; i<WORLD_WIDTH; ++i)
    {
        for (int j=0; j<WORLD_WIDTH; ++j)
        {       
            world.push_back(cell(i,j));
        }
    }

    for (it = world.begin(); it != world.end(); ++it)
    {
        it->gen_address();
    }

    int delilo = WORLD_WIDTH*WORLD_WIDTH;

    for (it = world.begin(); it != world.end(); ++it)
    {
        vector<cell>::iterator it1;
        it->up(&world[tor_cord(it->get_x())*WORLD_WIDTH + tor_cord(it->get_y()-1)]);
        it->down(&world[tor_cord(it->get_x())*WORLD_WIDTH + tor_cord(it->get_y()+1)]);
        it->left(&world[tor_cord(it->get_x()-1)*WORLD_WIDTH + tor_cord(it->get_y())]);
        it->right(&world[tor_cord(it->get_x()+1)*WORLD_WIDTH + tor_cord(it->get_y())]);
        it->upleft(&world[tor_cord(it->get_x()-1)*WORLD_WIDTH + tor_cord(it->get_y()-1)]);
        it->upright(&world[tor_cord(it->get_x()+1)*WORLD_WIDTH + tor_cord(it->get_y()-1)]);
        it->downleft(&world[tor_cord(it->get_x()-1)*WORLD_WIDTH + tor_cord(it->get_y()+1)]);
        it->downright(&world[tor_cord(it->get_x()+1)*WORLD_WIDTH + tor_cord(it->get_y()+1)]);
    }

    int LIMIT = 300000+randomer(rng, 100000);
    int gena = 0;
    int perbor = 0;
    int drop;
    int zapas = 0;
    while (gena < ISLANDS)
    {
        gena += 1;
        drop = randomer(rng, WORLD_WIDTH*WORLD_WIDTH);
        world[drop].terra = 1;
    }

    while (zapas < LIMIT)
    {
        #pragma omp parallel
        #pragma omp for
        for (it = world.begin(); it != world.end(); ++it)
        {
            perbor = randomer(rng, 7);
            if (it->terra == 1 and it->dead == 0 and it->side(perbor)->terra == 0 and zapas < LIMIT)
                {
                    it->side(perbor)->terra = 1;
                    zapas += 1;
                    drop = randomer(rng, 5);
                    if (drop == 0)
                        it->dead = 1;
                }
        } 
    }

    zapas = 0;

    while (true)
    {
        drop = randomer(rng, WORLD_WIDTH*WORLD_WIDTH);
        if (world[drop].terra != 1)
        {
        world[drop].darwin = 1;
        world[drop].terra = 1;
        break;
        }
    }

    while (zapas < 1000)
    {
        #pragma omp parallel
        #pragma omp for
        for (it = world.begin(); it != world.end(); ++it)
        {
            perbor = randomer(rng, 7);
            if (it->darwin == 1 and it->side(perbor)->darwin == 0)
                {
                    it->side(perbor)->terra = 1;
                    it->side(perbor)->darwin = 1;
                    zapas += 1;
                }
        } 
    }

    gena = 0;

    while (gena < ISLANDS*5)
    {
        drop = randomer(rng, WORLD_WIDTH*WORLD_WIDTH);
        if (world[drop].terra == 1 and world[drop].darwin != 1)
        {
            gena += 1;
            world[drop].river = 1;
            world[drop].direction = randomer(rng, 7);
        }
    }
    gena = 0;
    while (gena < 100)
    {
        gena += 1;
        #pragma omp parallel
        #pragma omp for
        for (it = world.begin(); it != world.end(); ++it)
        {
            perbor = randomer(rng, 7);
            if (it->river == 1 and it->dead1 == 0)
                {
                    it->side((perbor+it->direction)%8)->river = 1;
                    it->side((perbor+it->direction)%8)->dead1 = 0;
                    it->dead1 = 1;
                }
        } 
    }
    ofstream myfile;
    myfile.open ("mapa.txt");
    for (it = world.begin(); it != world.end(); ++it)
    {
        if (it->terra == 1 and it->river != 1)
            myfile << "[200 ,200, 0]" << "\n";
        else
            myfile << "[0, 0, 255]" << "\n";
    }
    myfile.close();
    myfile.open ("map.txt");
    for (it = world.begin(); it != world.end(); ++it)
    {
        if (it->terra == 1 and it->river != 1)
        {
            myfile << "200" << "\n";
            myfile << "200" << "\n";
            myfile << "0" << "\n";
            it->R = 200;
            it->G = 200;
            it->B = 0;
            if (it->darwin == 1)
            {
                it->R = 255;
                it->G = 0;
                it->B = 0;
                myfile << "255" << "\n";
                myfile << "0" << "\n";
                myfile << "0" << "\n";
            }
        }
        else
        {
            myfile << "0" << "\n";
            myfile << "0" << "\n";
            myfile << "255" << "\n";
            it->R = 0;
            it->G = 0;
            it->B = 255;
        }
    }
    myfile.close();

    #pragma omp parallel
    #pragma omp for
    for (it = world.begin(); it != world.end(); ++it)
    {
        if (it->terra == 1 and it->river != 1)
        {
            for (int i = 0; i < 8; i++) 
            {
                if (it->side(i)->terra != 1 or it->side(i)->river == 1)
                {
                    it->sand = 1;
                    it->water = 0;
                    break;
                }
            }
        }
        if (it->terra != 1 or it->river == 1)
        {
            it->water = 1;
            it->terra = 0;
        }
    } 

/*
    ifstream newfile;
    newfile.open("map.txt"); 
    if (newfile.is_open())
    {   
        string tp;
        while(getline(newfile, tp))
        {
            if (perbor%3 == 0)
            {
                perbor += 1;
                world[gena].R = stoi(tp);
            }
            else if (perbor%3 == 1)
            {
                perbor += 1;
                world[gena].G = stoi(tp);
            }
            else if (perbor%3 == 2)
            {
                perbor += 1;
                world[gena].B = stoi(tp);
                gena += 1;

            }
        }
        newfile.close(); //close the file object.
    }
*/

    perbor = 0;
    gena = 0;
    drop = 0;

    while (gena < 1000)
    {
        drop  = randomer(rng, WORLD_WIDTH*WORLD_WIDTH);
        if (world[drop].inside.empty() and world[drop].terra == 1)
        {
            world[drop].inside.push_back(object(world[drop].get_x(),world[drop].get_y(), 1));
            gena += 1;
        }
    }

    gena = 0;

    while (gena < 100)
    {
        drop  = randomer(rng, WORLD_WIDTH*WORLD_WIDTH);
        if (world[drop].inside.empty() and world[drop].terra == 1 and world[drop].darwin != 1)
        {
            world[drop].inside.push_back(object(world[drop].get_x(),world[drop].get_y(), 2));
            gena += 1;
        }
    }

    int player_n;

    while (true)
    {
        drop  = randomer(rng, WORLD_WIDTH*WORLD_WIDTH);
        if (world[drop].inside.empty() and world[drop].B != 255)
        {
            player_n = drop;
            world[drop+2].inside.push_back(object(world[drop+2].get_x(),world[drop+2].get_y(), 2));
            gena += 1;
            break;
        }

    }

    world[player_n].inside.push_back(object(world[player_n].get_x(),world[player_n].get_y(), 0));

    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    int curs_x;
    int curs_y;

    float koef;

    bool stop;

    bool buttonPressed = false;
    bool keyPressed = false;
    bool suck_mod = false;
    bool map_mod = false;
    bool paused = false;

    int key;

    SDL_Rect tile;
    tile.w = TILE_SIZE;
    tile.h = TILE_SIZE;

    SDL_Rect small_tile;
    small_tile.w = TILE_SIZE/10;
    small_tile.h = TILE_SIZE/10;

    SDL_Texture *player_texture[6];
    player_texture[0] = IMG_LoadTexture(renderer, "Untitled_Artwork-1.png");
    player_texture[1] = IMG_LoadTexture(renderer, "Untitled_Artwork-2.png");
    player_texture[2] = IMG_LoadTexture(renderer, "Untitled_Artwork-3.png");
    player_texture[3] = IMG_LoadTexture(renderer, "Untitled_Artwork-4.png");
    player_texture[4] = IMG_LoadTexture(renderer, "Untitled_Artwork-5.png");
    player_texture[5] = IMG_LoadTexture(renderer, "Untitled_Artwork-6.png");

    SDL_Texture *girl_texture;
    girl_texture = IMG_LoadTexture(renderer, "aim.png");

    SDL_Rect long_tile;
    long_tile.w = TILE_SIZE;
    long_tile.h = TILE_SIZE;

    int player_anim = 0;

    int player_speed = 1000;

    SDL_Texture *plant_texture = IMG_LoadTexture(renderer, "plant.png");

    SDL_Texture *cow_texture = IMG_LoadTexture(renderer, "cow.png");

    SDL_Texture *water_texture = IMG_LoadTexture(renderer, "water.png");

    SDL_Texture *grass_texture = IMG_LoadTexture(renderer, "grass.png");

    SDL_Texture *sand_texture = IMG_LoadTexture(renderer, "sand.png");

    SDL_Rect minimap;
    minimap.w = WORLD_WIDTH;
    minimap.h = WORLD_WIDTH;

    TTF_Init();
    TTF_Font *font = TTF_OpenFont("Roboto-Black.ttf", 20);

    SDL_Surface *info_suck = TTF_RenderText_Solid(font, "sucking on", {255,0,0});
    SDL_Texture *info_suck_text = SDL_CreateTextureFromSurface(renderer, info_suck);

    SDL_Surface *info_suck_off = TTF_RenderText_Solid(font, "sucking off", {0,0,0});
    SDL_Texture *info_suck_off_text = SDL_CreateTextureFromSurface(renderer, info_suck_off);

    SDL_Rect info_suck_rect;
    info_suck_rect.x = WORLD_WIDTH;  //controls the rect's x coordinate 
    info_suck_rect.y = 0; // controls the rect's y coordinte
    info_suck_rect.w = 200; // controls the width of the rect
    info_suck_rect.h = 100; // controls the height of the rect

    SDL_Surface *mocha_info;
    SDL_Texture *mocha_info_text;
    std::string mocha;
    std::string govno;

    SDL_Rect info_mocha_rect;
    info_mocha_rect.x = WORLD_WIDTH;  //controls the rect's x coordinate 
    info_mocha_rect.y = 100; // controls the rect's y coordinte
    info_mocha_rect.w = 200; // controls the width of the rect
    info_mocha_rect.h = 100; // controls the height of the rect

    SDL_Rect info_govno_rect;
    info_govno_rect.x = WORLD_WIDTH;  //controls the rect's x coordinate 
    info_govno_rect.y = 200; // controls the rect's y coordinte
    info_govno_rect.w = 200; // controls the width of the rect
    info_govno_rect.h = 100; // controls the height of the rect

    mocha = std::to_string(42);
    govno = std::to_string(42);

    keyPressed = false;

    bool occupied = false;

    int actions = 0;

    bool quit = 0;

    cell* pos;

    int use;
    int k;

    while (quit == false) 
    {
        player_anim += 1;
        player_anim = player_anim%6;

        //USER INPUT CHECK

        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                break;
            else if (event.type == SDL_MOUSEBUTTONDOWN and buttonPressed == false)
            {
                buttonPressed = true;
                SDL_GetMouseState(&curs_x, &curs_y);
            }
            else if (event.type == SDL_KEYDOWN and keyPressed == false)
            {
                key = event.key.keysym.sym;
                if (key == SDLK_s)
                {
                    suck_mod = not suck_mod;
                    keyPressed = true;
                }
                else if (key == SDLK_m)
                {
                    map_mod = not map_mod;
                }
                else if (key == SDLK_UP)
                {
                    for (it_inside = world[player_n].inside.begin(); it_inside != world[player_n].inside.end(); ++it_inside)
                    {
                        if (it_inside->player == true)
                        {
                            it_inside->y -= 1;
                            it_inside->y = tor_cord(it_inside->y);
                            player_n = it_inside->cell_number();
                            break;
                        }
                    }
                    keyPressed = true;
                    actions = MAX_SPEED/player_speed;
                }
                else if (key == SDLK_DOWN)
                {
                    for (it_inside = world[player_n].inside.begin(); it_inside != world[player_n].inside.end(); ++it_inside)
                    {
                        if (it_inside->player == true)
                        {
                            it_inside->y += 1;
                            it_inside->y = tor_cord(it_inside->y);
                            player_n = it_inside->cell_number();
                            break;
                        }
                    }
                    keyPressed = true;
                    actions = MAX_SPEED/player_speed;
                }
                else if (key == SDLK_LEFT)
                {
                    for (it_inside = world[player_n].inside.begin(); it_inside != world[player_n].inside.end(); ++it_inside)
                    {
                        if (it_inside->player == true)
                        {
                            it_inside->x -= 1;
                            it_inside->x = tor_cord(it_inside->x);
                            player_n = it_inside->cell_number();
                            break;
                        }
                    }
                    keyPressed = true;
                    actions = MAX_SPEED/player_speed;
                }
                else if (key == SDLK_RIGHT)
                {
                    for (it_inside = world[player_n].inside.begin(); it_inside != world[player_n].inside.end(); ++it_inside)
                    {
                        if (it_inside->player == true)
                        {
                            it_inside->x += 1;
                            it_inside->x = tor_cord(it_inside->x);
                            player_n = it_inside->cell_number();
                            break;
                        }
                    }
                    keyPressed = true;
                    actions = MAX_SPEED/player_speed;
                }
                else if (key == SDLK_EQUALS)
                {
                    TILE_SIZE += 10;
                    if (TILE_SIZE > 50)
                        TILE_SIZE = 50;
                    tiles_in_window = WINDOW_HEIGHT/TILE_SIZE;
                    center_tile = tiles_in_window/2;
                    tile.w = TILE_SIZE;
                    tile.h = TILE_SIZE;
                    small_tile.w = TILE_SIZE/10;
                    small_tile.h = TILE_SIZE/10;
                    long_tile.w = TILE_SIZE;
                    long_tile.h = TILE_SIZE;
                }
                else if (key == SDLK_MINUS)
                {
                    TILE_SIZE -= 10;
                    if (TILE_SIZE < 10)
                        TILE_SIZE = 10;
                    tiles_in_window = WINDOW_HEIGHT/TILE_SIZE;
                    center_tile = tiles_in_window/2;
                    tile.w = TILE_SIZE;
                    tile.h = TILE_SIZE;
                    small_tile.w = TILE_SIZE/10;
                    small_tile.h = TILE_SIZE/10;
                    long_tile.w = TILE_SIZE;
                    long_tile.h = TILE_SIZE;
                }
                else if (key == SDLK_SPACE)
                {
                    paused = !paused;
                }
                else if (key == SDLK_ESCAPE)
                    quit = true;
            }
        }

        if (keyPressed == true)

        {
            keyPressed = false;

            //PHYSICS

            while (actions > 0)

            {
                actions -= 1;

                #pragma omp for
                for (it = world.begin(); it != world.end(); ++it)
                {
                }

                for (it = world.begin(); it != world.end(); ++it)
                {
                    if (!it->inside.empty())
                    {
                        for (it_inside = it->inside.begin(); it_inside != it->inside.end(); ++it_inside)
                        {
                            if (it_inside->mob == 1)
                            {
                                koef = 0;
                                if (it->terra == false)
                                    koef = 0.01;
                                else if (it->terra == true)
                                    koef = 0.1;
                                if (it_inside->aim != -1)
                                {
                                    if (it_inside->x < world[it_inside->aim].get_x())
                                        it_inside->v_x = koef*randomer(rng, 10);
                                    else if (it_inside->x > world[it_inside->aim].get_x())
                                        it_inside->v_x = -koef*randomer(rng, 10);
                                    else if (it_inside->x == world[it_inside->aim].get_x())
                                    {
                                        it_inside->v_x = koef*randomer(rng, 10);
                                        it_inside->v_x -= koef*randomer(rng, 10);
                                    }
                                    if (it_inside->y < world[it_inside->aim].get_y())
                                        it_inside->v_y = koef*randomer(rng, 10);
                                    else if (it_inside->y > world[it_inside->aim].get_y())
                                        it_inside->v_y = -koef*randomer(rng, 10);
                                    else if (it_inside->y == world[it_inside->aim].get_y())
                                    {
                                        it_inside->v_y = koef*randomer(rng, 10);
                                        it_inside->v_y -= koef*randomer(rng, 10);
                                    }
                                    it_inside->x_loc += it_inside->speed*it_inside->v_x;
                                    it_inside->y_loc += it_inside->speed*it_inside->v_y;
                                    it_inside->x = it_inside->x_loc;
                                    it_inside->y = it_inside->y_loc;
                                }
                                if (it_inside->aim != -1 and it_inside->x == world[it_inside->aim].get_x() and it_inside->y == world[it_inside->aim].get_y())
                                {
                                    for (it_obj = world[it_inside->aim].inside.begin(); it_obj != world[it_inside->aim].inside.end(); ++it_obj)
                                                {
                                                    if (it_obj->plant == 1)
                                                    {
                                                        it_inside->aim = -1;
                                                        it_obj->dead = 1;
                                                        it_inside->hp += it_obj->hp;
                                                        break;
                                                    }
                                                }
                                    it_inside->aim = -1;
                                }
                                if (it_inside->aim == -1)
                                {
                                    pos = &world[it->get_n()];
                                    use = 0;
                                    k = 0;
                                    stop = false;
                                    for (int i = 0; i <= 50; i++)
                                    {
                                        for (int a = 0; a < use; a++)   
                                        {   
                                            pos = pos->side_spiral(k%4); 
                                            if (!pos->inside.empty())
                                            {
                                                for (it_obj = pos->inside.begin(); it_obj != pos->inside.end(); ++it_obj)
                                                {
                                                    if (it_obj->plant == 1)
                                                    {
                                                        it_inside->aim = it_obj->cell_number();
                                                        stop = true;
                                                        break;
                                                    }
                                                }
                                            }
                                        }    
                                        if (stop == true)
                                            break;               
                                        k += 1;
                                        for (int b = 0; b < use; b++)   
                                        {
                                            pos = pos->side_spiral(k%4); 
                                            if (!pos->inside.empty())
                                            {
                                                for (it_obj = pos->inside.begin(); it_obj != pos->inside.end(); ++it_obj)
                                                {
                                                    if (it_obj->plant == 1)
                                                    {
                                                        it_inside->aim = it_obj->cell_number();
                                                        stop = true;
                                                        break;
                                                    }
                                                }
                                            }
                                        }
                                        if (stop == true)
                                            break;    
                                        k += 1;
                                        use += 1;
                                    }
                                    it_inside->v_x = koef*randomer(rng, 10);
                                    it_inside->v_x -= koef*randomer(rng, 10);
                                    it_inside->v_y = koef*randomer(rng, 10);
                                    it_inside->v_y -= koef*randomer(rng, 10);
                                    it_inside->x_loc += it_inside->speed*it_inside->v_x;
                                    it_inside->y_loc += it_inside->speed*it_inside->v_y;
                                    it_inside->x = it_inside->x_loc;
                                    it_inside->y = it_inside->y_loc;
                                }
                            }
                            if (it_inside->plant == 1)
                            {
                                it_inside->hp += 1;
                            }
                            if (it_inside->hp > 2000)
                            {
                                perbor = randomer(rng, 7);
                                if (it->side(perbor)->B != 255 and it->side(perbor)->inside.empty() and it_inside->plant == 1)
                                {
                                    it_inside->hp = it_inside->hp-1000;
                                    objects_buff.push_back(object(it->side(perbor)->get_x(), it->side(perbor)->get_y(),1));
                                }
                                else if (it->side(perbor)->B != 255 and it->side(perbor)->inside.empty() and it_inside->mob == 1)
                                {
                                    it_inside->hp = it_inside->hp-1000;
                                    objects_buff.push_back(object(it->side(perbor)->get_x(), it->side(perbor)->get_y(),2));
                                }
                            }
                        }
                    } 
                }
            }
        }

        //DRAW

        pos = &world[player_n];
        k = 0;
        use = 0;
        SDL_SetRenderDrawColor(renderer, pos->R, pos->G, pos->B, 255);
        tile.x = center_tile*TILE_SIZE+SIDE_MENU;
        tile.y = center_tile*TILE_SIZE;
        long_tile.x = center_tile*TILE_SIZE+SIDE_MENU;
        long_tile.y = center_tile*TILE_SIZE;
        if (pos->terra == 1)
            SDL_RenderCopy(renderer, grass_texture, NULL, &tile);
        if (pos->sand == 1)
            SDL_RenderCopy(renderer, sand_texture, NULL, &tile);
        if (pos->water == 1)
            SDL_RenderCopy(renderer, water_texture, NULL, &tile);
        if (!pos->inside.empty() and pos->inside[0].plant == true)
                {
                    SDL_RenderCopy(renderer, plant_texture, NULL, &tile);
                } 
        else if (!pos->inside.empty() and pos->inside[0].mob == true)
                {
                    SDL_RenderCopy(renderer, cow_texture, NULL, &tile);
                } 
        #pragma omp for
        for (int i = 0; i <= tiles_in_window; i++)
        {
            for (int a = 0; a < use; a++)   
            {
                pos = pos->side_spiral(k%4);
                SDL_SetRenderDrawColor(renderer, pos->R, pos->G, pos->B, 255);
                if ((k%4)%2 != 0)
                    tile.x += tile_spiral(k%4)*TILE_SIZE;
                else if ((k%4)%2 == 0)
                    tile.y += tile_spiral(k%4)*TILE_SIZE;
                if (pos->terra == 1)
                    SDL_RenderCopy(renderer, grass_texture, NULL, &tile);
                if (pos->sand == 1)
                    SDL_RenderCopy(renderer, sand_texture, NULL, &tile);
                if (pos->water == 1)
                    SDL_RenderCopy(renderer, water_texture, NULL, &tile);
                if (!pos->inside.empty() and pos->inside[0].plant == true)
                {
                    SDL_RenderCopy(renderer, plant_texture, NULL, &tile);
                } 
                else if (!pos->inside.empty() and pos->inside[0].mob == true)
                {
                    SDL_RenderCopy(renderer, cow_texture, NULL, &tile);
                } 
            }
            k += 1;
            for (int b = 0; b < use; b++)   
            {
                pos = pos->side_spiral(k%4); 
                SDL_SetRenderDrawColor(renderer, pos->R, pos->G, pos->B, 255);
                if ((k%4)%2 != 0)
                    tile.x += tile_spiral(k%4)*TILE_SIZE;
                else if ((k%4)%2 == 0)
                    tile.y += tile_spiral(k%4)*TILE_SIZE;
                if (pos->terra == 1)
                    SDL_RenderCopy(renderer, grass_texture, NULL, &tile);
                if (pos->sand == 1)
                    SDL_RenderCopy(renderer, sand_texture, NULL, &tile);
                if (pos->water == 1)
                    SDL_RenderCopy(renderer, water_texture, NULL, &tile);
                if (!pos->inside.empty() and pos->inside[0].plant == true)
                {
                    SDL_RenderCopy(renderer, plant_texture, NULL, &tile);
                } 
                else if (!pos->inside.empty() and pos->inside[0].mob == true)
                {
                    SDL_RenderCopy(renderer, cow_texture, NULL, &tile);
                } 
            }
            k += 1;
            use += 1;
        }
        SDL_RenderCopy(renderer, girl_texture, NULL, &long_tile);

        //INTERFACE AND INFO

        if (map_mod == true)
        {
            while (map_mod == true)
            {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderClear(renderer);
                if (SDL_PollEvent(&event))
                {
                    if (event.type == SDL_KEYDOWN)
                    {
                        key = event.key.keysym.sym;
                        if (key == SDLK_m)
                        {
                            map_mod = false;
                            break;
                        }
                    }
                }
                for (it = world.begin(); it != world.end(); ++it)
                {
                    SDL_SetRenderDrawColor(renderer, it->R, it->G, it->B, 255);   
                    SDL_RenderDrawPoint(renderer, it->get_x(), it->get_y());
                }
                if (suck_mod == true)
                    SDL_RenderCopy(renderer, info_suck_text, NULL, &info_suck_rect);
                else
                    SDL_RenderCopy(renderer, info_suck_off_text, NULL, &info_suck_rect);
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                small_tile.x = world[player_n].get_x();
                small_tile.y = world[player_n].get_y();
                SDL_RenderFillRect(renderer, &small_tile);

                SDL_RenderPresent(renderer);
            }
        }

        SDL_RenderPresent(renderer);

        //CLEANING AND REFILING BUFFERS

        for (it = world.begin(); it != world.end(); ++it)
        {
            if (!it->inside.empty())
            {
                for (it_inside = it->inside.begin(); it_inside != it->inside.end(); ++it_inside)
                {
                    if (it_inside->dead == false)
                    {
                        it_inside->x = tor_cord(it_inside->x);
                        it_inside->y = tor_cord(it_inside->y);
                        objects_buff.push_back(*it_inside);
                    }
                }
                it->inside.clear();
            }
        }

        for (it_obj = objects_buff.begin(); it_obj != objects_buff.end(); ++it_obj)
        {
            world[it_obj->cell_number()].inside.push_back(*it_obj);
        }

        objects_buff.clear();

        SDL_Delay(time_left());
        next_time += TICK_INTERVAL;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}