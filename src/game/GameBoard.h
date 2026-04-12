#ifndef NVE_GAME_BOARD_H
#define NVE_GAME_BOARD_H

#include "Ball.h"
#include "Brick.h"
#include <GameComponent.h>
#include <memory>
#include <vector>

namespace nuvelocity
{
    class Game;
}

namespace nuvelocity::frs42
{
    class GameBoard : public GameComponent
    {
    public:
        GameBoard();
        virtual ~GameBoard();

        void Update(Game* aGame) override;
        void Draw(Game* aGame) override;

        void AddBall(std::unique_ptr<Ball> ball)
        {
            mBalls.push_back(std::move(ball));
        }
        void AddBrick(std::unique_ptr<Brick> brick)
        {
            mBricks.push_back(std::move(brick));
        }

        const std::vector<std::unique_ptr<Ball>>& GetBalls() const
        {
            return mBalls;
        }
        const std::vector<std::unique_ptr<Brick>>& GetBricks() const
        {
            return mBricks;
        }

    private:
        std::vector<std::unique_ptr<Ball>> mBalls;
        std::vector<std::unique_ptr<Brick>> mBricks;
    };
} // namespace nuvelocity::frs42

#endif // NVE_GAME_BOARD_H
