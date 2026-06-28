#include "Player.h"
#include <iostream>
using namespace std;

Player::Player(string n) {
    name = n;
    role = "";
    eliminated = false;
}

void Player::setName(string n) {
    name = n;
}

string Player::getName() {
    return name;
}

void Player::assignRole(string r) {
    role = r;
}

string Player::getRole() {
    return role;
}

void Player::eliminate() {
    eliminated = true;
}

bool Player::isEliminated() {
    return eliminated;
}

