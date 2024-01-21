#include "Engine.h"
using namespace std;

// Runs the video game engine (entry point)
void Engine::runGame(int x, int y, string name) {
    // Start the window
    sf::RenderWindow window(sf::VideoMode(x, y), name);
    this->window = &window;
    view = window.getView();

    // Spawn test objects
    spawnObjects();

    // Temp Code for FPS counter
    sf::Clock clock;
    sf::Clock fps;
    sf::Time timeSinceUpdate = sf::Time::Zero;
    sf::Text counter;
    sf::Font font;
    font.loadFromFile("Arialn.ttf");
    counter.setFont(font);
    counter.setCharacterSize(24);
    counter.setFillColor(sf::Color::Cyan);

    // Engine game loop
    while (window.isOpen()) {
        sf::Time time = clock.getElapsedTime();
        clock.restart();

        timeSinceUpdate += time;
        unsigned int frames = 0;

        // Wait until time to process frame
        while (timeSinceUpdate > timePerFrame) {
            frames++;
            timeSinceUpdate -= timePerFrame;

            // Process each frame
            sf::Event event;
            while (window.pollEvent(event)) {
                processEvents(event);
            }

            // Update the screen
            update();

            // Render the fps counter
            render(fps, counter);
        }
    }

    this->window = nullptr;
}

// Spawn each object
void Engine::spawnObjects() {
    // Spawn the player
    player = new Player(sf::Vector2f(64, 64));
    objects->addToList(player);

    // Spawn walls
    int size = 50;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i % 4 > 1 && i != 0 && i != size - 1 && j != 0 && j != size - 1) {
                continue;
            }

            if (j % 4 > 1 && j != 0 && j != size - 1 && i != 0 && i != size - 1) {
                continue;
            }

            if (i == 8 && j == 8) {
                continue;
            }

            Wall* wall = new Wall(sf::Vector2f(i * 32.0f, j * 32.0f));
            objects->addToList(wall, collisionObjects->addToList(wall));
        }
    }

    // Spawn balls
    int ballNum = 1000;
    for (int i = 0; i < ballNum; i++) {
        sf::Vector2f direction;
        float radAngle = ((float) rand() / (((float) RAND_MAX) / 2.0f)) * (float) M_PI;
        direction.x = cos(radAngle);
        direction.y = sin(radAngle);
        Ball* ball = new Ball(sf::Vector2f(64, 64), direction);
        objects->addToList(ball, objects->addToList(ball));
    }

    // Spawn a tower
    Tower* tower = new Tower(sf::Vector2f(256.0f, 256.0f));
    objects->addToList(tower, collisionObjects->addToList(tower));
}

// Processes window events
void Engine::processEvents(sf::Event event) {
    if (event.type == sf::Event::Closed) {
        window->close();
    }
}

// Updates all objects for the current frame
void Engine::update() {
    // Change game speed if needed
    setGameSpeed(gameSpeed);
    float time = gameSpeed * timePerFrame.asSeconds();

    // Remake the quad tree each frame (could be improved to reuse same quad tree)
    delete collision;
    collision = new QuadTree(sf::Rect<float>(0.0f, 0.0f, 8000.0f, 8000.0f));
    for (LinkedList::Node* node = collisionObjects->head; node != nullptr; node = node->next) {
        collision->addRect(node->data);
    }

    // For each object
    LinkedList::Node* nextNode;
    for (LinkedList::Node* node = objects->head; node != nullptr; node = nextNode) {
        nextNode = node->next;
        Object* object = node->data;
        Object::ACTION action = object->update(time);

        // Perform their update action
        switch (action) {
            default:
                break;

            // Removes from list
            case Object::ACTION::REMOVE:
                objects->removeFromList(node);
                collisionObjects->removeFromList(node->collision);
                break;

            // Move the object
            case Object::ACTION::MOVE:
            {
                Entity * entity = (Entity *) object;
                float distance = entity->getSpeed() * time;
                sf::Vector2f movement(entity->getDirection());
                movement.x = movement.x * distance;
                movement.y = movement.y * distance;

                // See if a collision occured
                entity->collisionOccured(collisionController(entity, movement));
                break;
            }

            // Generate a bullet for the tower
            case Object::ACTION::GENERATE:
            {
                Tower *tower = (Tower *) object;
                objects->addToList(tower->getGenerated());
                break;
            }
        }
    }

    // Center the player
    view.setCenter(player->getSprite().getPosition());
}

// Change the game speed if a key is pressed (for debugging)
void Engine::setGameSpeed(float gameSpeed) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::J)) {
        gameSpeed = 0.5f;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {
        gameSpeed = 1.0f;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
        gameSpeed = 1.5f;
    }
    this->gameSpeed = gameSpeed;
}

// Checks the collision of an object
pair<pair<bool, bool>, sf::Vector2f>
Engine::checkCollision(Object *object, const sf::Vector2f &direction) const {
    bool collisionX = false;
    bool collisionY = false;
    sf::Rect<float> movement = object->getCollision();

    Object* xResultObj;
    movement.left = movement.left + direction.x;
    if (direction.x < 0) {
        xResultObj = collision->collisionCheck(movement, QuadTree::DIRECTION::LEFT, object);
        if (xResultObj != nullptr) {
            sf::Rect<float> xResult = xResultObj->getCollision();
            collisionX = true;
            movement.left = xResult.left + xResult.width;
        }
    } else if (direction.x > 0) {
        xResultObj = collision->collisionCheck(movement, QuadTree::DIRECTION::RIGHT, object);
        if (xResultObj != nullptr) {
            sf::Rect<float> xResult = xResultObj->getCollision();
            collisionX = true;
            movement.left = xResult.left - movement.width;
        }
    }

    Object* yResultObj;
    movement.top = movement.top + direction.y;
    if (direction.y < 0) {
        yResultObj = collision->collisionCheck(movement, QuadTree::DIRECTION::UP, object);
        if (yResultObj != nullptr) {
            sf::Rect<float> yResult = yResultObj->getCollision();
            collisionY = true;
            movement.top = yResult.top + yResult.height;
        }
    }else if (direction.y > 0) {
        yResultObj = collision->collisionCheck(movement, QuadTree::DIRECTION::DOWN, object);
        if (yResultObj != nullptr) {
            sf::Rect<float> yResult = yResultObj->getCollision();
            collisionY = true;
            movement.top = yResult.top - movement.height;
        }
    }

    sf::Vector2f total;
    total.x = movement.left - object->getCollision().left;
    total.y = movement.top - object->getCollision().top;
    return {{collisionX, collisionY}, total};
}

// Removes a node from a linked list
void Engine::LinkedList::removeFromList(Node *node) {
    if (node == nullptr) {
        return;
    }

    Node* next = node->next;
    Node* prev = node->prev;
    if (head == node) {
        head = next;
    }
    if (tail == node) {
        tail = prev;
    }
    if (node->prev != nullptr) {
        prev->next = next;
    }

    if (next != nullptr) {
        next->prev = prev;
    }

    delete node;
}

// Renders the window
void Engine::render(sf::Clock &fps, sf::Text &counter) {
    // Clear the drawn window
    window->clear(sf::Color(255, 255, 255));
    window->setView(view);

    // Set fps counter position
    sf::Vector2f counterPos = view.getCenter();
    counterPos.y += -32.0f;
    counter.setPosition(counterPos);

    // Draw each object
    for (LinkedList::Node* node = objects->head; node != nullptr; node = node->next) {
        window->draw(node->data->getSprite());
    }

    // Draw the fps counter
    sf::Time time = fps.getElapsedTime();
    fps.restart();
    counter.setString(to_string((int)(1.0f / time.asSeconds())));
    window->draw(counter);

    // Display the window
    window->display();
}

// Controls collisions for each entity that moves
pair<bool, bool> Engine::collisionController(Entity *entity, const sf::Vector2f& movement) {
    pair<pair<bool, bool>, sf::Vector2f> result = checkCollision(entity, movement);

    entity->move(result.second);

    return result.first;
}

// Adds a node to the list
Engine::LinkedList::Node * Engine::LinkedList::addToList(Object *data, Node *collision) {
    Node* newNode = new Node;
    newNode->data = data;
    if (head == nullptr) {
        head = tail = newNode;
    } else {
        tail->next = newNode;
        newNode->prev = tail;
        newNode->next = nullptr;
        tail = newNode;
    }
    newNode->collision = collision;
    return newNode;
}

// Destructor
Engine::LinkedList::~LinkedList() {
    Node* currNode = head;
    while (currNode != nullptr) {
        Node* next = currNode->next;
        delete currNode;
        currNode = next;
    }
}

// Destructor
Engine::LinkedList::Node::~Node() {
    delete data;
}