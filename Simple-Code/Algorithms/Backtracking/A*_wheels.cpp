#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <queue>
#include <cmath>
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>

using namespace std;

// State definition
struct State {
    int digit[4];
    int depth;
    int heuristic;

    bool operator<(const State& other) const {
        return heuristic > other.heuristic; // Min-heap based on heuristic
    }
};

// Function to calculate heuristic (distance to target)
int eval(const int curr[4], const int target[4]) {
    int h = 0;
    for (int i = 0; i < 4; ++i) {
        int diff = abs(curr[i] - target[i]);
        h += min(diff, 10 - diff); // Minimum rotation cost
    }
    return h;
}

// Generate next states from current state
vector<State> nextStates(State current, const int target[4]) {
    vector<State> nextStates;
    for (int i = 0; i < 4; ++i) {
        State clockwise = current;
        clockwise.digit[i] = (clockwise.digit[i] + 1) % 10;
        clockwise.depth = current.depth + 1;
        clockwise.heuristic = eval(clockwise.digit, target);
        nextStates.push_back(clockwise);

        State counterClockwise = current;
        counterClockwise.digit[i] = (counterClockwise.digit[i] + 9) % 10;
        counterClockwise.depth = current.depth + 1;
        counterClockwise.heuristic = eval(counterClockwise.digit, target);
        nextStates.push_back(counterClockwise);
    }
    return nextStates;
}

// A* search algorithm
vector<State> aStar(State start, const int target[4], const vector<vector<int>>& forbidden, vector<string>& logMessages, int& stepCount) {
    bool visited[10][10][10][10] = {false};
    for (const auto& f : forbidden) {
        visited[f[0]][f[1]][f[2]][f[3]] = true;
    }

    priority_queue<State> pq;
    pq.push(start);
    vector<State> path;

    while (!pq.empty()) {
        State current = pq.top();
        pq.pop();

        path.push_back(current);

        if (equal(begin(current.digit), end(current.digit), target)) {
            logMessages.push_back("Target Found!");
            stepCount = current.depth;
            return path;
        }

        if (visited[current.digit[0]][current.digit[1]][current.digit[2]][current.digit[3]]) continue;
        visited[current.digit[0]][current.digit[1]][current.digit[2]][current.digit[3]] = true;

        logMessages.push_back("Visiting State: " + to_string(current.digit[0]) + " " +
                                to_string(current.digit[1]) + " " +
                                to_string(current.digit[2]) + " " +
                                to_string(current.digit[3]));

        for (const auto& next : nextStates(current, target)) {
            if (!visited[next.digit[0]][next.digit[1]][next.digit[2]][next.digit[3]]) {
                pq.push(next);
            }
        }
    }
    logMessages.push_back("No Path Found");
    return {}; // Return empty path if target is not reachable
}

// Draw the input box
void drawInputBox(sf::RenderWindow& window, sf::Font& font, sf::RectangleShape& box, sf::Text& hintText, sf::Text& inputText) {
    window.draw(box);
    window.draw(hintText);
    window.draw(inputText);
}

// Draw wheels
void drawWheels(sf::RenderWindow& window, sf::Font& font, const int digits[4]) {
    sf::CircleShape wheel(50);
    wheel.setFillColor(sf::Color::White);
    wheel.setOutlineColor(sf::Color::Black);
    wheel.setOutlineThickness(3);

    sf::Text digitText;
    digitText.setFont(font);
    digitText.setCharacterSize(24);
    digitText.setFillColor(sf::Color::Black);

    for (int i = 0; i < 4; ++i) {
        wheel.setPosition(150 + i * 150, 250);
        digitText.setString(to_string(digits[i]));
        digitText.setPosition(180 + i * 150, 260);

        window.draw(wheel);
        window.draw(digitText);
    }
}

// Draw the log (steps and path)
void drawLog(sf::RenderWindow& window, sf::Font& font, const vector<string>& logMessages, int& scrollPosition) {
    sf::RectangleShape logBox(sf::Vector2f(600, 150));
    logBox.setFillColor(sf::Color(240, 240, 240));
    logBox.setPosition(100, 400);

    window.draw(logBox);

    sf::Text logText;
    logText.setFont(font);
    logText.setCharacterSize(18);
    logText.setFillColor(sf::Color::Black);

    float yPos = 410 - scrollPosition;
    int maxVisibleLines = 6;
    int logHeight = 22;

    for (size_t i = 0; i < logMessages.size(); ++i) {
        logText.setString(logMessages[i]);
        logText.setPosition(110, yPos);
        window.draw(logText);
        yPos += logHeight;

        if (yPos > 410 + (maxVisibleLines * logHeight)) {
            break;
        }
    }
}



// Draw the restart button
void drawRestartButton(sf::RenderWindow& window, sf::Font& font, sf::RectangleShape& button, sf::Text& buttonText) {
    window.draw(button);
    window.draw(buttonText);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Four Wheels Game");
    sf::Font font;
    if (!font.loadFromFile("Arial.ttf")) {
        cerr << "Failed to load font!" << endl;
        return -1;
    }

    sf::RectangleShape inputBox(sf::Vector2f(600, 50));
    inputBox.setFillColor(sf::Color(200, 200, 200));
    inputBox.setPosition(100, 100);

    sf::Text hintText, inputText;
    hintText.setFont(font);
    hintText.setCharacterSize(20);
    hintText.setFillColor(sf::Color::Black);
    hintText.setPosition(110, 70);

    inputText.setFont(font);
    inputText.setCharacterSize(20);
    inputText.setFillColor(sf::Color::Black);
    inputText.setPosition(110, 110);

    string inputBuffer;
    vector<vector<int>> forbidden;
    int initial[4] = {0, 0, 0, 0};
    int target[4] = {0, 0, 0, 0};
    int forbiddenCount = 0;
    int step = 0;
    vector<string> logMessages;
    int stepCount = 0;
    bool isRunning = false;

    sf::RectangleShape restartButton(sf::Vector2f(200, 50));
    restartButton.setFillColor(sf::Color(0, 128, 0));
    restartButton.setPosition(600, 530);

    sf::Text restartText;
    restartText.setFont(font);
    restartText.setString("Restart");
    restartText.setCharacterSize(25);
    restartText.setFillColor(sf::Color::White);
    restartText.setPosition(625, 540);

    int scrollPosition = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b' && !inputBuffer.empty()) {
                    inputBuffer.pop_back();
                } else if (event.text.unicode == '\r') {
                    stringstream ss(inputBuffer);
                    if (step == 0) {
                        ss >> initial[0] >> initial[1] >> initial[2] >> initial[3];
                        step++;
                        inputBuffer.clear();
                    } else if (step == 1) {
                        ss >> target[0] >> target[1] >> target[2] >> target[3];
                        step++;
                        inputBuffer.clear();
                    } else if (step == 2) {
                        ss >> forbiddenCount;
                        step++;
                        inputBuffer.clear();
                    } else if (step == 3) {
                        vector<int> f(4);
                        ss >> f[0] >> f[1] >> f[2] >> f[3];
                        forbidden.push_back(f);
                        if (forbidden.size() == static_cast<size_t>(forbiddenCount)) {
                            step++;
                            inputBuffer.clear();
                        }
                    }
                } else {
                    inputBuffer += event.text.unicode;
                }
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (restartButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    step = 0;
                    forbidden.clear();
                    inputBuffer.clear();
                    logMessages.clear();
                    isRunning = false;
                }
            }

            if (event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.delta > 0) {
                    scrollPosition -= 20;
                } else if (event.mouseWheelScroll.delta < 0) {
                    scrollPosition += 20;
                }
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) {
                    scrollPosition -= 20;
                } else if (event.key.code == sf::Keyboard::Down) {
                    scrollPosition += 20;
                }
            }

            if (scrollPosition < 0) {
                scrollPosition = 0;
            }

            int maxScroll = logMessages.size() * 22 - 150;
            if (scrollPosition > maxScroll) {
                scrollPosition = maxScroll;
            }
        }

        window.clear(sf::Color::White);

        if (step == 0) {
            hintText.setString("Enter Initial State (4 digits, e.g., 1 2 3 4):");
        } else if (step == 1) {
            hintText.setString("Enter Target State (4 digits, e.g., 5 6 7 8):");
        } else if (step == 2) {
            hintText.setString("Enter Forbidden States Count:");
        } else if (step == 3) {
            hintText.setString("Enter Forbidden States (4 digits, e.g., 9 9 9 9):");
        } else if (step == 4 && !isRunning) {
            hintText.setString("Running A* Search...");
            isRunning = true;

            vector<State> path = aStar({{initial[0], initial[1], initial[2], initial[3]}, 0, eval(initial, target)}, target, forbidden, logMessages, stepCount);

            for (size_t i = 0; i < path.size(); ++i) {
                drawWheels(window, font, path[i].digit);
                window.display();
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }

            logMessages.push_back("Number of Steps: " + to_string(stepCount));
        }

        inputText.setString(inputBuffer);
        drawInputBox(window, font, inputBox, hintText, inputText);
        drawLog(window, font, logMessages, scrollPosition);
        drawRestartButton(window, font, restartButton, restartText);

        window.display();
    }

    return 0;
}

