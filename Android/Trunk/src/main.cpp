#include "native-lib.hpp"
#include "Circleville.hpp"
#include "../../SFML-2.4.1/include/SFML/Window/Event.hpp"
#include <SFML/System/Clock.hpp>

int main(int argc, char *argv[])
{
    // Retrieve the JVM
    JavaVM* vm = getJvm();

    // Retrieve the JNI environment
    JNIEnv* env = getEnv();

    jint res = vm->AttachCurrentThread(&env, NULL);

    if (res == JNI_ERR)
    {
        LOGGER << "ERROR";
        return EXIT_FAILURE;
    }

    sf::VideoMode screen(sf::VideoMode::getDesktopMode());

    sf::RenderWindow window(screen, "Circleville");
    window.setFramerateLimit(30);

    sf::View view = window.getDefaultView();

    sf::Color background = sf::Color::White;

    // We shouldn't try drawing to the screen while in background
    // so we'll have to track that. You can do minor background
    // work, but keep battery life in mind.
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

    cv::Circleville* circleville = new cv::Circleville((window.getSize().x > window.getSize().y) ? true : false, window.getSize(), foci, [](const std::string& message){LOGGER << message;}, *reinterpret_cast<unsigned int*>(&window), timeLeft, timeLeftBase);

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
                case sf::Event::Resized:
                    view.setSize(static_cast<float>(event.size.width), static_cast<float>(event.size.height));
                    view.setCenter(view.getSize()*0.5f);
                    window.setView(view);
                    break;
                case sf::Event::LostFocus:
                    background = sf::Color::Black;
                    break;
                case sf::Event::GainedFocus:
                    background = sf::Color::White;
                    first = true;
                    break;
                // On Android MouseLeft/MouseEntered are (for now) triggered,
                // whenever the app loses or gains focus.
                case sf::Event::MouseLeft:
                    active = false;
                    break;
                case sf::Event::MouseEntered:
                    active = true;
                    first = true;
                    break;
                case sf::Event::TouchBegan:
                    ++touchesReal;
                    if (abs(touches-touchesReal) <= 1)
                    {
                        touches = touchesReal;
                    }
                    //LOGGER << touches;
                    if (event.touch.finger == 0)
                    {
                        if (circleville != nullptr)
                        {
                            circleville->touch(sf::Vector2i(event.touch.x, event.touch.y), touches, true);
                        }
                        else
                        {
                            if (touches <= 1)
                            {
                                foci = fociBase;
                                score = 0.0f;
                                timeLost = 0.0f;
                                timeLeft = timeLeftBase*powf(timeLeftBase/timeGainBase, 2.0f);
                                circleville = new cv::Circleville((view.getSize().x > view.getSize().y) ? true : false, sf::Vector2u(view.getSize()), foci, [](const std::string& message){LOGGER << message;}, *reinterpret_cast<unsigned int*>(&window), timeLeft, timeLeftBase);
                            }
                        }
                    }
                    break;
                case sf::Event::TouchMoved:
                    if (event.touch.finger == 0)
                    {
                        if (circleville != nullptr)
                        {
                            circleville->touch(sf::Vector2i(event.touch.x, event.touch.y), touches, false);
                        }
                    }
                    break;
                case sf::Event::TouchEnded:
                    --touchesReal;
                    if (event.touch.finger == 0)
                    {
                        touches = 0;
                    }
                    if (touches == 0)
                    {
                        if (circleville != nullptr)
                        {
                            circleville->touch(sf::Vector2i(), touches, false);
                        }
                    }
                    break;
            }
        }

        if (active)
        {
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
                        circleville = new cv::Circleville((view.getSize().x > view.getSize().y) ? true : false, sf::Vector2u(view.getSize()), foci, [](const std::string& message){LOGGER << message;}, *reinterpret_cast<unsigned int*>(&window), timeLeft, timeLeftBase);
                    }
                    else if (result < 0)
                    {
                        score *= static_cast<float>(foci);
                        timeLeft = 0.0f;
                        LOGGER << score;
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
                        circleville = new cv::Circleville((view.getSize().x > view.getSize().y) ? true : false, sf::Vector2u(view.getSize()), foci, [](const std::string& message){LOGGER << message;}, *reinterpret_cast<unsigned int*>(&window), timeLeft, timeLeftBase);
                    }
                    else if (result < 0)
                    {
                        score *= static_cast<float>(foci);
                        timeLeft = 0.0f;
                        LOGGER << score;
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
            sf::sleep(sf::milliseconds(100));
            first = false;
        }
        timeLeft -= deltaTime;
        if (timeLeft > 0.0f)
        {
            LOGGER << timeLeft;
        }
        timeLost += deltaTime;
    }

    delete circleville;

    // Detach thread again
    vm->DetachCurrentThread();

    return EXIT_SUCCESS;
}
