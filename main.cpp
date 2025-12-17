#include "raylib.h"
#include <random>
#include <cmath>

using namespace std;

int left_score = 0;
int right_score = 0;

class Ball{
public:
    float x,y;
    float speed_x, speed_y;
    float radius;
    float reset_timer;

    Ball(float x, float y, float speed_x, float speed_y, float radius)
        : x(x),
          y(y),
          speed_x(speed_x),
          speed_y(speed_y),
          radius(radius) {
        this->radius = 20;
        this->x = x;
        this->y = y;
        this->speed_x = speed_x;
        this->speed_y = speed_y;
        reset_timer = 0.0f;
    }

    void Draw() {
        DrawCircle(x, y, radius, WHITE);
    }

    void Update() {
        if (reset_timer > 0.0f) {
            reset_timer -= GetFrameTime();
            return;
        }
        x += speed_x;
        y += speed_y;

        if (0 >= y - radius or y + radius >= GetScreenHeight()) speed_y = -speed_y;
        if (x < -radius) {
            right_score++;
            ResetBall();
        }
        else if (x > GetScreenWidth() + radius) {
            left_score++;
            ResetBall();
        }
    }

    void ResetBall() {
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;

        int speed_directions[2] = {-1, 1};
        speed_x = 8 * speed_directions[GetRandomValue(0,1)];
        speed_y = GetRandomValue(0,8) * speed_directions[GetRandomValue(0,1)];

        reset_timer = 1.0f;
    }
};

class Paddle {
    protected:
    void LimitMovement() {
        if (y <= 0) y = 0;
        if (y + height >= GetScreenHeight()) y = GetScreenHeight() - height;
    }

    public:
    float x,y;
    float width,height;
    int speed;

    Paddle(float x, float y, float width, float height, int speed)
        : x(x),
          y(y),
          width(width),
          height(height),
          speed(speed) {
        this->width = width;
        this->height = height;
        this->speed = speed;
        this->x = x;
        this->y = y;
    }

    void Draw() {
        DrawRectangleRounded(Rectangle{x,y,width,height},0.8,0,WHITE);
    }

    void Update() {
        if (IsKeyDown(KEY_UP)) {
            y -= speed;
        }
        if (IsKeyDown(KEY_DOWN)) {
            y += speed;
        }
        LimitMovement();
    }
};

class PcPaddle : public Paddle {
    public:
    PcPaddle(float x, float y, float width, float height, int speed)
        : Paddle(x, y, width, height, speed) {}
    void Update(Ball ball) {
        if (y + height/2 > ball.y) y -= speed;
        if (y + height/2 < ball.y) y += speed;
        LimitMovement();
    }
};

void collision(Ball &ball, Paddle paddle) {
    if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{paddle.x, paddle.y, paddle.width, paddle.height})) {
        if (ball.speed_x > 0) {
            ball.speed_x = 10.0;
            if (ball.x > paddle.x + paddle.width/4) return;
        }

        else if (ball.speed_x < 0) {
            ball.speed_x = -10.0;
            if (ball.x < paddle.x + 3 * paddle.width/4) return;
        }

        ball.speed_x *= -1;

        float centerPaddleY = paddle.y + paddle.height / 2.0f;
        float distance = ball.y - centerPaddleY;
        float normalizedDiff = distance / (paddle.height / 2.0f);

        float curve = pow(normalizedDiff, 3.0f);
        ball.speed_y = (curve * 10.0f);
        ball.speed_y += normalizedDiff * 2;

        if (ball.speed_x > 0) ball.x = paddle.x + paddle.width + ball.radius + 1.0f;
        else ball.x = paddle.x - ball.radius - 1.0f;
    }
}

int main() {
    const int screenWidth = 1920;
    const int screenHeight = 1020;
    int speed_directions[2] = {-1, 1};
    int random = GetRandomValue(0,1);
    Ball ball(screenWidth/2,screenHeight/2,8*speed_directions[random],GetRandomValue(0,8)*speed_directions[random],20);
    Paddle paddle(10,screenHeight/2 - 60,25,120,8);
    PcPaddle paddle2(screenWidth - 35,screenHeight/2 - 60,25,120,8);
    InitWindow(screenWidth, screenHeight, "Pong");
    SetTargetFPS(120);
    while (!WindowShouldClose()) {
        BeginDrawing();

        paddle.Update();
        paddle2.Update(ball);
        ball.Update();

        collision(ball, paddle);
        collision(ball, paddle2);

        ClearBackground(BLACK);
        DrawLine(screenWidth/2,0,screenWidth/2,screenHeight,WHITE);
        ball.Draw();
        paddle.Draw();
        paddle2.Draw();
        DrawText(TextFormat("%i",left_score),screenWidth / 4 - 20, 20, 80, WHITE);
        DrawText(TextFormat("%i",right_score),3*screenWidth / 4 - 20, 20, 80, WHITE);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}