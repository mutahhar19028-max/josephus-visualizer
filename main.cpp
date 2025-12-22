//#include <windows.h>
//#undef DrawText
//#undef LoadImage
//#undef PlaySound

#include "raylib.h"
#include <cmath>
#include <vector>
#include <string>
#include <iostream>

using std::cout;
using std::endl;
using std::cin;

#include <iostream>
using namespace std;

template<class dataType>
class CircularQueue {
private:
    class Node;
    using nptr = Node*;

    class Node {
    public:
        dataType _m_data;
        nptr _m_next;

        Node() : _m_data(), _m_next(nullptr) {}
        Node(dataType dt) : _m_data(dt), _m_next(nullptr) {}
    };

    int _m_size;
    nptr _m_front;
    nptr _m_rear;

public:
    CircularQueue() : _m_size(0), _m_front(nullptr), _m_rear(nullptr) {}

    bool isEmpty() const { return !_m_size; }

    // Add an element at the rear
    void enqueue(dataType val) {
        nptr temp = new Node(val);

        if(!_m_front) {
            _m_front = _m_rear = temp;
            _m_rear->_m_next = _m_front;
            _m_size++;
            return;
        }

        _m_rear->_m_next = temp;
        _m_rear = temp;
        _m_rear->_m_next = _m_front;
        _m_size++;
    }

    // Remove the element at the front
    void dequeue() {
        if(isEmpty()) {
            cerr << "Queue underflow!\n";
            return;
        }

        if(_m_size == 1) {
            delete _m_front;
            _m_front = _m_rear = nullptr;
            _m_size = 0;
            return;
        }

        nptr temp = _m_front;
        _m_front = _m_front->_m_next;
        _m_rear->_m_next = _m_front;
        delete temp;
        _m_size--;
    }

    // Rotate queue (Josephus step)
    void next() {
        if(!isEmpty()) {
            _m_front = _m_front->_m_next;
            _m_rear = _m_rear->_m_next;
        }
    }

    int getSize() const { return _m_size; }

    dataType front() const {
        if(isEmpty()) {
            cerr << "Queue underflow!\n";
            return dataType();
        }
        return _m_front->_m_data;
    }

    dataType rear() const {
        if(isEmpty()) {
            cerr << "Queue underflow!\n";
            return dataType();
        }
        return _m_rear->_m_data;
    }

    void display() const {
        cout << endl;
        if(isEmpty()) return;

        nptr cur = _m_front;
        do {
            cout << cur->_m_data << " ";
            cur = cur->_m_next;
        } while(cur != _m_front);
        cout << endl;
    }

    ~CircularQueue() {
        // clean up all remaining nodes
        while(!isEmpty())
            dequeue();
    }
};

int calcAns(int n, int k) {
    if (n < 1) return -1;

    int res = 0;                // 0-indexed
    for (int i = 2; i <= n; ++i) {
        int km = k % i;
        res = (res + km) % i;
    }
    return res;
}

int main() {

    SetTraceLogLevel(LOG_NONE);

    //HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Purple text (Red + Blue) + Intensity for bright effect
    // Blue background with intensity for brighter tone
    //SetConsoleTextAttribute(hConsole, 
    //    FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY |
    //    BACKGROUND_BLUE | BACKGROUND_INTENSITY);

    //system("color 1D"); // Blue background, purple text

    int num,k;
    cout << "Enter n : ";
    cin >> num;
    cout << "Enter k : ";
    cin >> k;

    if(k < 1)  {
        cerr << "k should must be >= 1";
        return 0;
    }

    cout << calcAns(num, k) + 1 << " should be the survivor!\n";
    k--;

    // Reset to default (Gray on Black)
    //SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

    if(num <= 0) return 0;

    CircularQueue<int> q;
    for(int i = 0; i < num; ++i) q.enqueue(i + 1);

    const int screenWidth = 1920;
    const int screenHeight = 1080;
    InitWindow(screenWidth, screenHeight, "Josephus Problem");
    InitAudioDevice();

    Font font = LoadFontEx("assets/font/vister-regular.otf", 48, nullptr, 0);

    // Show quick loading text
    BeginDrawing();
    ClearBackground(BLACK);
    DrawTextEx(font, "Loading background...", {200, 200}, (float)font.baseSize,2.0f, WHITE);
    EndDrawing();

    // Load GIF background
    int frameCount = 0;
    Image gifAnim = LoadImageAnim("assets/visuals/background.gif", &frameCount);
    Texture2D background = LoadTextureFromImage(gifAnim);
    Color* frameData = (Color*)gifAnim.data;
    int currentFrame = 0;
    float gifFrameTime = 0.08f; 
    float gifAccumulator = 0.0f;

    Texture2D sprite = LoadTexture("assets/visuals/sprite.png");
    Texture2D deathImage = LoadTexture("assets/visuals/deathImage.png");
    Sound deathSound = LoadSound("assets/audio/deathSound.wav");

    // Background music stream
    Music bgm = LoadMusicStream("assets/audio/bgm.mp3");
    PlayMusicStream(bgm);
    SetMusicVolume(bgm, 0.65f);

    const float baseRadius = 350.0f;
    float radius = baseRadius;
    const float circlePosX = 400.0f;
    const float circlePosY = 135.0f;
    const float centerX = circlePosX + baseRadius;
    const float centerY = circlePosY + baseRadius;

    std::vector<Vector2> charPositions(num);
    std::vector<Vector2> textPositions(num);
    for(int i = 0; i < num; ++i) {
        float angle = 2.0f * PI * float(i) / float(num) - PI/2.0f;
        float x = centerX + cosf(angle) * baseRadius;
        float y = centerY + sinf(angle) * baseRadius;
        charPositions[i] = { x - sprite.width/2.0f, y - sprite.height/2.0f };
        textPositions[i] = { x - 10.0f, y + 40.0f };
    }

    std::vector<bool> isDead(num, false);
    std::vector<float> deathAlpha(num, 0.0f);

    SetTargetFPS(60);

    bool flag = false;
    bool autoplay = false;
    bool paused = false;
    float autoplayTimer = 0.0f;
    float autoplayInterval = 1.0f;
    int rounds = 0;
    int deathsCount = 0;
    float totalTime = 0.0f;

    // Survivor animation state
    bool gameOver = false;
    int survivorIndex = -1;
    float survivorAnimT = 0.0f;
    Vector2 survivorStart;
    Vector2 survivorEnd = { (float)screenWidth/2 - sprite.width/2, (float)screenHeight/2 - sprite.height/2 };

    while(!WindowShouldClose()) {
        float dt = GetFrameTime();
        if(!paused) UpdateMusicStream(bgm);

        // Controls
        if(!gameOver && IsKeyPressed(KEY_SPACE)) autoplay = !autoplay;
        if(IsKeyPressed(KEY_R)) {
            q = CircularQueue<int>();
            for(int i = 0; i < num; ++i) q.enqueue(i + 1);
            for(int i = 0; i < num; ++i) { isDead[i] = false; deathAlpha[i] = 0.0f; }
            rounds = 0; deathsCount = 0; autoplayTimer = 0.0f;
            flag = false; autoplay = false; paused = false;
            gameOver = false; survivorAnimT = 0.0f; survivorIndex = -1;
            cout << "Reset done." << endl;
        }
        if(!gameOver && IsKeyPressed(KEY_ESCAPE)) paused = !paused;

        if(!paused && !gameOver && IsKeyPressed(KEY_ENTER) && q.getSize() != 1) flag = true;
        if(!paused && !gameOver && autoplay && q.getSize() != 1) {
            autoplayTimer += dt;
            if(autoplayTimer >= autoplayInterval) { autoplayTimer = 0.0f; flag = true; }
        }

        // Elimination step
        if(!paused && !gameOver && q.getSize() != 1 && flag) {
            PlaySound(deathSound);
            for(int i = 0; i < k; ++i) q.next();
            int x = q.front();
            q.dequeue();

            rounds++;
            deathsCount++;

            if(q.getSize() == 1) {
                cout << q.front() << " is the survivor " << endl;
                gameOver = true;
                survivorIndex = q.front() - 1;
                survivorStart = charPositions[survivorIndex];
            }
            if(x >= 1 && x <= num) { isDead[x - 1] = true; deathAlpha[x - 1] = 0.0f; }
            flag = false;
        }

        // Animate GIF
        if(!paused) {
            gifAccumulator += dt;
            if(gifAccumulator >= gifFrameTime) {
                gifAccumulator -= gifFrameTime;
                currentFrame++;
                if(currentFrame >= frameCount) currentFrame = 0;
                UpdateTexture(background, frameData + (currentFrame * gifAnim.width * gifAnim.height));
            }
        }

        if(!paused) totalTime += dt;
        radius = baseRadius * (1.0f + 0.03f * sinf(totalTime * 2.5f));
        for(int i = 0; i < num; ++i)
            if(isDead[i]) { deathAlpha[i] += dt * 2.0f; if(deathAlpha[i] > 1.0f) deathAlpha[i] = 1.0f; }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            // Background
            DrawTexturePro(background,
                           Rectangle{0.0f,0.0f,(float)background.width,(float)background.height},
                           Rectangle{0.0f,0.0f,(float)screenWidth,(float)screenHeight},
                           Vector2{0,0},0.0f,WHITE);

            // Dim overlay
            DrawRectangle(0,0,screenWidth,screenHeight, Color{0,0,0,120});

            if(gameOver) {
                // Survivor animation timing
                if(survivorAnimT < 1.0f) survivorAnimT += dt * 0.7f;

                // Survivor position on screen center
                Vector2 pos;
                pos.x = screenWidth/2.0f - sprite.width/2.0f;
                pos.y = screenHeight/2.0f - sprite.height/2.0f;

                // Bounce scale effect for survivor :smile
                float scale = 1.5f + 0.25f * sinf(totalTime * 6.0f) * (1.0f - survivorAnimT); // CHANGED: base scale 1.5x

                // Shadow/glow layers
                for(int g = 0; g < 4; ++g) {
                    float s = scale + 0.12f * (4 - g);
                    DrawTexturePro(sprite,
                                Rectangle{0,0,(float)sprite.width,(float)sprite.height},
                                Rectangle{
                                    screenWidth/2.0f - (sprite.width * s)/2.0f,
                                    screenHeight/2.0f - (sprite.height * s)/2.0f,
                                    sprite.width * s, sprite.height * s
                                },
                                Vector2{0,0},0.0f, Fade(RED, 0.12f));
                }

                // Main survivor sprite for giving main character vibes :smile
                DrawTexturePro(sprite,
                            Rectangle{0,0,(float)sprite.width,(float)sprite.height},
                            Rectangle{
                                screenWidth/2.0f - (sprite.width * scale)/2.0f,
                                screenHeight/2.0f - (sprite.height * scale)/2.0f,
                                sprite.width * scale, sprite.height * scale
                            },
                            Vector2{0,0},0.0f, WHITE);

                // Survivor number centered below sprite
                std::string sNum = "Survivor: " + std::to_string(survivorIndex + 1);
                int textW = MeasureTextEx(font, sNum.c_str(), 52, 2).x;
                Color glow = (GetTime() - (int)GetTime() < 0.5f) ? LIME : YELLOW;
                DrawTextEx(font,
                        sNum.c_str(),
                        Vector2{ (float)(screenWidth/2 - textW/2),
                                    (float)(screenHeight/2 + (sprite.height * scale)/2 + 20) }, // 20px below sprite
                        52, 2, glow);
            }
            else {
                // Spotlight circle
                float spotlightRadius = radius * 1.15f;
                for(int g = 0; g < 6; ++g) {
                    float rr = spotlightRadius * (1.0f + 0.06f * g);
                    DrawCircleGradient((int)centerX,(int)centerY,rr,Fade(BLACK,0.0f),Fade(PURPLE,0.06f*(6-g)));
                }
                for(int i = 0; i < 6; ++i) {
                    float wobble = 0.6f * sinf(totalTime*3.0f + i);
                    DrawCircleLines((int)centerX,(int)centerY,radius+i*1.5f+wobble,Color{200,230,255,(unsigned char)(200-i*20)});
                }
                DrawCircle((int)centerX,(int)centerY,radius*0.6f,Fade(SKYBLUE,0.02f));

                // Characters
                for(int i = 0; i < num; ++i) {
                    Vector2 pos = charPositions[i];
                    if(isDead[i]) {
                        DrawTextureEx(sprite,pos,0.0f,1.0f,Fade(WHITE,0.25f));
                        DrawTextureEx(deathImage,pos,0.0f,1.0f,Fade(WHITE,deathAlpha[i]));
                    } else {
                        DrawTextureEx(sprite,pos,0.0f,1.0f,WHITE);
                    }
                    std::string label = std::to_string(i+1);
                    Color labelColor = isDead[i]?Fade(GRAY,0.9f):SKYBLUE;
                    DrawTextEx(font,label.c_str(),textPositions[i],(float)font.baseSize,2.0f,labelColor);
                }

                // UI
                int remaining = num - deathsCount;
                int eliminated = deathsCount;
                std::string roundsStr = "Round: " + std::to_string(rounds);
                std::string remainingStr = "Remaining: " + std::to_string(remaining);
                std::string eliminatedStr = "Eliminated: " + std::to_string(eliminated);
                std::string statusStr;
                if(paused) statusStr = "Status: PAUSED (ESC)";
                else if(autoplay) statusStr = "Status: AUTOPLAY (SPACE)";
                else statusStr = "Status: MANUAL (ENTER)";

                int panelW = 380;
                int panelH = 140;
                int panelX = screenWidth - panelW - 20;
                int panelY = 20;
                DrawRectangle(panelX,panelY,panelW,panelH,Fade(BLACK,0.55f));
                DrawRectangleLines(panelX,panelY,panelW,panelH,Color{120,180,255,180});

                auto DrawShadowText=[&](const std::string &txt,int x,int y,Color col){
                    DrawTextEx(font,txt.c_str(),Vector2{(float)x+2,(float)y+2},28.0f,1.5f,BLACK);
                    DrawTextEx(font,txt.c_str(),Vector2{(float)x,(float)y},28.0f,1.5f,col);
                };
                DrawShadowText(roundsStr,panelX+16,panelY+18,Color{180,220,255,255});
                DrawShadowText(remainingStr,panelX+16,panelY+46,Color{120,255,180,255});
                DrawShadowText(eliminatedStr,panelX+16,panelY+74,Color{255,200,120,255});
                DrawShadowText(statusStr,panelX+16,panelY+102,Color{200,160,255,255});

                std::string mode = autoplay ? "AUTOPLAY: ON (SPACE toggles)" : "AUTOPLAY: OFF (SPACE toggles)";
                if(paused) mode = "PAUSED (ESC to resume)";
                DrawText(mode.c_str(),20,screenHeight-40,18,WHITE);
            }
        EndDrawing();
    }

    // Cleanup
    UnloadTexture(background);
    UnloadImage(gifAnim);
    UnloadTexture(sprite);
    UnloadTexture(deathImage);
    UnloadSound(deathSound);
    UnloadMusicStream(bgm);
    UnloadFont(font);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}