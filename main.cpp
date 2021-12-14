#include "raylib.h"
#include <random>
#include <tuple>
#include <vector>

using std::vector;
using std::tie;

struct Dot {
  int x;
  int y;
};

struct Direction {
  int x;
  int y;
};

bool operator!=(const Direction &p_lhs, const Direction &p_rhs) {
  return tie(p_lhs.x, p_lhs.y) != tie(p_rhs.x, p_rhs.y);
}
bool operator==(const Direction &p_lhs, const Direction &p_rhs) {
  return tie(p_lhs.x, p_lhs.y) == tie(p_rhs.x, p_rhs.y);
}

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
  const int dotSize = 10;
  const int screenWidth = 50;
  const int screenHeight = 50;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> WRNG(0, screenWidth);
  std::uniform_int_distribution<int> HRNG(0, screenHeight);

  Dot food{WRNG(gen), HRNG(gen)};
  auto score = 0;
  auto snakeDirection = East;
  auto snake = vector<struct Dot>(5, {0, 0});
  auto speed = 10;

  InitWindow((screenWidth+1)*dotSize, (screenHeight+1)*dotSize, "SNAKE");

  SetTargetFPS(speed);

  while (!WindowShouldClose()) {

    auto &head = snake.at(0);
    Dot newHead = { head.x + snakeDirection.x, head.y + snakeDirection.y };

    // Controls
    if (IsKeyPressed(KEY_DOWN) && snakeDirection != North) snakeDirection = South;
    else if (IsKeyPressed(KEY_UP) && snakeDirection != South) snakeDirection = North;
    else if (IsKeyPressed(KEY_RIGHT) && snakeDirection != West) snakeDirection = East;
    else if (IsKeyPressed(KEY_LEFT) && snakeDirection != East) snakeDirection = West;

    // Collisions
    if(newHead.x == -1  || newHead.x == screenWidth+1  || newHead.y == -1  || newHead.y == screenHeight+1)
      state = GameOver;

    for(auto& d:snake)
      if(newHead.x == d.x && newHead.y == d.y)
        state = GameOver;

    if (food.x == head.x + snakeDirection.x &&
        food.y == head.y + snakeDirection.y) {
      snake.push_back({});
      food.x = WRNG(gen);
      food.y = HRNG(gen);
      score += 100;
      speed++;
      SetTargetFPS(speed);
    }

    // Update
    if(state == Running) {
      snake.insert(snake.begin(), newHead);
      snake.pop_back();
    }

    // Draw
    BeginDrawing();
    {
      ClearBackground(RAYWHITE);
      for (auto &d : snake) DrawRectangle(d.x * dotSize, d.y * dotSize, dotSize, dotSize, GOLD);
      DrawRectangle(food.x * dotSize, food.y * dotSize, dotSize, dotSize, RED);
      DrawText(("SCORE: " + std::to_string(score)).c_str(), 10, 10, 20, BLACK);

      if (state == GameOver)
        DrawText("GAME OVER", GetScreenWidth() / 2 - 80, GetScreenHeight() / 2, 20, BLACK);
    }
    EndDrawing();
  }

    CloseWindow();
    return 0;
}
