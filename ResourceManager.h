#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "Utilities.h"
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>

template <class Derived, typename T>
class ResourceManager
{
public:
	ResourceManager(const std::string& path) {
		loadPath(path);
	}
	virtual ~ResourceManager() {
		purgeResources();
		m_paths.clear();
	}

	void purgeResources()
	{
		for (auto &i : m_resources) {
			delete i.second.first;
			i.second.first = nullptr;
		}
		m_resources.clear();
	}

	bool requireResource(const std::string& id)
	{
		auto res = find(id);
		if (res) {
			res->second++;
			return true;
		}
		auto iter = m_paths.find(id);
		if (iter != m_paths.end())
		{
			T* resource = load(iter->second);
			if (resource)
			{
				m_resources.emplace(id, std::make_pair(resource, 1));
				return true;
			}
		}
		return false;
	}

	T* getResource(const std::string& id)
	{
		auto res = find(id);
		return (res ? res->first : nullptr);
	}

	std::string& getPath(const std::string& path)
	{
		for (auto cIter = m_paths.cbegin(); cIter != m_paths.cend(); cIter++)
		{
			if (cIter->second == path)
			{
				return cIter->second;
			}
		}
	}

	bool releaseResource(const std::string& id)
	{
		auto res = find(id);
		if (res)
		{
			res->second--;
			if (res->second == 0)
			{
				destroyResource(id);
			}
			return true;
		}
		return false;
	}

protected:
	T* load(const std::string& path)
	{
		return static_cast<Derived*>(this)->load(path);
	}

private:
	std::unordered_map<std::string, std::pair<T*, unsigned int>> m_resources;
	std::unordered_map<std::string, std::string> m_paths;

	std::pair<T*, unsigned int>* find(const std::string& id)
	{
		auto iter = m_resources.find(id);
		return (iter != m_resources.end() ? &iter->second : nullptr);
	}

	void destroyResource(const std::string& id)
	{
		auto iter = m_resources.find(id);
		if (iter != m_resources.end())
		{
			delete iter->second.first;
			iter->second.first = nullptr;
			m_resources.erase(iter);
		}
	}

	bool loadPath(const std::string path)
	{
		std::ifstream file;
		file.open(Utils::GetWorkingDirectory() + path);
		if (file.is_open())
		{
			std::string line;
			while (std::getline(file, line))
			{
				std::stringstream keyStream;
				std::string path;
				std::string pathName;
				keyStream >> path;
				keyStream >> pathName;
				m_paths[path] = pathName;

			}
			return true;
		}
		return false;
	}
};
#endif // !RESOURCE_MANAGER_H
