#include <iostream>
#include <optional>
#include <vector>

#include <SFML/Graphics.hpp>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int FPS_LIMIT = 30;
const int FPA = 90;
static int frame_counter = 0;

using Point2D = sf::Vector2f;

// (Part 1) Define a function that samples a cubic Bezier curve at t in [0, 1].
Point2D getPoint(const std::vector<sf::Vector2f>& pts, float t) {
    float k = (1 - t);

    float b1 = k * k * k;
    float b2 = k * k * 3 * t;
    float b3 = k * 3 * t * t;
    float b4 = t * t * t;

    Point2D bezier = pts[0] * b1 + pts[1] * b2 + pts[2] * b3 + pts[3] * b4;

    return bezier;
}

// (Part 2) Define a function that returns the curve's slope at t in [0, 1].
Point2D getSlope(const std::vector<sf::Vector2f>& pts, float t) {
    float k = (1 - t);

    float b1 = 3 * k * k;
    float b2 = 6 * k * t;
    float b3 = 3 * t * t;

    Point2D bezier_slope = b1 * (pts[1] - pts[0]) + b2 * (pts[2] - pts[1])  + b3 * (pts[3] - pts[2]);

    return bezier_slope;
 }

// (Part 1) Store four control points for the curve.
Point2D p0(100, 400);
Point2D p1(100, 200);
Point2D p2(700, 300);
Point2D p3(700, 400);

std::vector<sf::Vector2f> points = {p0, p1, p2, p3};
// (Part 2) Track animation time for the square moving along the curve.
float animation_time;
// (Part 3) Track the index of the control point being dragged.
int grabbed_point = -1;

void handleInput(sf::Window& window, bool& shouldQuit) {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            shouldQuit = true;
        } else if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
            // (Part 3) On left-click, select the closest control point
            // using mouse->position and start dragging it.
            switch (mouse->button) {
                case sf::Mouse::Button::Right:
                    break;
                case sf::Mouse::Button::Left: {
                    if (grabbed_point > -1) {
                        break;
                        //points[grabbed_point] = sf::Vector2f(mouse->position);
                    }
                    float closest_distance = std::numeric_limits<float>::max();
                    for (int i = 0; i < points.size(); ++i) {
                        Point2D temp = points[i] - sf::Vector2f(mouse->position);
                        float distance = temp.x * temp.x + temp.y * temp.y;

                        if (distance < closest_distance) {
                            closest_distance = distance;
                            grabbed_point = i;
                        }
                    }
                }
                    break;
                case sf::Mouse::Button::Middle:
                    break;
                default:
                    break;
            }
        } else if (const auto* mouse = event->getIf<sf::Event::MouseButtonReleased>()) {
            // (Part 3) On left-button release, stop dragging.
            switch (mouse->button) {
                case sf::Mouse::Button::Left:
                    grabbed_point = -1;
                    break;
                default:
                    break;
            }
        } else if (const auto* mouse = event->getIf<sf::Event::MouseMoved>()) {
            if (grabbed_point == -1) {
                continue;
            }
            points[grabbed_point] = sf::Vector2f(mouse->position);

            // (Part 3) Move the selected control point to mouse->position.
            // TODO: (Part 4) Maintain matching slopes at shared endpoints.
            // When moving point 3, move point 5 without changing its distance
            // from point 4 (point numbers here start at 1).
        } else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            // (Part 4) '+' adds three control points; '-' removes three,
            // keeping at least four points.
            switch (key->code) {
                case sf::Keyboard::Key::Add:
                    for (int i = 0; i < 3; ++i) {
                        Point2D temp(points[points.size() - 1].x + 50, points[points.size() - 1].y + 50);
                        points.push_back(temp);
                    }
                    break;
                case sf::Keyboard::Key::Subtract:
                    for (int i = 0; i < 3; ++i) {
                        if (points.size() > 4) {
                            points.pop_back();
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

void render(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    // ====== ====== ======
    // (Part 1) Sample GetPoint over t in [0, 1] and connect samples using the line-drawing
    // code from your project. Draw all four control points as circles after drawing the curve.
    // ====== ====== ======

    if (points.size() < 4) {
        window.display();
        return;
    }
    sf::VertexArray bezier_curve(sf::PrimitiveType::LineStrip, 101);

    for (int i = 0; i <= 100; ++i) {
        float t = static_cast<float>(i) / 100.f;
        Point2D current_point = getPoint(points, t);
        bezier_curve[i].position = current_point;
        bezier_curve[i].color = sf::Color::White;
    }

    window.draw(bezier_curve);

    for (const auto& p : points) {
        sf::CircleShape circle(15.f);
        circle.setOrigin({15.f, 15.f});
        circle.setPosition(p);
        circle.setFillColor(sf::Color::Green);
        window.draw(circle);
    }


    // ====== ====== ======
    // (Part 2) Draw a small square moving repeatedly along the curve.
    // Use GetSlope to orient it to the curve at each time step.
    // ====== ====== ======
    animation_time = (frame_counter % FPA) / static_cast<float>(FPA);
    sf::RectangleShape square({15.f, 15.f});
    square.setOrigin({7.5f, 7.5f});
    Point2D square_angle = getSlope(points, animation_time);
    Point2D square_position = getPoint(points, animation_time);
    square.setPosition({square_position.x, square_position.y});
    square.setFillColor(sf::Color::Magenta);
    square.setRotation(square_angle.angle());
    window.draw(square);
    frame_counter++;

    //(Part 3) Draw control handles from point 1 to 2 and point 3 to 4.
    sf::VertexArray control_handle1(sf::PrimitiveType::LineStrip, 101);
    sf::VertexArray control_handle2(sf::PrimitiveType::LineStrip, 101);


    for (int i = 0; i <= 100; ++i) {
        //Update Code
        float t = static_cast<float>(i) / 100.f;
        Point2D current_point1 = points[0] + (points[1] - points[0]) * t;
        control_handle1[i].position = current_point1;
        control_handle1[i].color = sf::Color::Red;

        Point2D current_point2 = points[2] + (points[3] - points[2]) * t;
        control_handle2[i].position = current_point2;
        control_handle2[i].color = sf::Color::Red;
    }
    window.draw(control_handle1);
    window.draw(control_handle2);
    // TODO: (Part 4) Draw all connected cubic Bezier segments and their handles.
    // ====== ====== ======

    // ====== ====== ======
    // TODO: (Bonus) Support multiple curves, a Galaga screen overlay at a 1:2 ratio, and exporting
    // curve points as C++ code for Project 1b.
    // ====== ====== ======

    window.display();
}

int main() {
    sf::RenderWindow window;

    try {
        // Initialize window
        window.create(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Bezier Curve Editor");
        window.setFramerateLimit(FPS_LIMIT);
        // Prevent key repeats.
        window.setKeyRepeatEnabled(false);

        bool shouldQuit = false;
        // Main game loop
        while (window.isOpen()) {
            handleInput(window, shouldQuit);
            if (shouldQuit) {
                break;
            }
            render(window);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
