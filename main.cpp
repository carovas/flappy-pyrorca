#include <raylib.h>
#include <random>
#include <map>
#include <stdio.h>

Color background_color = Color{20, 160, 133, 255};
const float screen_width = 800.f;
const float screen_height = 600.f;
const float jump_height = 250.f;

const float cs = 50.f;
const float max_angle = 35.f;

float obstacle_speed = 125.f;

class SoundHandler {
public:
    SoundHandler() {
        sounds[0] = LoadSound("./Sounds/Braguette.wav");
        sounds[1] = LoadSound("./Sounds/jsp.wav");
        sounds[2] = LoadSound("./Sounds/PRENEUR.wav");
        sounds[3] = LoadSound("./Sounds/youhou.wav");
    }

    void play_random_sound() {
        bool any_sound_playing = false;
        for(int i = 0; i < 4; i++)
        {
            if(IsSoundPlaying(sounds[i]))
                any_sound_playing = true;
        }
        if(!any_sound_playing)
            PlaySound(sounds[rand()%4]);
    }

private:
    Sound sounds[4];
};

class Obstacle {
public:
    Obstacle(float poz) : height((rand()% (int)(screen_height - cs * 3)) +cs * 3), pos(poz), texture(LoadTexture("./Graphics/maxala.png")),
    off_screen(false), p_lost(false)
    {
    }

    ~Obstacle() {
        UnloadTexture(texture);
    }

    bool is_off_screen() {
        return off_screen;
    }

    bool is_p_lost() {
        return p_lost;
    }

    void update(float dt, float p_pos) {
        Rectangle topRec{pos, 0, cs, height - cs * 2.f};
        Rectangle botRec{pos, height + cs * 2.f, cs, height};
        float ccs = 15.f;
        DrawTexturePro(texture, srcRect, topRec, {0.f, 0.f}, 0.f, WHITE);// top rect
        DrawTexturePro(texture, srcRect, botRec, {0.f, 0.f}, 0.f, WHITE);// bot rect
        pos -= obstacle_speed * dt;

        if(CheckCollisionCircleRec({150.f, p_pos}, ccs, topRec) || CheckCollisionCircleRec({150.f, p_pos}, ccs, botRec) 
        || p_pos < 0.f - cs || p_pos > screen_height + cs
        )
            p_lost = true;

        if(pos <= 0.f - cs)
            off_screen = true;
    }

private:

    Rectangle srcRect{0.f, 0.f, 200.f, 305.f};
    float height;
    float pos; // pos on the x axis
    Texture2D texture;
    bool off_screen;
    bool p_lost;
};

class Obstacle_Handler {
public:
    Obstacle_Handler* add(int pos) {
        p_llost = false;
        obstacles[pos] = new Obstacle(pos);
        return this;
    }

    void set_p_pos(float* pp)  {
        p_pos = pp;
    }

    void update(float dt) {
        for(auto it = obstacles.begin(); it != obstacles.end(); it++)
        {
            if(it->second->is_off_screen()) {
                delete it->second;
                it->second = new Obstacle(it->first);
            } else {
                it->second->update(dt, *p_pos);
                if(it->second->is_p_lost()) {
                   p_llost = true;
                }
            }
        }
    }

    bool is_p_llost() {return p_llost;}

    void clear() {
        p_llost = false;
        obstacles.clear();
    }

private:
    std::map<int, Obstacle*> obstacles;
    float* p_pos;
    bool p_llost;
};

class Pyrorca {
public:
    Pyrorca(): force(0.f), height(0.f), texture(LoadTexture("./Graphics/pyrorca.png")) 
    {
    }

    ~Pyrorca() {
        UnloadTexture(texture);
    }

    void set_h(float h) {
        height = h;
    }

    // draws and update y pos
    void update(float dt) {
        
        DrawTexturePro(texture, {0.f, 0.f, 200.f, 200.f}, {150.f, height, cs, cs},
         {cs/6.f,cs/2.f}, handle_rotation(), WHITE); // 200.f is original size
        
        //DrawTextureEx(texture, {100.f, height}, handle_rotation(), cs / 200.f, WHITE); // 200.f is original size

        if(force > -1.f && !IsKeyDown(KEY_SPACE))
            force = force - 0.1f;

        height = height - jump_height * force * dt;
    }

    // TODO apply a formula
    void jump(float dt) {
        dt = dt * 1;
        force = 1.5f;
    }

    /*
    force is a value from 1.5 to -1 (2.5 to 0)
    handle rotation map this value to an angle between -75 to 75
    map the value from 1.5f and -1 to 1 and 0
    then the value is mapped from 0 to 150
    then to 75 to -75 and finnally inversed
    */
    float handle_rotation() {
        return (((force + 1.f) / 2.5f) * max_angle * 2 - max_angle) * -1.f; 
    }

    float* get_pos_ptr() {
        return &height;
    }

private:

    float force;
    float height;
    Texture2D texture;
};

int main() {
    
    InitWindow(screen_width, screen_height, "Flappy Pyrorca");
    InitAudioDevice();
    SetTargetFPS(60);  

    Pyrorca pyrorca;
    Obstacle_Handler handler;
    SoundHandler s_handler;
    handler.add(screen_width)->add(screen_width + screen_width / 3)->add(screen_width + (screen_width / 3) * 2);
    handler.set_p_pos(pyrorca.get_pos_ptr());

    while (!WindowShouldClose())
    {
        float delta_time = GetFrameTime();

        if(IsKeyDown(KEY_SPACE))
        {
            pyrorca.jump(delta_time);
            s_handler.play_random_sound();
            
        }

        if(IsKeyDown(KEY_R)){

            handler.clear();
            handler.add(screen_width)->add(screen_width + screen_width / 3)->add(screen_width + (screen_width / 3) * 2);
        }

        BeginDrawing();
        if(!handler.is_p_llost()) {
            ClearBackground(background_color);

            pyrorca.update(delta_time);
            handler.update(delta_time);
        }

        //void DrawLineEx(Vector2 startPos, Vector2 endPos, float thick, Color color); 
        DrawLineEx({0, 0}, {screen_width, 0}, 7, PURPLE);
        DrawLineEx({0, screen_height}, {screen_width, screen_height}, 7, PURPLE);
        DrawLineEx({0, 0}, {0, screen_height}, 7, PURPLE);
        DrawLineEx({screen_width, 0}, {screen_width, screen_height}, 7, PURPLE);

        EndDrawing();
    }
    
    CloseAudioDevice(); 
    CloseWindow();
}