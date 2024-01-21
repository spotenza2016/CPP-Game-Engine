#pragma once
#include "Object.h"
#include "QuadTree.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>

class Engine {
    // 60 FPS
    const sf::Time timePerFrame = sf::seconds(1.0f / 60.0f);
    float gameSpeed = 1.0f;

    // Collision Vars
    enum DIRECTION {LEFT = 0, RIGHT = 1, UP = 2, DOWN = 3};
    QuadTree* collision = new QuadTree(sf::Rect<float>(0.0f, 0.0f, 8000.0f, 8000.0f));
    pair<bool, bool> collisionController(Entity *entity, const sf::Vector2f& movement);

    // Window Vars
    sf::RenderWindow* window = nullptr;
    sf::View view;
    Player* player;

    // Functions
    void spawnObjects();
    void processEvents(sf::Event event);
    void update();
    void setGameSpeed(float gameSpeed);
    pair<pair<bool, bool>, sf::Vector2f> checkCollision(Object *object, const sf::Vector2f &direction) const;
    void render(sf::Clock &fps, sf::Text &counter);

    // Standard linked list to hold objects
    struct LinkedList {
        struct Node {
            Node* next;
            Node* prev;
            Object* data;
            Node* collision;
            ~Node();
        };
        Node* head = nullptr;
        Node* tail = nullptr;
        ~LinkedList();
        Node * addToList(Object *data, Node *collision = nullptr);
        void removeFromList(Node* node);
    };
    LinkedList* objects = new LinkedList;
    LinkedList* collisionObjects = new LinkedList;

public:
    void runGame(int x = 800, int y = 800, string name = "Test Window");
};