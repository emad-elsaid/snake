#include "raylib.h"
#include "raymath.h"
#include <random>
#include <vector>

bool operator!=(const Vector2 &l, const Vector2 &r) { return l.x != r.x || l.y != r.y; };

class SpinningInt {
  int v, max;
public:
  SpinningInt(int max) : v {0}, max {max}{};
  operator int() const { return v; };
  SpinningInt& operator++(int) {
    v = (v+1) % max;
    return *this;
  };
};

struct Snake {
  std::vector<Vector2> dots;
  Vector2 direction;
};

struct Sprite {
  Texture texture;
  SpinningInt frame;
  float frameWidth;
  Rectangle operator()(){ return Rectangle{frame*frameWidth, 0, frameWidth, float(texture.height)}; }
};

enum State {
  Running,
  GameOver
};

template <int min, int max>
float ConstrainedRNG() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<int> rng(min, max);
  return static_cast<float>(rng(gen));
}

int main(void) {
  const auto dotSize = 16;
  const auto screenWidth = 50;
  const auto screenHeight = 50;
  const auto WRNG = ConstrainedRNG<0, screenWidth>;
  const auto HRNG = ConstrainedRNG<0, screenHeight>;
  const Vector2
    North = {0, -1},
    South = {0, 1},
    East = {1, 0},
    West = {-1, 0};
  auto state = Running;
  auto food = Vector2{WRNG(), HRNG()};
  auto score = 0;
  auto speed = 10;
  auto snake = Snake{std::vector<Vector2>(5, {0, 0}), East};

  InitWindow((screenWidth+1)*dotSize, (screenHeight+1)*dotSize, "SNAKE");

  Sprite foodSprite = {
    .texture = LoadTexture("assets/food.png"),
    .frame = {5},
    .frameWidth = 16
  };

  SetTargetFPS(speed);

  while (!WindowShouldClose()) {
    // Controls
    if (IsKeyPressed(KEY_DOWN) && snake.direction != North) snake.direction = South;
    else if (IsKeyPressed(KEY_UP) && snake.direction != South) snake.direction = North;
    else if (IsKeyPressed(KEY_RIGHT) && snake.direction != West) snake.direction = East;
    else if (IsKeyPressed(KEY_LEFT) && snake.direction != East) snake.direction = West;

    auto head = snake.dots.at(0);
    auto newHead =  Vector2{ head.x + snake.direction.x, head.y + snake.direction.y };

    // Collisions
    if(newHead.x == -1  || newHead.x == screenWidth+1  || newHead.y == -1  || newHead.y == screenHeight+1)
      state = GameOver;

    for(auto& d:snake.dots)
      if(newHead.x == d.x && newHead.y == d.y)
        state = GameOver;

    if (food.x == head.x + snake.direction.x &&
        food.y == head.y + snake.direction.y) {
      snake.dots.push_back({});
      food = { WRNG(), HRNG() };
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
      foodSprite.frame++;
      DrawTextureRec(foodSprite.texture, foodSprite(), {float(food.x*dotSize), float(food.y*dotSize)}, WHITE);
      DrawText(("SCORE: " + std::to_string(score)).c_str(), dotSize, dotSize, 20, BLACK);
      if (state == GameOver)
        DrawText("GAME OVER", GetScreenWidth() / 2 - 80, GetScreenHeight() / 2, 20, BLACK);
    }
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
