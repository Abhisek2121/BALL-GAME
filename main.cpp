#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

class Ball {
public:
    sf::CircleShape shape;
    bool isWhite;
    float speed;

    Ball(float radius, bool isWhite) : isWhite(isWhite) {
        shape.setRadius(radius);
        shape.setPosition(rand() % 800, 0);
        shape.setFillColor(isWhite ? sf::Color::White : sf::Color::Red);
        speed = 100.0f + static_cast<float>(rand() % 100);
    }

    void respawn() {
        shape.setPosition(rand() % 800, 0);
    }

    void update(float deltaTime) {
        shape.move(0, speed * deltaTime);
    }
};

class Game {
public:
    Game() : score(0), timeLimit(30), isPaused(true), isGameOver(false), bucketWidth(100), bucketHeight(20) {
        window.create(sf::VideoMode(800, 600), "Catch the Ball Game");
        font.loadFromFile("KOMIKAP_.ttf");
        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);
        gameOverText.setFont(font);
        gameOverText.setCharacterSize(48);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setString("Game Over!");
        gameOverText.setPosition(300, 250);

        for (int i = 0; i < 6; ++i) {
            balls.push_back(Ball(20, false));
        }
        balls.push_back(Ball(20, true));

        bucket.setSize(sf::Vector2f(bucketWidth, bucketHeight));
        bucket.setFillColor(sf::Color::Yellow);
        bucket.setPosition((800 - bucketWidth) / 2, 580);

        timeBar.setSize(sf::Vector2f(200, 20));
        timeBar.setFillColor(sf::Color::Blue);
        timeBar.setPosition(600, 0);

        srand(static_cast<unsigned int>(time(0)));
    }

    void run() {
        sf::Clock clock;
        float elapsedTime = 0;

        while (window.isOpen()) {
            handleEvents();
            if (!isPaused && !isGameOver) {
                float deltaTime = clock.restart().asSeconds();
                update(deltaTime);
                elapsedTime += deltaTime;
                if (elapsedTime >= 1.0f) {
                    timeLimit--;
                    elapsedTime = 0;
                }
                if (timeLimit <= 0) {
                    gameOver();
                }
            }
            render();
        }
    }

private:
    sf::RenderWindow window;
    sf::Font font;
    sf::Text scoreText;
    sf::Text gameOverText;
    std::vector<Ball> balls;
    sf::RectangleShape bucket;
    sf::RectangleShape timeBar;
    int score;
    int timeLimit;
    bool isPaused;
    bool isGameOver;
    float bucketWidth;
    float bucketHeight;

    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    isPaused = !isPaused;
                }
                if (event.key.code == sf::Keyboard::Enter) {
                    if (isGameOver) {
                        restartGame();
                    } else {
                        isPaused = false;
                    }
                }
                if (event.key.code == sf::Keyboard::Left) {
                    if (bucket.getPosition().x > 0) {
                        bucket.move(-20, 0);
                    }
                }
                if (event.key.code == sf::Keyboard::Right) {
                    if (bucket.getPosition().x < 800 - bucketWidth) {
                        bucket.move(20, 0);
                    }
                }
            }
        }
    }

    void update(float deltaTime) {
        for (auto& ball : balls) {
            ball.update(deltaTime);

            if (ball.shape.getPosition().y > 600) {
                ball.respawn();
            }

            if (ball.shape.getGlobalBounds().intersects(bucket.getGlobalBounds())) {
                if (ball.isWhite) {
                    score++;
                } else {
                    gameOver();
                }
                ball.respawn();
            }

            if (!ball.isWhite && ball.shape.getPosition().y < 0) {
                gameOver();
            }
        }

        scoreText.setString("Score: " + std::to_string(score));
        scoreText.setPosition(10, 10);

        float timeBarWidth = (static_cast<float>(timeLimit) / 30.0f) * 200;
        timeBar.setSize(sf::Vector2f(timeBarWidth, 20));
        timeBar.setPosition(600, 0);
    }

    void render() {
        window.clear();
        for (const auto& ball : balls) {
            window.draw(ball.shape);
        }
        window.draw(bucket);
        window.draw(scoreText);
        window.draw(timeBar);
        if (isGameOver) {
            window.draw(gameOverText);
        }
        window.display();
    }

    void gameOver() {
        isGameOver = true;
        std::cout << "Game Over! Final Score: " << score << std::endl;
    }

    void restartGame() {
        score = 0;
        timeLimit = 30;
        isPaused = true;
        isGameOver = false;
        for (auto& ball : balls) {
            ball.respawn();
        }
        bucket.setPosition((800 - bucketWidth) / 2, 580);
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}
