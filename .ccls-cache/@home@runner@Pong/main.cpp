#include <chrono>
#include <cstdio>  // For printf
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()
#include <iostream>
#include <thread>

using namespace std;

// Game dimensions
const int width = 40;
const int height = 20;

// Player positions and score
int paddleSize = 5;
int paddle1Y = height / 2 - 3;
int paddle2Y = height / 2 - 3;
int ballX = width / 2;
int ballY = height / 2;
int score1 = 0, score2 = 0;

// Ball direction
int ballDirX = -1, ballDirY = -1;

void Draw() {
  // cout << "\x1B[?25l\x1B[2J\x1B[H"; // Clear the screen
  cout << "\r\x1B[?25l\x1B[H"; // hides cursor and moves it back to home.
  for (int i = 0; i < width + 2; i++)
    cout << "#";
  cout << endl;

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      if (j == 0)
        cout << "#";
      if (i == ballY && j == ballX)
        cout << "O"; // Ball
      else if (i >= paddle1Y && i <= paddle1Y + paddleSize-1 && j == 1)
        cout << "|"; // Left Paddle
      else if (i >= paddle2Y && i <= paddle2Y + paddleSize-1 && j == width - 2)
        cout << "|"; // Right Paddle
      else
        cout << " ";
      if (j == width - 1)
        cout << "#";
    }
    cout << endl;
  }

  for (int i = 0; i < width + 2; i++)
    cout << "#";
  cout << endl;

  cout << "Player 1: " << score1 << "    Player 2: " << score2 << endl;
}

void Logic() {
  ballX += ballDirX;
  ballY += ballDirY;
  int randomOffset = rand() % 3 - 1; //-1,0,1
  // Ball collision with top and bottom
  if (ballY <= 0 || ballY >= height - 1)
    ballDirY = -ballDirY;

  if (ballY <= 0)
    ballY = 0;
  else if (ballY >= height - 1)
    ballY = height - 1;

  // Ball collision with left paddle
  if (ballX == 1 && ballY >= paddle1Y && ballY <= paddle1Y + (paddleSize -1)) {
    ballDirX = -ballDirX; // Change horizontal direction
    // Adjust ballDirY based on where it hits the paddle
    int hitPos = ballY - paddle1Y;
    int paddleMidpoint = paddleSize / 2;
    int adjustment =  ballDirY = hitPos - paddleMidpoint; // -mid to +mid range, sharper angles for edges
    ballDirY = adjustment * 2 / paddleMidpoint;
  }

  // Ball collision with right paddle
  if (ballX == width - 2 && ballY >= paddle2Y && ballY <= paddle2Y + (paddleSize - 1)) {
    ballDirX = -ballDirX; // Change horizontal direction
    // Adjust ballDirY based on where it hits the paddle
    int hitPos = ballY - paddle2Y;
    ballDirY = hitPos - 2; // -2 to +2 range, sharper angles for edges
  }
  if (ballDirY == 0) {
    ballDirY = randomOffset;
  }

  // Scoring
  if (ballX == 0 || ballX == width - 1) {
    if (ballX == 0)
      score2++;
    else
      score1++;
    ballX = width / 2;
    ballY = height / 2;
    ballDirX = -ballDirX;
    ballDirY = 0; // Reset angle
  }
}

// void AutoMovePaddles() {
//     // Random movement tweak for AI paddles
//     int randomOffset = rand() % 3 - 1; // -1, 0, or 1

//     // Move paddle 1 (Left)
//     if (paddle1Y + 2 < ballY + randomOffset && paddle1Y + 4 < height - 1) {
//         paddle1Y++;
//     } else if (paddle1Y + 2 > ballY + randomOffset && paddle1Y > 0) {
//         paddle1Y--;
//     }

//     // Move paddle 2 (Right)
//     if (paddle2Y + 2 < ballY + randomOffset && paddle2Y + 4 < height - 1) {
//         paddle2Y++;
//     } else if (paddle2Y + 2 > ballY + randomOffset && paddle2Y > 0) {
//         paddle2Y--;
//     }
// }

// const int paddleSpeed = 1; // Max units the paddle can move per game loop
// iteration

// void AutoMovePaddles() {
//     // Random movement tweak for AI paddles
//     int randomOffset = rand() % 3 - 1; // -1, 0, or 1

//     // Determine direction for paddle 1 (Left) movement
//     if (paddle1Y + 2 < ballY + randomOffset && paddle1Y + 4 < height - 1) {
//         paddle1Y += min(paddleSpeed, ballY + randomOffset - (paddle1Y + 2));
//     } else if (paddle1Y + 2 > ballY + randomOffset && paddle1Y > 0) {
//         paddle1Y -= min(paddleSpeed, (paddle1Y + 2) - (ballY +
//         randomOffset));
//     }

//     // Determine direction for paddle 2 (Right) movement
//     if (paddle2Y + 2 < ballY + randomOffset && paddle2Y + 4 < height - 1) {
//         paddle2Y += min(paddleSpeed, ballY + randomOffset - (paddle2Y + 2));
//     } else if (paddle2Y + 2 > ballY + randomOffset && paddle2Y > 0) {
//         paddle2Y -= min(paddleSpeed, (paddle2Y + 2) - (ballY +
//         randomOffset));
//     }
// }

int PredictBallPosition(int paddleSide) {
  int randomOffset = rand() % 3 - 1; // -1, 0, or 1
  int predictedY = ballY;
  int ballDirYCopy = ballDirY; // Copy of the ball's Y direction for prediction

  int steps; // Steps needed for the ball to reach the paddle
  if (paddleSide == 1) {
    // Calculate steps for the ball to reach the left paddle
    steps = abs(ballX - 2); // Assuming the left paddle is at x=1
  } else {
    // Calculate steps for the ball to reach the right paddle
    steps =
        abs(ballX - (width - 3)); // Assuming the right paddle is at x=width-2
  }

  while (steps > 0) {
    predictedY += ballDirYCopy;

    // If the prediction goes out of bounds, invert the direction
    if (predictedY <= 0 || predictedY >= height - 1) {
      ballDirYCopy = -ballDirYCopy;
      predictedY += ballDirYCopy; // Adjust after bounce
    }
    steps--;
  }

  return predictedY +
         randomOffset; // Add random offset to the predicted Y position in order
                       // to create a more realistic gameplay;
}

void AutoMovePaddles() {
  int targetY1 = PredictBallPosition(1); // Prediction for paddle 1
  int targetY2 = PredictBallPosition(2); // Prediction for paddle 2

  // Randomly decide whether to move, simulating hesitation or error
  bool movePaddle1 = rand() % 2; // 50% chance to move
  bool movePaddle2 = rand() % 2; // 50% chance to move

  // Move paddle 1 (Left) towards the predicted position of the ball
  if (movePaddle1) {
    if (paddle1Y + (paddleSize-1)/2 < targetY1 && paddle1Y + (paddleSize-1) < height - 1) {
      paddle1Y++;
    } else if (paddle1Y + 2 > targetY1 && paddle1Y > 0) {
      paddle1Y--;
    }
  }

  // Move paddle 2 (Right) towards the predicted position of the ball
  if (movePaddle2) {
    if (paddle2Y + (paddleSize-1)/2 < targetY2 && paddle2Y + (paddleSize-1) < height - 1) {
      paddle2Y++;
    } else if (paddle2Y + (paddleSize-1)/2 > targetY2 && paddle2Y > 0) {
      paddle2Y--;
    }
  }
}

int main() {
  while (true) {
    AutoMovePaddles();
    Draw();
    // Input();
    Logic();
    std::this_thread::sleep_for(
        std::chrono::milliseconds(50)); // Slow down game to a reasonable speed
  }
  return 0;
}
