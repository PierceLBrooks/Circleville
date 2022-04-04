
// Author: Pierce Brooks

#include <cv/Circleville.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>

int main(int argc, char** argv)
{
    std::string report = "";

    sf::VideoMode screen(sf::VideoMode::getDesktopMode());

    sf::RenderWindow window(screen, "Circleville");
    window.setFramerateLimit(30);

    sf::Image icon;
    if (icon.loadFromFile("circleville.jpg"))
    {
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    }
    else
    {
        std::cout << "No Icon" << std::endl;
    }

    sf::View view = window.getDefaultView();

    sf::Color background = sf::Color::White;

    sf::Font* font = new sf::Font();
    if (!font->loadFromFile("circleville.ttf"))
    {
        delete font;
        font = nullptr;
        std::cout << "No Font" << std::endl;
    }

    sf::Text* text = new sf::Text();
    if (font != nullptr)
    {
        text->setFont(*font);
    }
    else
    {
        delete text;
        text = nullptr;
    }

    bool active = true;
    bool first = true;

    int touches = 0;
    int touchesReal = 0;

    unsigned int fociBase = 3;
    unsigned int foci = fociBase;

    float score = 0.0f;
    float timeGainBase = 30.0f;
    float timeLost = 0.0f;
    float timeLeftBase = timeGainBase*2.0f;
    float timeLeft = timeLeftBase*powf(timeLeftBase/timeGainBase, 2.0f);

    sf::Clock clock;

    cv::Circleville* circleville = new cv::Circleville((window.getSize().x > window.getSize().y) ? true : false, window.getSize(), foci, [](const std::string& message){std::cout << message << std::endl;}, *reinterpret_cast<unsigned int*>(&window), timeLeft, timeLeftBase, font);

    clock.restart();

    while (window.isOpen())
    {
        sf::Event event;

        float deltaTime = clock.restart().asSeconds();

        while (active ? window.pollEvent(event) : window.waitEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Escape)
                    {
                        window.close();
                    }
                    break;
                case sf::Event::LostFocus:
                    active = false;
                    background = sf::Color::Black;
                    break;
                case sf::Event::GainedFocus:
                    active = true;
                    background = sf::Color::White;
                    first = true;
                    break;
            }
        }

        if (active)
        {
            if ((sf::Mouse::isButtonPressed(sf::Mouse::Left)) || (sf::Mouse::isButtonPressed(sf::Mouse::Right)))
            {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                {
                    touchesReal = 1;
                }
                else
                {
                    touchesReal = 2;
                }
            }
            else
            {
                touchesReal = 0;
            }
            touches = touchesReal;
            if (circleville != nullptr)
            {
                circleville->touch(sf::Mouse::getPosition(window), touches, false);
            }
            else
            {
                if (touches == 1)
                {
                    report = "";
                    foci = fociBase;
                    score = 0.0f;
                    timeLost = 0.0f;
                    timeLeft = timeLeftBase*powf(timeLeftBase/timeGainBase, 2.0f);
                    circleville = new cv::Circleville((view.getSize().x > view.getSize().y) ? true : false, sf::Vector2u(view.getSize()), foci, [](const std::string& message){std::cout << message << std::endl;}, *reinterpret_cast<unsigned int*>(&window), timeLeft, timeLeftBase, font);
                }
            }
            window.clear(background);
            if (circleville != nullptr)
            {
                while (true)
                {
                    int result = circleville->update(&window, deltaTime, touches, first, active);
                    if (result > 0)
                    {
                        float timeGain = std::max(10.0f, sqrtf(timeGainBase*static_cast<float>(foci))-timeLost);
                        score += timeGain;
                        timeLeft += timeGain;
                        timeLost = 0.0f;
                        first = false;
                        touches = 0;
                        ++foci;
                        delete circleville;
                        circleville = new cv::Circleville((view.getSize().x > view.getSize().y) ? true : false, sf::Vector2u(view.getSize()), foci, [](const std::string& message){std::cout << message << std::endl;}, *reinterpret_cast<unsigned int*>(&window), timeLeft, timeLeftBase, font);
                    }
                    else if (result < 0)
                    {
                        score *= static_cast<float>(foci);
                        timeLeft = 0.0f;
                        std::cout << score << std::endl;
                        report = "   Game over!\nScore: "+std::to_string(static_cast<int>(score))+"\nRounds: "+std::to_string(foci-2);
                        delete circleville;
                        circleville = nullptr;
                        break;
                    }
                    else
                    {
                        break;
                    }
                }
            }
            else
            {
                if (text != nullptr)
                {
                    text->setString(sf::String(report));
                    text->setOrigin(sf::Vector2f(text->getGlobalBounds().width, text->getGlobalBounds().height)*0.5f);
                    text->setPosition(view.getCenter());
                    text->setFillColor(sf::Color::Black);
                    window.draw(*text);
                }
            }
            window.display();
            first = false;
        }
        else
        {
            deltaTime = 0.0f;
            if (circleville != nullptr)
            {
                while (true)
                {
                    int result = circleville->update(&window, deltaTime, touches, first, active);
                    if (result > 0)
                    {
                        float timeGain = std::max(10.0f, sqrtf(timeGainBase*static_cast<float>(foci))-timeLost);
                        score += timeGain;
                        timeLeft += timeGain;
                        timeLost = 0.0f;
                        first = false;
                        touches = 0;
                        ++foci;
                        delete circleville;
                        circleville = new cv::Circleville((view.getSize().x > view.getSize().y) ? true : false, sf::Vector2u(view.getSize()), foci, [](const std::string& message){std::cout << message << std::endl;}, *reinterpret_cast<unsigned int*>(&window), timeLeft, timeLeftBase, font);
                    }
                    else if (result < 0)
                    {
                        score *= static_cast<float>(foci);
                        timeLeft = 0.0f;
                        std::cout << score << std::endl;
                        report = "   Game over!\nScore: "+std::to_string(static_cast<int>(score))+"\nRounds: "+std::to_string(foci-2);
                        delete circleville;
                        circleville = nullptr;
                        break;
                    }
                    else
                    {
                        break;
                    }
                }
                clock.restart();
            }
            first = false;
        }
        timeLeft -= deltaTime;
        if (timeLeft > 0.0f)
        {
            std::cout << timeLeft << std::endl;
        }
        timeLost += deltaTime;
    }

    delete circleville;
    delete text;
    delete font;

    return 0;
}
