#include "pch.h"
#include "ComponentContainer.h"
#include "../Components/Component.h"

void ComponentContainer::UpdateComponents(float deltaTime)
{
	//for (auto it : _componentMap)
	//{
	//	it.second->UpdateComponent(deltaTime);
	//}

	for (auto& [priority, components] : _priorityGroups)
	{
		components->UpdateComponent(deltaTime);
	}
}

void ComponentContainer::RenderComponents(HDC hdc)
{
	for (auto it : _componentMap)
	{
		it.second->RenderComponent(hdc);
	}
}
