
// Author: Pierce Brooks

#include "Circleville.hpp"
#include <cmath>

cv::Circleville::Circleville(bool orientation, const sf::Vector2u& resolution, unsigned int foci, std::function<void(const std::string&)> messenger, unsigned int seed, float timeLeft, float timeLeftBase) :
    touches(0),
    seed(seed),
    foci(foci),
    focusHue(0.0f),
    focusMove(-1.0f),
    focusMoveBase(1.0f),
    win(-1.0f),
    winDistance(0.0f),
    winThreshold(10.0f),
    temperature(0.0f),
    pi(3.141592654f),
    areaRatio(1.0f/3.0f),
    fociRadius(15.0f),
    timeLeft(timeLeft),
    timeLeftBase(timeLeftBase),
    origin(-sf::Vector2f(resolution)*0.5f),
    orientation(orientation),
    resolution(resolution),
    messenger(messenger),
    areaMiddle(nullptr),
    areaLeft(nullptr),
    areaRight(nullptr),
    fociCenter(nullptr),
    fociPerimeterReal(nullptr),
    fociPerimeterRealOutline(nullptr),
    fociControlsOutline(nullptr),
    goalTemperature(nullptr),
    goal(nullptr),
    focus(nullptr),
    cursor(nullptr),
    target(nullptr),
    random(nullptr)
{

}

cv::Circleville::~Circleville()
{
    delete goalTemperature;
    delete goal;
    delete focus;
    delete cursor;
    delete areaMiddle;
    delete areaLeft;
    delete areaRight;
    delete fociCenter;
    delete fociPerimeterReal;
    delete fociPerimeterRealOutline;
    delete fociControlsOutline;
    if (random != nullptr)
    {
        for (unsigned int i = 0; i != foci; ++i)
        {
            delete fociPerimeter[i];
            delete fociSpokes[i];
            delete fociShapes[i];
            delete fociReals[i];
        }
    }
    for (unsigned int i = 0; i != fociControls.size(); ++i)
    {
        delete fociControls[i];
    }
    for (unsigned int i = 0; i != fociControlsReal.size(); ++i)
    {
        delete fociControlsReal[i];
    }
    delete random;
}

void cv::Circleville::touch(const sf::Vector2i& location, int touches, bool first)
{
    sf::Vector2i previous = touchLocation;
    this->touches = touches;
    touchLocation = location;
    cursor->setPosition(sf::Vector2f(location)+origin);
    if ((target == nullptr) && (fociControls.size() > 0))
    {
        return;
    }
    if (touches > 1)
    {
        return;
    }
    if (touches == 0)
    {
        if (target == areaRight)
        {
            unsigned int cap = 0;
            for (unsigned int i = 0; i != foci; ++i)
            {
                if (fociShapes[i] == nullptr)
                {
                    continue;
                }
                ++cap;
            }
            if (fociIndices.size() < cap)
            {
                for (unsigned int i = 0; i != fociControlsReal.size(); ++i)
                {
                    delete fociControlsReal[i];
                }
                for (unsigned int i = 0; i != fociControls.size(); ++i)
                {
                    delete fociControls[i];
                }
                fociControlsReal.clear();
                fociControls.clear();
                fociIndices.clear();
            }
        }
        target = nullptr;
        touchLocation = sf::Vector2i();
        return;
    }
    if ((target == areaMiddle) || ((areaMiddle->getGlobalBounds().contains(sf::Vector2f(location))) && (target == nullptr)))
    {
        target = areaMiddle;
        if ((previous.x > 0) && (previous.y > 0))
        {
            areaRatio += static_cast<float>(location.x-previous.x)/static_cast<float>(resolution.x);
            areaRatio = std::min(std::max(areaRatio, 0.2f), 0.8f);
            initialize(false);
        }
        return;
    }
    if ((target == areaRight) || ((areaRight->getGlobalBounds().contains(sf::Vector2f(location))) && (target == nullptr)))
    {
        if (target == nullptr)
        {
            if (getDistance(areaRight->getPosition(), sf::Vector2f(location)) < spokeRadius)
            {
                target = areaRight;
                fociControls.push_back(new sf::RectangleShape());
                fociControls.back()->setPosition(sf::Vector2f(location));
                fociControls.back()->setOrigin(sf::Vector2f(fociRadius*0.25f, 0.0f));
                fociControls.back()->setFillColor(sf::Color::White);
            }
        }
        else
        {
            unsigned int cap = 0;
            for (unsigned int i = 0; i != foci; ++i)
            {
                if (fociShapes[i] == nullptr)
                {
                    continue;
                }
                ++cap;
            }
            if (fociIndices.size() < cap)
            {
                sf::Vector2f intersection;
                for (int i = 0; i != static_cast<int>(foci); ++i)
                {
                    if (fociShapes[i] == nullptr)
                    {
                        continue;
                    }
                    if (fociIndices.size() > 0)
                    {
                        int skip = -1;
                        for (unsigned int j = 0; j != fociIndices.size(); ++j)
                        {
                            if (fociIndices[j] == static_cast<unsigned int>(i))
                            {
                                skip = i;
                                break;
                            }
                        }
                        if (skip > -1)
                        {
                            continue;
                        }
                        if (!(((i+1)%static_cast<int>(foci) == fociIndices.back()) || ((i-1<0)?(static_cast<int>(foci)-1):(i-1) == fociIndices.back())))
                        {
                            continue;
                        }
                    }
                    if (getIntersection(intersection, fociControls.back()->getPosition(), sf::Vector2f(location), fociCenter->getPosition(), fociShapes[i]->getPosition()))
                    {
                        fociPoints.push_back(intersection);
                        fociIndices.push_back(static_cast<unsigned int>(i));
                        fociControls.back()->setFillColor(fociShapes[i]->getFillColor());
                        fociControls.back()->setSize(sf::Vector2f(fociRadius*0.5f, getDistance(fociControls.back()->getPosition(), intersection)));
                        fociControls.back()->setRotation((getDirection(fociControls.back()->getPosition(), intersection)-(pi*0.5f))*(180.0f/pi));
                        if (fociIndices.size() < cap)
                        {
                            fociControlsReal.push_back(new sf::CircleShape(fociRadius));
                            fociControlsReal.back()->setOrigin(sf::Vector2f(fociRadius, fociRadius));
                            fociControlsReal.back()->setPosition(intersection);
                            fociControlsReal.back()->setFillColor(fociControls.back()->getFillColor());
                            fociControls.push_back(new sf::RectangleShape());
                            fociControls.back()->setPosition(intersection);
                            fociControls.back()->setOrigin(sf::Vector2f(fociRadius*0.25f, 0.0f));
                            fociControls.back()->setFillColor(sf::Color::White);
                        }
                        else
                        {
                            fociControlsReal.push_back(new sf::CircleShape(fociRadius));
                            fociControlsReal.back()->setOrigin(sf::Vector2f(fociRadius, fociRadius));
                            fociControlsReal.back()->setPosition(intersection);
                            fociControlsReal.back()->setFillColor(fociShapes[i]->getFillColor());
                            fociControls.front()->setSize(sf::Vector2f(fociRadius*0.5f, getDistance(fociControls[1]->getPosition(), intersection)));
                            fociControls.front()->setRotation((getDirection(intersection, fociControls[1]->getPosition())-(pi*0.5f))*(180.0f/pi));
                            fociControls.front()->setPosition(intersection);
                            focusPoint = focus->getPosition()-areaLeft->getPosition();
                            focusPointPrevious = focusPoint;
                            focusMove = focusMoveBase;
                        }
                        break;
                    }
                }
            }
            if (fociIndices.size() < cap)
            {
                fociControls.back()->setSize(sf::Vector2f(fociRadius*0.5f, getDistance(fociControls.back()->getPosition(), sf::Vector2f(location))));
                fociControls.back()->setRotation((getDirection(fociControls.back()->getPosition(), sf::Vector2f(location))-(pi*0.5f))*(180.0f/pi));
            }
        }
        return;
    }
}

int cv::Circleville::update(sf::RenderWindow* window, float deltaTime, int touches, bool first, bool active)
{
    focusHue += deltaTime*360.0f;
    focusHue = fmodf(focusHue, 360.0f);
    if (win >= 0.0f)
    {
        win -= deltaTime;
        if (win < 0.0f)
        {
            return 2;
        }
    }
    if ((timeLeft < 0.0f) && (win < 0.0f))
    {
        return -1;
    }
    this->touches = touches;
    if ((sf::Vector2u(window->getView().getSize()) != resolution) || (random == nullptr) || (first))
    {
        origin = window->getView().getCenter();
        resolution = sf::Vector2u(window->getView().getSize());
        origin -= sf::Vector2f(resolution)*0.5f;
        initialize(first);
    }
    if (!active)
    {
        return 0;
    }
    timeLeft -= deltaTime;
    if ((focus != nullptr) && (fociControls.size() == 0))
    {
        if (getDistance(focus->getPosition(), areaLeft->getPosition()) > (1.0f/3.0f)*std::min(areaLeft->getSize().x, areaLeft->getSize().y)*(std::max(0.0f, std::min(timeLeft, timeLeftBase))/timeLeftBase))
        {
            return -2;
        }
    }
    window->draw(*areaLeft);
    window->draw(*areaRight);
    window->draw(*areaMiddle);
    if (goalTemperature != nullptr)
    {
        goalTemperature->setSize(sf::Vector2f(areaMiddle->getSize().x, temperature*areaMiddle->getSize().y));
        goalTemperature->setOrigin(goalTemperature->getSize()*0.5f);
        window->draw(*goalTemperature);
    }
    if (fociControlsOutline != nullptr)
    {
        window->draw(*fociControlsOutline);
    }
    for (unsigned int i = 0; i != foci; ++i)
    {
        if (fociPerimeter[i] != nullptr)
        {
            window->draw(*fociPerimeter[i]);
        }
        if (fociSpokes[i] != nullptr)
        {
            window->draw(*fociSpokes[i]);
        }
    }
    for (unsigned int i = 0; i != foci; ++i)
    {
        if (fociShapes[i] != nullptr)
        {
            window->draw(*fociShapes[i]);
        }
        if (win < 0.0f)
        {
            if (fociReals[i] != nullptr)
            {
                window->draw(*fociReals[i]);
            }
        }
    }
    if (fociPerimeterReal != nullptr)
    {
        fociPerimeterReal->setRadius((1.0f/3.0f)*std::min(areaLeft->getSize().x, areaLeft->getSize().y)*(std::max(0.0f, std::min(timeLeft, timeLeftBase))/timeLeftBase));
        fociPerimeterReal->setOrigin(sf::Vector2f(fociPerimeterReal->getRadius(), fociPerimeterReal->getRadius()));
        window->draw(*fociPerimeterReal);
    }
    if (fociPerimeterRealOutline != nullptr)
    {
        window->draw(*fociPerimeterRealOutline);
    }
    if (fociCenter != nullptr)
    {
        window->draw(*fociCenter);
    }
    if ((touches > 1) && (focusMove > 0.0f)  && (win < 0.0f) && (focus != nullptr))
    {
        for (unsigned int i = 0; i != fociControlsReal.size(); ++i)
        {
            delete fociControlsReal[i];
        }
        for (unsigned int i = 0; i != fociControls.size(); ++i)
        {
            delete fociControls[i];
        }
        fociControlsReal.clear();
        fociControls.clear();
        fociIndices.clear();
        focusPoint = focus->getPosition()-areaLeft->getPosition();
        focusPoint *= 1.0f/((1.0f/3.0f)*std::min(areaLeft->getSize().x, areaLeft->getSize().y));
        focusMove = -1.0f;
    }
    for (unsigned int i = 0; i != fociControls.size(); ++i)
    {
        window->draw(*fociControls[i]);
    }
    for (unsigned int i = 0; i != fociControlsReal.size(); ++i)
    {
        window->draw(*fociControlsReal[i]);
    }
    if (focus != nullptr)
    {
        focus->setFillColor(hsvToRGB(sf::Vector3f(focusHue, 1.0f, 1.0f)));
        if ((target == nullptr) && (fociControls.size() > 0) && (win < 0.0f))
        {
            focusMove -= deltaTime;
            while (focusMove < 0.0f)
            {
                if (fociControls.size() == 0)
                {
                    break;
                }
                focusMove += focusMoveBase;
                delete fociControls.front();
                delete fociControlsReal.front();
                fociControls.erase(fociControls.begin());
                fociControlsReal.erase(fociControlsReal.begin());
                fociIndices.erase(fociIndices.begin());
                focusPointPrevious = focusPoint;
            }
            if (focusMove > 0.0f)
            {
                if (fociControls.size() > 0)
                {
                    float key = getProjection(fociControlsReal[fociIndices.front()]->getPosition(), fociShapes[fociIndices.front()]->getPosition(), fociCenter->getPosition());
                    focusPoint = getInterpolation(getInterpolation(focusPointPrevious+areaLeft->getPosition(), fociReals[fociIndices.front()]->getPosition(), key), focusPointPrevious+areaLeft->getPosition(), focusMove/focusMoveBase);
                    focus->setPosition(focusPoint);
                    focusPoint -= areaLeft->getPosition();
                    temperature = getDistance(focus->getPosition(), areaLeft->getPosition()+(goalPoint*(1.0f/3.0f)*std::min(areaLeft->getSize().x, areaLeft->getSize().y)));
                    if (temperature < winThreshold)
                    {
                        win = 5.0f;
                        temperature = 1.0f;
                        focusPoint = focus->getPosition()-areaLeft->getPosition();
                        focusPoint *= 1.0f/((1.0f/3.0f)*std::min(areaLeft->getSize().x, areaLeft->getSize().y));
                    }
                    else
                    {
                        temperature = 1.0f-std::min(temperature/(winDistance-winThreshold), 1.0f);
                    }
                }
                else
                {
                    focusPoint = focus->getPosition()-areaLeft->getPosition();
                    focusPoint *= 1.0f/((1.0f/3.0f)*std::min(areaLeft->getSize().x, areaLeft->getSize().y));
                    focusMove = -1.0f;
                }
            }
            else
            {
                focusPoint = focus->getPosition()-areaLeft->getPosition();
                focusPoint *= 1.0f/((1.0f/3.0f)*std::min(areaLeft->getSize().x, areaLeft->getSize().y));
                focusMove = -1.0f;
            }
        }
        else
        {
            focus->setPosition(areaLeft->getPosition()+(focusPoint*(1.0f/3.0f)*std::min(areaLeft->getSize().x, areaLeft->getSize().y)));
        }
        if (focusMove < 0.0f)
        {
            if (win < 0.0f)
            {
                if ((fociControls.size() == 0) && (target == nullptr))
                {
                    while (getDistance(goalPoint, focusPoint)*((1.0f/3.0f)*std::min(areaLeft->getSize().x, areaLeft->getSize().y)) < fociRadius*pi)
                    {
                        goalPoint = areaLeft->getPosition();
                        for (unsigned int i = 0; i != foci; ++i)
                        {
                            goalPoint = getInterpolation(goalPoint, fociReals[i]->getPosition(), random->getFloat(0.0f, 1.0f));
                        }
                        winDistance = 0.0f;
                        for (unsigned int i = 0; i != foci; ++i)
                        {
                            float distance = getDistance(goalPoint, fociReals[i]->getPosition());
                            if (distance > winDistance)
                            {
                                winDistance = distance;
                            }
                        }
                        goalPoint -= areaLeft->getPosition();
                        goalPoint *= 1.0f/((1.0f/3.0f)*std::min(areaLeft->getSize().x, areaLeft->getSize().y));
                    }
                }
                temperature = getDistance(focus->getPosition(), areaLeft->getPosition()+(goalPoint*(1.0f/3.0f)*std::min(areaLeft->getSize().x, areaLeft->getSize().y)));
                if (temperature < winThreshold)
                {
                    win = 5.0f;
                    temperature = 1.0f;
                }
                else
                {
                    temperature = 1.0f-std::min(temperature/(winDistance-winThreshold), 1.0f);
                }
            }
            else
            {
                temperature = 1.0f;
            }
        }
        if (win > 0.0f)
        {
            focus->setPosition(goal->getPosition()+(sf::Vector2f(cosf(win*pi), sinf(win*pi))*fociRadius*pi));
        }
        window->draw(*focus);
    }
    if (cursor != nullptr)
    {
        window->draw(*cursor);
    }
    if (win > 0.0f)
    {
        goal->setPosition(areaLeft->getPosition()+(goalPoint*(1.0f/3.0f)*std::min(areaLeft->getSize().x, areaLeft->getSize().y)));
        goal->setFillColor(hsvToRGB(sf::Vector3f(360.0f-focusHue, 1.0f, 0.5f)));
        window->draw(*goal);
    }
    return 0;
}

void cv::Circleville::initialize(bool first)
{
    if (random == nullptr)
    {
        random = new Random(seed+foci);
    }
    if (cursor == nullptr)
    {
        cursor = new sf::CircleShape(fociRadius);
        cursor->setOrigin(sf::Vector2f(fociRadius, fociRadius));
        cursor->setFillColor(sf::Color::White);
        cursor->setPosition(sf::Vector2f(resolution)*0.5f);
    }
    if (areaMiddle == nullptr)
    {
        areaMiddle = new sf::RectangleShape(sf::Vector2f(fociRadius*4.0f, static_cast<float>(resolution.y)));
        areaMiddle->setFillColor(sf::Color::Black);
    }
    else
    {
        areaMiddle->setSize(sf::Vector2f(fociRadius*4.0f, static_cast<float>(resolution.y)));
    }
    if (areaLeft == nullptr)
    {
        areaLeft = new sf::RectangleShape(sf::Vector2f(static_cast<float>(resolution.x)*areaRatio, static_cast<float>(resolution.y)));
        areaLeft->setFillColor(sf::Color(160, 160, 160));
    }
    else
    {
        areaLeft->setSize(sf::Vector2f(static_cast<float>(resolution.x)*areaRatio, static_cast<float>(resolution.y)));
    }
    if (areaRight == nullptr)
    {
        areaRight = new sf::RectangleShape(sf::Vector2f(static_cast<float>(resolution.x)*(1.0f-areaRatio), static_cast<float>(resolution.y)));
        areaRight->setFillColor(sf::Color(96, 96, 96));
    }
    else
    {
        areaRight->setSize(sf::Vector2f(static_cast<float>(resolution.x)*(1.0f-areaRatio), static_cast<float>(resolution.y)));
    }
    areaLeft->setPosition(0.0f, 0.0f);
    areaRight->setPosition(areaLeft->getPosition());
    areaRight->move(areaLeft->getSize().x, 0.0f);
    areaMiddle->setPosition(areaRight->getPosition());
    areaLeft->setOrigin(areaLeft->getSize()*0.5f);
    areaRight->setOrigin(areaRight->getSize()*0.5f);
    areaMiddle->setOrigin(areaMiddle->getSize()*0.5f);
    areaLeft->move(origin);
    areaRight->move(origin);
    areaMiddle->move(origin);
    areaLeft->move(areaLeft->getOrigin());
    areaRight->move(areaRight->getOrigin());
    areaMiddle->move(areaMiddle->getOrigin());
    //messenger(std::to_string(areaRatio));
    spokeRadius = std::max(1.0f/3.0f, areaRatio);
    spokeRadius = std::min(areaRight->getSize().y*powf(spokeRadius, 1.0f-spokeRadius), areaRight->getSize().x*(1.0f-spokeRadius)*spokeRadius);
    if (goalTemperature == nullptr)
    {
        goalTemperature = new sf::RectangleShape(sf::Vector2f());
        goalTemperature->setFillColor(sf::Color::White);
    }
    goalTemperature->setPosition(areaMiddle->getPosition());
    if (goal == nullptr)
    {
        goal = new sf::CircleShape(fociRadius);
        goal->setOrigin(sf::Vector2f(fociRadius, fociRadius));
    }
    if (focus == nullptr)
    {
        focus = new sf::CircleShape(fociRadius);
        focus->setOrigin(sf::Vector2f(fociRadius, fociRadius));
        focus->setFillColor(sf::Color::White);
        focus->setPosition(areaLeft->getPosition());
    }
    if (fociCenter == nullptr)
    {
        fociCenter = new sf::CircleShape(fociRadius);
    }
    fociCenter->setFillColor(sf::Color::White);
    fociCenter->setPosition(areaRight->getPosition());
    fociCenter->setOrigin(sf::Vector2f(fociRadius, fociRadius));
    if (fociPerimeterReal == nullptr)
    {
        fociPerimeterReal = new sf::CircleShape((1.0f/3.0f)*std::min(areaLeft->getSize().x, areaLeft->getSize().y)*(std::max(0.0f, std::min(timeLeft, timeLeftBase))/timeLeftBase));
    }
    else
    {
        fociPerimeterReal->setRadius((1.0f/3.0f)*std::min(areaLeft->getSize().x, areaLeft->getSize().y)*(std::max(0.0f, std::min(timeLeft, timeLeftBase))/timeLeftBase));
    }
    fociPerimeterReal->setFillColor(sf::Color::Transparent);
    fociPerimeterReal->setOutlineColor(sf::Color::Black);
    fociPerimeterReal->setOutlineThickness(fociRadius*0.5f);
    fociPerimeterReal->setPosition(areaLeft->getPosition());
    fociPerimeterReal->setOrigin(sf::Vector2f(fociPerimeterReal->getRadius(), fociPerimeterReal->getRadius()));
    if (fociPerimeterRealOutline == nullptr)
    {
        fociPerimeterRealOutline = new sf::RectangleShape(sf::Vector2f((1.0f/3.0f)*std::min(areaLeft->getSize().x, areaLeft->getSize().y), (1.0f/3.0f)*std::min(areaLeft->getSize().x, areaLeft->getSize().y)));
    }
    else
    {
        fociPerimeterRealOutline->setSize(sf::Vector2f((1.0f/3.0f)*std::min(areaLeft->getSize().x, areaLeft->getSize().y), (1.0f/3.0f)*std::min(areaLeft->getSize().x, areaLeft->getSize().y)));
    }
    fociPerimeterRealOutline->setSize(fociPerimeterRealOutline->getSize()*2.0f);
    fociPerimeterRealOutline->setFillColor(sf::Color::Transparent);
    fociPerimeterRealOutline->setOutlineColor(sf::Color::Black);
    fociPerimeterRealOutline->setOutlineThickness(fociRadius*0.5f);
    fociPerimeterRealOutline->setPosition(areaLeft->getPosition());
    fociPerimeterRealOutline->setOrigin(fociPerimeterRealOutline->getSize()*0.5f);
    if (fociControlsOutline == nullptr)
    {
        fociControlsOutline = new sf::CircleShape(spokeRadius);
    }
    else
    {
        fociControlsOutline->setRadius(spokeRadius);
    }
    fociControlsOutline->setOrigin(sf::Vector2f(spokeRadius, spokeRadius));
    fociControlsOutline->setFillColor(sf::Color::Transparent);
    fociControlsOutline->setOutlineColor(sf::Color::Black);
    fociControlsOutline->setOutlineThickness(fociRadius*0.5f);
    fociControlsOutline->setPosition(areaRight->getPosition());
    for (unsigned int i = 0; i != foci; ++i)
    {
        float angle = (static_cast<float>(i)/static_cast<float>(foci))*pi*2.0f;
        angle -= pi*0.5f;
        if (fociPoints.size() <= i)
        {
            sf::Vector2f point(random->getFloat(-pi, pi), random->getFloat(0.0f, 1.0f));
            fociPoints.push_back(sf::Vector2f(cosf(point.x)*point.y, sinf(point.x)*point.y));
            fociReals.push_back(new sf::CircleShape(fociRadius));
            fociShapes.push_back(new sf::CircleShape(fociRadius));
            fociSpokes.push_back(new sf::RectangleShape());
            fociPerimeter.push_back(new sf::RectangleShape());
        }
        fociReals[i]->setFillColor(getColor(i+1));
        fociReals[i]->setPosition(areaLeft->getPosition()+(fociPoints[i]*(1.0f/3.0f)*std::min(areaLeft->getSize().x, areaLeft->getSize().y)));
        fociReals[i]->setOrigin(fociCenter->getOrigin());
        fociShapes[i]->setFillColor(fociReals[i]->getFillColor());
        fociShapes[i]->setPosition(fociCenter->getPosition()+(sf::Vector2f(cosf(angle), sinf(angle))*spokeRadius));
        fociShapes[i]->setOrigin(fociCenter->getOrigin());
        fociSpokes[i]->setFillColor(fociShapes[i]->getFillColor());
        fociSpokes[i]->setPosition(fociCenter->getPosition());
        fociSpokes[i]->setOrigin(fociRadius*0.25f, 0.0f);
        fociSpokes[i]->setRotation((getDirection(fociCenter->getPosition(), fociShapes[i]->getPosition())-(pi*0.5f))*(180.0f/pi));
        fociSpokes[i]->setSize(sf::Vector2f(fociRadius*0.5f, getDistance(fociShapes[i]->getPosition(), fociCenter->getPosition())));
        if (i > 0)
        {
            fociPerimeter[i-1]->setFillColor(sf::Color::Black);
            fociPerimeter[i-1]->setPosition(fociShapes[i-1]->getPosition());
            fociPerimeter[i-1]->setOrigin(fociRadius*0.25f, 0.0f);
            fociPerimeter[i-1]->setRotation((getDirection(fociShapes[i-1]->getPosition(), fociShapes[i]->getPosition())-(pi*0.5f))*(180.0f/pi));
            fociPerimeter[i-1]->setSize(sf::Vector2f(fociRadius*0.5f, getDistance(fociShapes[i]->getPosition(), fociShapes[i-1]->getPosition())));
            if (i == foci-1)
            {
                fociPerimeter[i]->setFillColor(sf::Color::Black);
                fociPerimeter[i]->setPosition(fociShapes[i]->getPosition());
                fociPerimeter[i]->setOrigin(fociRadius*0.25f, 0.0f);
                fociPerimeter[i]->setRotation((getDirection(fociShapes[i]->getPosition(), fociShapes[0]->getPosition())-(pi*0.5f))*(180.0f/pi));
                fociPerimeter[i]->setSize(sf::Vector2f(fociRadius*0.5f, getDistance(fociShapes[i]->getPosition(), fociShapes[0]->getPosition())));
            }
        }
    }
    winDistance = 0.0f;
    for (unsigned int i = 0; i != foci; ++i)
    {
        float distance = getDistance(goalPoint, fociReals[i]->getPosition());
        if (distance > winDistance)
        {
            winDistance = distance;
        }
    }
}

float cv::Circleville::getDirection(const sf::Vector2f& first, const sf::Vector2f& last)
{
    return atan2f(last.y-first.y, last.x-first.x);
}

float cv::Circleville::getDistance(const sf::Vector2f& left, const sf::Vector2f& right)
{
    return sqrtf(powf(left.x-right.x, 2.0f)+powf(left.y-right.y, 2.0f));
}

sf::Color cv::Circleville::getColor(unsigned int index)
{
    sf::Color color = sf::Color::Black;
    sf::Uint8* channels[3] = {&color.r, &color.g, &color.b};
    int key = static_cast<int>(index);
    int block = static_cast<int>(powf(2.0f, 3.0f));
    int temp = key/block;
    temp *= block;
    temp = key-temp;
    for (int i = 0; i != 3; ++i)
    {
        if (temp&(static_cast<int>(powf(2.0f, static_cast<float>(i)))))
        {
            *(channels[i]) = 255/(1+(abs(key)/block));
        }
    }
    return color;
}

bool cv::Circleville::getIntersection(sf::Vector2f& intersection, const sf::Vector2f& leftFirst, const sf::Vector2f& leftLast, const sf::Vector2f& rightFirst, const sf::Vector2f& rightLast)
{
    if (!checkIntersection(leftFirst, leftLast, rightFirst, rightLast))
    {
        return false;
    }
    sf::Vector2f differenceX(leftFirst.x-leftLast.x, rightFirst.x-rightLast.x);
    sf::Vector2f differenceY(leftFirst.y-leftLast.y, rightFirst.y-rightLast.y);
    float divisor = getDeterminant(differenceX, differenceY);
    if (divisor == 0.0f)
    {
        return false;
    }
    sf::Vector2f determinant(getDeterminant(leftFirst, leftLast), getDeterminant(rightFirst, rightLast));
    intersection = sf::Vector2f(getDeterminant(determinant, differenceX)/divisor, getDeterminant(determinant, differenceY)/divisor);
    return true;
}

bool cv::Circleville::checkIntersection(const sf::Vector2f& leftFirst, const sf::Vector2f& leftLast, const sf::Vector2f& rightFirst, const sf::Vector2f& rightLast)
{
    sf::Vector2i orientationFirst(checkOrientation(leftFirst, leftLast, rightFirst), checkOrientation(leftFirst, leftLast, rightLast));
    sf::Vector2i orientationLast(checkOrientation(rightFirst, rightLast, leftFirst), checkOrientation(rightFirst, rightLast, leftLast));
    if ((orientationFirst.x != orientationFirst.y) && (orientationLast.x != orientationLast.y))
    {
        return true;
    }
    if ((orientationFirst.x == 0) && (checkSegment(leftFirst, rightFirst, leftLast)))
    {
        return true;
    }
    if ((orientationFirst.y == 0) && (checkSegment(leftFirst, rightLast, leftLast)))
    {
        return true;
    }
    if ((orientationLast.x == 0) && (checkSegment(rightFirst, leftFirst, rightLast)))
    {
        return true;
    }
    if ((orientationLast.y == 0) && (checkSegment(rightFirst, leftLast, rightLast)))
    {
        return true;
    }
    return false;
}


bool cv::Circleville::checkSegment(const sf::Vector2f& first, const sf::Vector2f& last, const sf::Vector2f& other)
{
    if ((last.x <= std::max(first.x, other.x)) && (last.x >= std::min(first.x, other.x)) && (last.y <= std::max(first.y, other.y)) && (last.y >= std::min(first.y, other.y)))
    {
        return true;
    }
    return false;
}

int cv::Circleville::checkOrientation(const sf::Vector2f& first, const sf::Vector2f& last, const sf::Vector2f& other)
{
    float value = (((last.y-first.y)*(other.x-last.x))-((last.x-first.x)*(other.y-last.y)));
    if (value == 0.0f)
    {
        return 0;
    }
    if (value > 0.0f)
    {
        return 1;
    }
    return -1;
}

float cv::Circleville::getDeterminant(const sf::Vector2f& left, const sf::Vector2f& right)
{
    return ((left.x*right.y)-(left.y*right.x));
}

sf::Vector2f cv::Circleville::getInterpolation(const sf::Vector2f& left, const sf::Vector2f& right, float key)
{
    return sf::Vector2f((left.x*(1.0f-key))+(right.x*key), (left.y*(1.0f-key))+(right.y*key));
}

float cv::Circleville::getProjection(const sf::Vector2f& first, const sf::Vector2f& last, const sf::Vector2f& other)
{
    return getDistance(first, other)/getDistance(last, other);
}

sf::Color cv::Circleville::hsvToRGB(sf::Vector3f color)
{
    int i = 0;
    float f = 0.0f;
    float p = 0.0f;
    float q = 0.0f;
    float t = 0.0f;
    sf::Color colorNew = sf::Color::Black;
    if (color.y == 0.0f)
    {
        colorNew.r = color.z*255;
        colorNew.g = color.z*255;
        colorNew.b = color.z*255;
    }
    else
    {
        color.x /= 60.0f;
        i = floor(color.x);
        f = color.x-i;
        p = color.z*(1.0f-color.y);
        q = color.z*(1.0f-(color.y*f));
        t = color.z*(1.0f-(color.y*(1.0f-f)));
        switch (i)
        {
            case 0:
                colorNew.r = color.z*255;
                colorNew.g = t*255;
                colorNew.b = p*255;
                break;
            case 1:
                colorNew.r = q*255;
                colorNew.g = color.z*255;
                colorNew.b = p*255;
                break;
            case 2:
                colorNew.r = p*255;
                colorNew.g = color.z*255;
                colorNew.b = t*255;
                break;
            case 3:
                colorNew.r = p*255;
                colorNew.g = q*255;
                colorNew.b = color.z*255;
                break;
            case 4:
                colorNew.r = t*255;
                colorNew.g = p*255;
                colorNew.b = color.z*255;
                break;
            case 5:
                colorNew.r = color.z*255;
                colorNew.g = p*255;
                colorNew.b = q*255;
                break;
        }
    }
    return colorNew;
}

sf::Vector3f cv::Circleville::rgbToHSV(sf::Color color, bool normalize)
{
    float rgbMax;
    float rgbDif;
    sf::Vector3f newColor;
    sf::Vector3f oldColor(((float)color.r)/255.0f,((float)color.g)/255.0f,((float)color.b)/255.0f);
    rgbMax = std::max(oldColor.x,std::max(oldColor.y,oldColor.z));
    newColor.z = rgbMax;
    rgbDif = rgbMax-std::min(oldColor.x,std::min(oldColor.y,oldColor.z));
    if (rgbMax != 0)
    {
        newColor.y = rgbDif/rgbMax;
    }
    else
    {
        newColor.y = 0;
        newColor.x = 0;
        return newColor;
    }
    if (oldColor.x == rgbMax)
    {
        newColor.x = (oldColor.y-oldColor.z)/rgbDif;
    }
    else
    {
        if (oldColor.y == rgbMax)
        {
            newColor.x = 2+((oldColor.z-oldColor.x)/rgbDif);
        }
        else
        {
            newColor.x = 4+((oldColor.x-oldColor.y)/rgbDif);
        }
    }
    newColor.x *= 60;
    while (newColor.x < 0)
    {
        newColor.x += 360;
    }
    if (normalize)
    {
        newColor.x /= 360;
        return newColor;
    }
    return newColor;
}
