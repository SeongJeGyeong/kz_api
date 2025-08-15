#pragma once

class Component;

class ComponentContainer
{
private:
	map<size_t, Component*> _componentMap;
	map<uint32, Component*> _priorityGroups;

public:
	template<typename T>
	void AddComponent()
	{
		auto component = new T();
		_componentMap.insert(make_pair(T::StaticClass(), component));
		_priorityGroups[component->GetPriority()] = component;
	}

	template<typename T>
	T* GetComponent()
	{
		auto it = _componentMap.find(T::StaticClass());
		if (it != _componentMap.end())
		{
			return static_cast<T*>(it->second);
		}

		return nullptr;
	}

	void UpdateComponents(float deltaTime);
	void RenderComponents(HDC hdc);
};

