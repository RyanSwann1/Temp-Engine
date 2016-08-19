#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H
#include "ResourceManager.h"
#include "Utilities.h"
#include <SFML\Graphics.hpp>
class TextureManager :
	public ResourceManager<TextureManager, sf::Texture> 
{
public:
	TextureManager() 
		: ResourceManager("Textures.cfg") {}

	~TextureManager() {}

	sf::Texture* load(const std::string& path)
	{
		sf::Texture* texture = new sf::Texture();
		if (!texture->loadFromFile(Utils::GetWorkingDirectory() + path))
		{
			std::cerr << "Couldn't load path: " << path << "\n";
			delete texture;
			texture = nullptr;
		}
		else
		{
			return texture;
		}
	}
};
#endif // !TEXTURE_MANAGER_H
