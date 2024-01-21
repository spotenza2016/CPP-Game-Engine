#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <queue>
#include <cmath>
#include "Object.h"
using namespace std;

class QuadTree {
public:
    enum DIRECTION {LEFT = 0, RIGHT = 1, UP = 2, DOWN = 3};
private:
    struct Node {
        vector<Node*> children;
        vector<Object*> contains;
        sf::Rect<float> region;
        int size;
        bool leaf;
        Node() {
            children.push_back(nullptr);
            children.push_back(nullptr);
            children.push_back(nullptr);
            children.push_back(nullptr);
            size = 0;
        }
    };
    Node* root = nullptr;
    int bucketMax = 8;
    int divisionMax = 100000;
    int size = 0;
    void addRectHelper(Node *currNode, Object *object, int divisions);
    void addToLeaf(Node* currNode, Object *object, int divisions);
    pair<float, Object *>
    collisionHelper(Node *currNode, const sf::Rect<float> &movement, QuadTree::DIRECTION direction,
                    Object *object) const;
public:
    void addRect(Object *object);
    Object * collisionCheck(const sf::Rect<float> &movement, QuadTree::DIRECTION direction, Object* object) const;

    QuadTree(const sf::Rect<float> &rect);

    ~QuadTree();
    void destructorHelper(Node* currNode);

    void levelOrder();
};