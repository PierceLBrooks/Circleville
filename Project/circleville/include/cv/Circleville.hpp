
// Author: Pierce Brooks

#ifndef CV_CIRCLEVILLE_HPP
#define CV_CIRCLEVILLE_HPP

#include <functional>
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include "Random.hpp"

namespace cv
{
    class Circleville
    {
        public:
            Circleville(bool orientation, const sf::Vector2u& resolution, unsigned int foci, std::function<void(const std::string&)> messenger, unsigned int seed, float timeLeft, float timeLeftBase, sf::Font* font = nullptr);
            ~Circleville();
            void touch(const sf::Vector2i& location, int touches, bool first);
            int update(sf::RenderWindow* window, float deltaTime, int touches, bool first, bool active);
        private:
            void initialize(bool first);
            static float getDirection(const sf::Vector2f& first, const sf::Vector2f& last);
            static float getDistance(const sf::Vector2f& left, const sf::Vector2f& right);
            static sf::Color getColor(unsigned int index);
            static bool getIntersection(sf::Vector2f& intersection, const sf::Vector2f& leftFirst, const sf::Vector2f& leftLast, const sf::Vector2f& rightFirst, const sf::Vector2f& rightLast);
            static bool checkIntersection(const sf::Vector2f& leftFirst, const sf::Vector2f& leftLast, const sf::Vector2f& rightFirst, const sf::Vector2f& rightLast);
            static bool checkSegment(const sf::Vector2f& first, const sf::Vector2f& last, const sf::Vector2f& other);
            static int checkOrientation(const sf::Vector2f& first, const sf::Vector2f& last, const sf::Vector2f& other);
            static float getDeterminant(const sf::Vector2f& left, const sf::Vector2f& right);
            static sf::Vector2f getInterpolation(const sf::Vector2f& left, const sf::Vector2f& right, float key);
            static float getProjection(const sf::Vector2f& first, const sf::Vector2f& last, const sf::Vector2f& other);
            static sf::Color hsvToRGB(sf::Vector3f color);
            static sf::Vector3f rgbToHSV(sf::Color color, bool normalize);
            float pi;
            float win;
            float winDistance;
            float winThreshold;
            float focusMoveBase;
            float focusMove;
            float focusHue;
            float timeLeft;
            float timeLeftBase;
            float fociRadius;
            float spokeRadius;
            float areaRatio;
            float temperature;
            int touches;
            unsigned int seed;
            unsigned int foci;
            bool orientation;
            sf::Vector2u resolution;
            sf::Vector2f origin;
            sf::Vector2f goalPoint;
            sf::Vector2f focusPoint;
            sf::Vector2f focusPointPrevious;
            sf::Vector2i touchLocation;
            sf::Font* font;
            sf::Text* text;
            sf::RectangleShape* areaMiddle;
            sf::RectangleShape* areaLeft;
            sf::RectangleShape* areaRight;
            sf::Drawable* target;
            sf::CircleShape* goal;
            sf::CircleShape* cursor;
            sf::CircleShape* focus;
            sf::CircleShape* fociCenter;
            sf::CircleShape* fociPerimeterReal;
            sf::CircleShape* fociControlsOutline;
            sf::RectangleShape* fociPerimeterRealOutline;
            sf::RectangleShape* goalTemperature;
            std::vector<sf::CircleShape*> fociControlsReal;
            std::vector<sf::CircleShape*> fociReals;
            std::vector<sf::CircleShape*> fociShapes;
            std::vector<sf::RectangleShape*> fociSpokes;
            std::vector<sf::RectangleShape*> fociPerimeter;
            std::vector<sf::RectangleShape*> fociControls;
            std::vector<unsigned int> fociIndices;
            std::vector<sf::Vector2f> fociPoints;
            std::function<void(const std::string&)> messenger;
            Random* random;
    };
}

#endif
