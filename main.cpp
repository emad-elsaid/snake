#include "raylib.h"
#include <random>
#include <vector>

using std::vector;

struct Dot {
  int x;
  int y;
};

struct Direction {
  int x;
  int y;
};

bool operator!=(const Direction &l, const Direction &r) {
  return l.x != r.x || l.y != r.y;
}
bool operator==(const Direction &l, const Direction &r) {
  return l.x == r.x && l.y == r.y;
}

struct Snake {
  vector<Dot> dots;
  Direction direction;
};

enum State {
  Running,
  GameOver
};

int main(void) {
  Direction North = {0, -1};
  Direction South = {0, 1};
  Direction East = {1, 0};
  Direction West = {-1, 0};

  State state = Running;
  const int dotSize = 16;
  const int screenWidth = 50;
  const int screenHeight = 50;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> WRNG(0, screenWidth);
  std::uniform_int_distribution<int> HRNG(0, screenHeight);

  Dot food{WRNG(gen), HRNG(gen)};
  auto score = 0;
  auto speed = 10;
  Snake snake = {vector<Dot>(5, {0, 0}), East};

  InitWindow((screenWidth+1)*dotSize, (screenHeight+1)*dotSize, "SNAKE");

  auto foodTexture = LoadTexture("assets/food.png");
  int foodFrames = 5;
  int foodCurrentFrame = 0;
  float foodFrameWidth = static_cast<float>(float(foodTexture.width) / foodFrames);

  SetTargetFPS(speed);

  while (!WindowShouldClose()) {
    // Controls
    if (IsKeyPressed(KEY_DOWN) && snake.direction != North) snake.direction = South;
    else if (IsKeyPressed(KEY_UP) && snake.direction != South) snake.direction = North;
    else if (IsKeyPressed(KEY_RIGHT) && snake.direction != West) snake.direction = East;
    else if (IsKeyPressed(KEY_LEFT) && snake.direction != East) snake.direction = West;

    auto &head = snake.dots.at(0);
    Dot newHead = { head.x + snake.direction.x, head.y + snake.direction.y };

    // Collisions
    if(newHead.x == -1  || newHead.x == screenWidth+1  || newHead.y == -1  || newHead.y == screenHeight+1)
      state = GameOver;

    for(auto& d:snake.dots)
      if(newHead.x == d.x && newHead.y == d.y)
        state = GameOver;

    if (food.x == head.x + snake.direction.x &&
        food.y == head.y + snake.direction.y) {
      snake.dots.push_back({});
      food.x = WRNG(gen);
      food.y = HRNG(gen);
      score += 100;
      speed++;
      SetTargetFPS(speed);
    }

    // Update
    if(state == Running) {
      snake.dots.insert(snake.dots.begin(), newHead);
      snake.dots.pop_back();
    }

    // Draw
    BeginDrawing();
    {
      ClearBackground(RAYWHITE);
      for (auto &d : snake.dots) DrawRectangle(d.x * dotSize, d.y * dotSize, dotSize, dotSize, GOLD);

      foodCurrentFrame = (foodCurrentFrame+1) % foodFrames;
      Rectangle foodFrame = {
          foodCurrentFrame*foodFrameWidth,
          0,
          foodFrameWidth,
          float(foodTexture.height)
      };

      DrawTextureRec(foodTexture, foodFrame, {float(food.x*dotSize), float(food.y*dotSize)}, WHITE);
      DrawText(("SCORE: " + std::to_string(score)).c_str(), dotSize, dotSize, 20, BLACK);

      if (state == GameOver)
        DrawText("GAME OVER", GetScreenWidth() / 2 - 80, GetScreenHeight() / 2, 20, BLACK);
    }
    EndDrawing();
  }

    CloseWindow();
    return 0;
}
