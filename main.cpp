#include "Admin.h"
#include "Game.h"

int main() {
    Admin admin;
    while (true) {
        // Must log in or register before playing
        if (!admin.showAuthMenu()) {
            return 0;
        }

        Game game;
        game.playGame();
    }

    return 0;
}
