#include "QuadTree.h"

void QuadTree::addRect(Object *object) {
    size++;
    addRectHelper(root, object, 0);
}

void QuadTree::addRectHelper(Node *currNode, Object *object, int divisions) {
    sf::Rect<float> rect = object->getCollision();
    if (!currNode->leaf) {
        for (int i = 0; i < 4; i++) {
            Node* child = currNode->children.at(i);

            if (child->region.intersects(rect)) {
                addRectHelper(child, object, divisions + 1);
            }
        }

    } else {
        addToLeaf(currNode, object, divisions);
    }
}

pair<float, Object *>
QuadTree::collisionHelper(Node *currNode, const sf::Rect<float> &movement, QuadTree::DIRECTION direction,
                          Object *object) const {
    if (currNode == nullptr) {
        return {0.0f, nullptr};
    }
    if (!currNode->leaf) {
        pair<float, Object*> minimum(0.0f, nullptr);
        for (int i = 0; i < 4; i++) {
            Node* child = currNode->children.at(i);

            if (child->region.intersects(movement)) {
                pair<float, Object*> result = collisionHelper(child, movement, direction, object);
                if ((minimum.second == nullptr || result.first < minimum.first) && result.second != nullptr) {
                    minimum = result;
                }
            }
        }
        return minimum;
    } else {
        pair<float, Object*> minimum;
        minimum.first = 0.0f;
        minimum.second = nullptr;
        for (int i = 0; i < currNode->size; i++) {
            Object* result = currNode->contains.at(i);
            sf::Rect<float> resultRect = result->getCollision();
            if (resultRect.intersects(movement) && object->checkCollision(currNode->contains.at(i))) {
                float resultDistance;

                switch (direction) {
                    case QuadTree::DIRECTION::LEFT:
                        resultDistance = -1.0 * (resultRect.left + resultRect.width);
                        break;
                    case QuadTree::DIRECTION::RIGHT:
                        resultDistance = resultRect.left;
                        break;
                    case QuadTree::DIRECTION::UP:
                        resultDistance = -1.0 * (resultRect.top + resultRect.height);
                        break;
                    case QuadTree::DIRECTION::DOWN:
                        resultDistance = resultRect.top;
                        break;
                }

                if (minimum.second == nullptr || resultDistance < minimum.first) {
                    minimum.first = resultDistance;
                    minimum.second = result;
                }

            }
        }
        return minimum;
    }
}

Object *
QuadTree::collisionCheck(const sf::Rect<float> &movement, QuadTree::DIRECTION direction, Object* object) const {
    if (root == nullptr) {
        return nullptr;
    }
    else if (movement.intersects(root->region)) {
        return collisionHelper(root, movement, direction, object).second;
    }
}

QuadTree::QuadTree(const sf::Rect<float> &rect) {
    Node* node = new Node;
    root = node;
    node->leaf = true;
    node->region = rect;
}

QuadTree::~QuadTree() {
    destructorHelper(root);
}

void QuadTree::destructorHelper(QuadTree::Node *currNode) {
    if (currNode != nullptr) {
        if (!currNode->leaf) {
            for (int i = 0; i < 4; i++) {
                destructorHelper(currNode->children.at(i));
            }
        }
        delete currNode;
    }
}

void QuadTree::levelOrder() {
    if (root == nullptr) {
        return;
    }
    queue<Node*> nodes;
    nodes.push(root);
    int level = 0;
    cout << size << " size with division of " << divisionMax << endl;
    cout << "Level Order:" << endl;
    while (!nodes.empty()) {
        cout << "Level " << level++ << ":" << endl;
        int size = nodes.size();
        for (int i = 0; i < size; i++) {
            Node* curr = nodes.front();

            cout << curr->size << " ";

            for (int j = 0; j < curr->children.size(); j++) {
                Node* child = curr->children.at(j);
                if (child != nullptr) {
                    nodes.push(curr->children.at(j));
                }
            }
            nodes.pop();
        }
        cout << endl;
    }
}

void QuadTree::addToLeaf(QuadTree::Node *currNode, Object *object, int divisions) {
    currNode->size++;
    currNode->contains.push_back(object);
    if (divisions < divisionMax && currNode->size > bucketMax) {
        currNode->leaf = false;
        for (int i = 0; i < 4; i++) {
            Node* child = new Node;
            currNode->children.at(i) = child;
            child->leaf = true;

            sf::Rect<float> region;
            region.height = currNode->region.height / 2;
            region.width = currNode->region.width / 2;

            switch (i) {
                case 0:
                    region.top = currNode->region.top;
                    region.left = currNode->region.left + region.width;
                    break;
                case 1:
                    region.left = currNode->region.left;
                    region.top = currNode->region.top;
                    break;
                case 2:
                    region.top = currNode->region.top + region.height;
                    region.left = currNode->region.left;

                    break;
                case 3:
                    region.top = currNode->region.top + region.height;
                    region.left = currNode->region.left + region.width;
                    break;
            }
            child->region = region;

            for (int i = 0; i < currNode->size; i++) {
                if (currNode->contains.at(i)->getCollision().intersects(child->region)) {
                    child->contains.push_back(currNode->contains.at(i));
                    child->size++;
                }
            }
        }
        currNode->size = 0;
        currNode->contains.clear();
    }
}
