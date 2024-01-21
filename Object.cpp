#include "Object.h"

void Player::loadTextures() {
    sf::Texture playerTexture;
    playerTexture.loadFromFile("./testSprites/joe.png");
    textures.push_back(playerTexture);
    sprite.setTexture(textures.at(0));
}

Player::Player(sf::Vector2f location) : Entity(location, 32.0f, 32.0f) {
    loadTextures();
    speedStat = 200.0f;
}

sf::Vector2f Object::getLocation() {
    return location;
}

void Object::setLocation(sf::Vector2f location) {
    this->location = location;
}

sf::Rect<float> Object::getCollision() {
    return collisionBox;
}

void Object::setCollision(sf::Rect<float> collisionBox) {
    this->collisionBox = collisionBox;

}

Object::Object(sf::Vector2f location, float collisionWidth, float collisionHeight) {
    this->location = location;

    collisionBox.left = location.x;
    collisionBox.top = location.y;
    collisionBox.width = collisionWidth;
    collisionBox.height = collisionHeight;

    sprite.setPosition(location);
}

sf::Sprite Object::getSprite() {
    return sprite;
}

void Object::setSprite(sf::Sprite sprite) {
    this->sprite = sprite;
}

vector<sf::Texture> Object::getTextures() {
    return textures;
}

bool Object::checkCollision(Object * object) {
    return this != object;
}

float Entity::getSpeed() {
    return speedStat;
}

void Entity::setSpeed(float speed) {
    if (speed >= 0) {
        speedStat = speed;
    }
}

Entity::Entity(sf::Vector2f location, float collisionWidth, float collisionHeight) : Object(location, collisionWidth, collisionHeight) {

}

void Entity::move(sf::Vector2f movement) {
    sprite.move(movement);
    collisionBox.left += movement.x;
    collisionBox.top += movement.y;
}

sf::Vector2f Entity::getDirection() {
    return direction;
}

void Entity::setDirection(sf::Vector2f direction) {
    this->direction = direction;
}

void Entity::collisionOccured(pair<bool, bool> collisionDirection) {
    return;
}

Wall::Wall(sf::Vector2f location) : CollisionElement(location, 32.0f, 32.0f) {
    loadTextures();
    collision = true;
}

void Wall::loadTextures() {
    sf::Texture wallTexture;
    wallTexture.loadFromFile("./testSprites/AWALL.png");
    textures.push_back(wallTexture);
    sprite.setTexture(textures.at(0));
}

Object::ACTION Wall::update(float time) {
    return WAIT;
}

void Ball::loadTextures() {
    sf::Texture texture;
    texture.loadFromFile("./testSprites/ball.png");
    textures.push_back(texture);
    sprite.setTexture(textures.at(0));
}

Ball::Ball(sf::Vector2f location, sf::Vector2f direction) : Entity(location, 8.0f, 8.0f) {
    loadTextures();
    speedStat = 100.0f;
    this->direction = direction;
    collision = true;
}

CollisionElement::CollisionElement(sf::Vector2f location, float collisionWidth, float collisionHeight)
        : MapElement(location, collisionWidth, collisionHeight) {

}

MapElement::MapElement(sf::Vector2f location, float collisionWidth, float collisionHeight) : Object(location, collisionWidth, collisionHeight) {

}

void Ball::collisionOccured(pair<bool, bool> collisionDirection) {
    if (collisionDirection.first) {
        direction.x = -1.0 * direction.x;
    }

    if (collisionDirection.second) {
        direction.y = -1.0 * direction.y;
    }
}

Object::ACTION Ball::update(float time) {
    return MOVE;
}

Object::ACTION Player::update(float time) {
    sf::Vector2f direction(0.0f, 0.0f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        direction.x += -1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        direction.x += 1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        direction.y += -1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        direction.y += 1.0f;
    }
    this->direction = direction;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::PageDown)) {
        setSpeed(speedStat - 100);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::PageUp)) {
        setSpeed(speedStat + 100);
    }

    return MOVE;
}

bool Player::checkCollision(Object *object) {
    return dynamic_cast<Ball*>(object) == nullptr;
}

void Bullet::loadTextures() {
    sf::Texture texture;
    texture.loadFromFile("./testSprites/bullet.png");
    textures.push_back(texture);
    sprite.setTexture(textures.at(0));
}

Bullet::Bullet(sf::Vector2f location, sf::Vector2f direction, Object* parent) : Entity(location, 8.0f, 8.0f) {
    loadTextures();
    speedStat = 100.0f;
    this->direction = direction;
    collision = true;
    this->parent = parent;
}

void Bullet::collisionOccured(pair<bool, bool> collisionDirection) {
    if (collisionDirection.first || collisionDirection.second) {
        exists = false;
    }
}

Object::ACTION Bullet::update(float time) {
    if (exists) {
        return ACTION::MOVE;
    } else {
        return ACTION::REMOVE;
    }
}

bool Bullet::checkCollision(Object* object) {
    return object != parent;
}

void Tower::loadTextures() {
    sf::Texture texture;
    texture.loadFromFile("./testSprites/tower.png");
    textures.push_back(texture);
    sprite.setTexture(textures.at(0));
}

Tower::Tower(sf::Vector2f location) : Entity(location, 32.0f, 32.0f) {
    loadTextures();
    speedStat = 0.0f;
    direction = sf::Vector2f();
    collision = true;
}

void Tower::collisionOccured(pair<bool, bool> collisionDirection) {
}

Object::ACTION Tower::update(float time) {
    frameCounter += time;
    if (frameCounter > frameDelay) {
        frameCounter = 0;
        shoot();
        return ACTION::GENERATE;
    } else {
        return ACTION::WAIT;
    }
}

void Tower::shoot() {
    angleRad = angleRad + M_PI_4;
    if (angleRad > M_2_PI) {
        angleRad -= M_2_PI;
    }
    direction.x = cos(angleRad);
    direction.y = sin(angleRad);
    generated = new Bullet(location + sf::Vector2f(12.0f, 12.0f), direction, this);
}

Bullet *Tower::getGenerated() {
    return generated;
}
