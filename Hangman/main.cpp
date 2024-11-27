#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <cstdlib>

// Struct to manage game assets
struct GameAssets {
    sf::SoundBuffer winBuffer, loseBuffer;
    sf::Sound winSound, loseSound;
    sf::Font font;
    sf::Texture hangmanWinTexture;
    std::vector<sf::Texture> hangmanImages;
};

// Load words and hints from a CSV file
std::vector<std::pair<std::string, std::string>> loadWordsAndHints(const std::string& filename) {
    std::vector<std::pair<std::string, std::string>> wordsAndHints;
    std::ifstream file(filename);
    std::string line, word, hint;

    if (!file) {
        std::cerr << "Failed to open " << filename << std::endl;
        return wordsAndHints;
    }
    
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        if (std::getline(ss, word, ',') && std::getline(ss, hint)) {
            wordsAndHints.emplace_back(word, hint);
        }
    }
    return wordsAndHints;
}

bool loadAssets(GameAssets& assets) {
    if (!assets.winBuffer.loadFromFile("../audio/cheer.wav") ||
        !assets.loseBuffer.loadFromFile("../audio/death.wav") ||
        !assets.font.loadFromFile("../font/Lindelof-Font.ttf") ||
        !assets.hangmanWinTexture.loadFromFile("../imgs/hangmanwin.png")) {
        std::cerr << "Failed to load a required asset" << std::endl;
        return false;
    }
    assets.winSound.setBuffer(assets.winBuffer);
    assets.loseSound.setBuffer(assets.loseBuffer);

    // Load hangman images
    for (int i = 0; i < 7; ++i) {
        sf::Texture texture;
        std::string filename = "../imgs/hangman" + std::to_string(i) + ".png";
        if (!texture.loadFromFile(filename)) {
            std::cerr << "Failed to load " << filename << std::endl;
            return false;
        }
        assets.hangmanImages.push_back(texture);
    }
    return true;
}

// Display the selected word with blanks for unguessed letters
std::string getDisplayedWord(const std::string& word, const std::vector<char>& guessedLetters) {
    std::string displayedWord;
    for (char c : word) {
        if (std::find(guessedLetters.begin(), guessedLetters.end(), c) != guessedLetters.end())
            displayedWord += c;
        else
            displayedWord += "_";
    }
    return displayedWord;
}

// Update the hangman image based on incorrect guesses
void updateHangmanImage(sf::Sprite& hangmanSprite, const GameAssets& assets, int incorrectGuesses) {
    if (incorrectGuesses < assets.hangmanImages.size()) {
        hangmanSprite.setTexture(assets.hangmanImages[incorrectGuesses]);
    }
}

void resetGame(std::string& word, std::string& hint, std::vector<char>& guessedLetters, 
               int& incorrectGuesses, bool& gameWon, bool& gameLost, const std::vector<std::pair<std::string, std::string>>& wordsAndHints) {
    guessedLetters.clear();
    incorrectGuesses = 0;
    gameWon = false;
    gameLost = false;

    // Randomly pick a word and its associated hint
    auto& selectedPair = wordsAndHints[std::rand() % wordsAndHints.size()];
    word = selectedPair.first;
    hint = selectedPair.second;
}
int main() {
    sf::RenderWindow window(sf::VideoMode(1080, 820), "Hangman of SFML");

    GameAssets assets;
    if (!loadAssets(assets)) return -1;

    sf::RectangleShape background(sf::Vector2f(1080, 820));
    background.setFillColor(sf::Color::White);

    sf::Text wordText("", assets.font, 50);
    wordText.setPosition(100, 200);
    wordText.setFillColor(sf::Color::Black);

    sf::Text guessedText("Guessed: ", assets.font, 50);
    guessedText.setPosition(100, 400);
    guessedText.setFillColor(sf::Color::Black);

    sf::Text hintText("", assets.font, 30);
    hintText.setPosition(100, 250);
    hintText.setFillColor(sf::Color::Blue);

    sf::Sprite hangmanSprite;
    hangmanSprite.setPosition(500, 50);
    hangmanSprite.setScale(0.5, 0.5);

    std::vector<std::pair<std::string, std::string>> wordsAndHints = loadWordsAndHints("../words.csv");
    if (wordsAndHints.empty()) {
        std::cerr << "No words available" << std::endl;
        return -1;
    }

    std::string word, hint;
    bool gameWon = false, gameLost = false;
    std::vector<char> guessedLetters;
    int incorrectGuesses = 0;
    resetGame(word, hint, guessedLetters, incorrectGuesses, gameWon, gameLost, wordsAndHints);

    hintText.setString("Hint: " + hint);

    sf::Clock gameClock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }

            if (!gameWon && !gameLost && event.type == sf::Event::TextEntered && event.text.unicode >= 'a' && event.text.unicode <= 'z') {
                char guessedChar = static_cast<char>(event.text.unicode);

                if (std::find(guessedLetters.begin(), guessedLetters.end(), guessedChar) == guessedLetters.end()) {
                    guessedLetters.push_back(guessedChar);

                    if (word.find(guessedChar) != std::string::npos) {
                        wordText.setString(getDisplayedWord(word, guessedLetters));
                        gameWon = (wordText.getString().find("_") == std::string::npos);
                        if (gameWon) {
                            assets.winSound.play();
                            gameClock.restart();
                        }
                    } else {
                        incorrectGuesses++;
                        updateHangmanImage(hangmanSprite, assets, incorrectGuesses);
                        gameLost = (incorrectGuesses >= 6);
                        if (gameLost) {
                            assets.loseSound.play();
                            gameClock.restart();
                        }
                    }
                }
            }
        }

        std::string guessedString = "Guessed: ";
        for (char c : guessedLetters) guessedString += c + std::string(" ");
        guessedText.setString(guessedString);

        window.clear();
        window.draw(background);
        window.draw(wordText);
        window.draw(guessedText);
        window.draw(hintText);
        window.draw(hangmanSprite);

        if (gameLost) {
            sf::Text loseText("You Lose! The word was " + word, assets.font, 50);
            loseText.setFillColor(sf::Color::Red);
            loseText.setPosition(100, 500);
            window.draw(loseText);

            if (gameClock.getElapsedTime().asSeconds() > 3.f) {
                resetGame(word, hint, guessedLetters, incorrectGuesses, gameWon, gameLost, wordsAndHints);
                wordText.setString(getDisplayedWord(word, guessedLetters));
                hintText.setString("Hint: " + hint);
            }
        } else if (gameWon) {
            sf::Text winText("You Win!", assets.font, 50);
            winText.setFillColor(sf::Color::Green);
            winText.setPosition(100, 500);
            window.draw(winText);

            if (gameClock.getElapsedTime().asSeconds() > 3.f) {
                resetGame(word, hint, guessedLetters, incorrectGuesses, gameWon, gameLost, wordsAndHints);
                wordText.setString(getDisplayedWord(word, guessedLetters));
                hintText.setString("Hint: " + hint);
            }
        }

        window.display();
    }

    return 0;
}
