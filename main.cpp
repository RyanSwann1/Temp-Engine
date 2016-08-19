#include "Game.h"

int main()
{
	//Initialize and enter game loop
	Game game("Mini SFML Engine", sf::Vector2u(640, 480));
	while (game.isRunning())
	{
		game.update();
		game.render();
		game.lateUpdate();
	}
	
	return 0;
}