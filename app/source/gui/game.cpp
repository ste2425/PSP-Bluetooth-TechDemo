#include "gui.h"

#include "btController.h"
#include "btAnimations.h"
#include "g2d.h"
#include "colours.h"

#define PADDLE_HEIGHT 65
#define PADDLE_WIDTH 25
#define PLAYER_ONE_X 20
#define PLAYER_TWO_X (SCREEN_WIDTH - PADDLE_WIDTH - 20)
#define BALL_WIDTH 25
#define BALL_HEIGHT 25

namespace GUI {
    static float playerOneY = (SCREEN_HEIGHT / 2) - (PADDLE_HEIGHT / 2);
    static float playerTwoY = (SCREEN_HEIGHT / 2) - (PADDLE_HEIGHT / 2);
    static float ballX = (SCREEN_WIDTH / 2) - (BALL_WIDTH / 2);
    static float ballY = (SCREEN_HEIGHT / 2) - (BALL_HEIGHT / 2);
    // Add ball velocity and direction
    static float ballVelX = 0.25f;
    static float ballVelY = 0.15f;

    void setRedLED() {
        BTController::SetColour(0x00, 0xFF, 0x00, 0x00);
    }

    void setPurpleLED() {
        BTController::SetColour(0x00, 0x80, 0x00, 0x80);
    }

    void MainGameLoop(float delta)
    {
            BTController::LoadControllerState();

            // Paddle controls
            if (BTController::IsDpadHeld(0, BT_DPAD_UP)){//Utils::IsButtonHeld(PSP_CTRL_UP)) {
                playerOneY -= delta * 0.2f;
            } else if (BTController::IsDpadHeld(0, BT_DPAD_DOWN)){//Utils::IsButtonHeld(PSP_CTRL_DOWN)) {
                playerOneY += delta * 0.2f;
            }

            if (BTController::IsDpadHeld(1, BT_DPAD_UP)){//Utils::IsButtonHeld(PSP_CTRL_TRIANGLE)) {
                playerTwoY -= delta * 0.2f;
            } else if (BTController::IsDpadHeld(1, BT_DPAD_DOWN)){//Utils::IsButtonHeld(PSP_CTRL_CROSS)) {
                playerTwoY += delta * 0.2f;
            }

            // Clamp paddles
            if (playerOneY < 0.f) {
                playerOneY = 0.f;
            } else if (playerOneY > (SCREEN_HEIGHT - PADDLE_HEIGHT)) {
                playerOneY = SCREEN_HEIGHT - PADDLE_HEIGHT;
            }

            if (playerTwoY < 0.f) {
                playerTwoY = 0.f;
            } else if (playerTwoY > (SCREEN_HEIGHT - PADDLE_HEIGHT)) {
                playerTwoY = SCREEN_HEIGHT - PADDLE_HEIGHT;
            }

            // Ball movement
            ballX += ballVelX * (delta * 0.5f); // scale for frame rate
            ballY += ballVelY * (delta * 0.5f);

            // Ball collision with top/bottom
            if (ballY <= 0.f) {
                ballY = 0.f;
                ballVelY = -ballVelY;
            } else if (ballY >= (SCREEN_HEIGHT - BALL_HEIGHT)) {
                ballY = SCREEN_HEIGHT - BALL_HEIGHT;
                ballVelY = -ballVelY;
            }

            // Ball collision with paddles
            // Player One
            if (ballX <= PLAYER_ONE_X + PADDLE_WIDTH &&
                ballY + BALL_HEIGHT >= playerOneY &&
                ballY <= playerOneY + PADDLE_HEIGHT &&
                ballX >= PLAYER_ONE_X) {
                ballX = PLAYER_ONE_X + PADDLE_WIDTH;
                ballVelX = -ballVelX;
                    setPurpleLED();
                    
                    BTAnimations::StartAnimation(0);
                    BTController::SetVibration(0, 100, 180, 0, 200);

                    //::StartAnimation(1);
                // Add angle based on where it hit the paddle
                float hitPos = ((ballY + BALL_HEIGHT / 2) - (playerOneY + PADDLE_HEIGHT / 2)) / (PADDLE_HEIGHT / 2);
                ballVelY += hitPos * 0.15f;
            }

            // Player Two
            if (ballX + BALL_WIDTH >= PLAYER_TWO_X &&
                ballY + BALL_HEIGHT >= playerTwoY &&
                ballY <= playerTwoY + PADDLE_HEIGHT &&
                ballX + BALL_WIDTH <= PLAYER_TWO_X + PADDLE_WIDTH + BALL_WIDTH) {
                ballX = PLAYER_TWO_X - BALL_WIDTH;
                ballVelX = -ballVelX;
                    setRedLED();
                    BTController::SetVibration(1, 100, 180, 0, 200);
                // Add angle based on where it hit the paddle
                float hitPos = ((ballY + BALL_HEIGHT / 2) - (playerTwoY + PADDLE_HEIGHT / 2)) / (PADDLE_HEIGHT / 2);
                ballVelY += hitPos * 0.15f;
            }

            // Ball out of bounds (reset to center)
            if (ballX < 0.f || ballX > (SCREEN_WIDTH - BALL_WIDTH)) {
                ballX = (SCREEN_WIDTH / 2) - (BALL_WIDTH / 2);
                ballY = (SCREEN_HEIGHT / 2) - (BALL_HEIGHT / 2);
                ballVelX = (ballX < 0.f) ? 0.25f : -0.25f;
                ballVelY = 0.15f;
            }

            G2D::DrawRect(PLAYER_ONE_X, playerOneY, PADDLE_WIDTH, PADDLE_HEIGHT, STATUS_BAR_COLOUR);            
            G2D::DrawRect(PLAYER_TWO_X, playerTwoY, PADDLE_WIDTH, PADDLE_HEIGHT, STATUS_BAR_COLOUR);

            G2D::DrawRect(ballX, ballY, BALL_WIDTH, BALL_HEIGHT, STATUS_BAR_COLOUR);

    }
}