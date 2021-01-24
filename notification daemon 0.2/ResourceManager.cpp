#include "ResourceManager.h"

ResourceManager* ResourceManager::instance = nullptr;

ResourceManager::ResourceManager()
{
}


ResourceManager::~ResourceManager()
{
}

ResourceList* ResourceManager::GetListByName(std::string name)
{
	std::lock_guard<std::mutex> l(this->listMutex);
	for (int i = 0; i < this->list.size(); i++) {
		if (this->list[i]->GetName() == name)//search in the vector for the list that has the correct name
			return this->list[i];
	}
	return nullptr;
}

ResourceList* ResourceManager::AddNewList(std::string name)
{
	std::lock_guard<std::mutex> l(this->listMutex);
	ResourceList* rList = new ResourceList();
	rList->SetName(name);
	this->list.push_back(rList);
	return rList;
}

ResourceManager* ResourceManager::GetInstance()
{
	if (ResourceManager::instance == nullptr)
		ResourceManager::instance = new ResourceManager();
	return ResourceManager::instance;
}

std::string Resource::GetName()
{
	std::lock_guard<std::mutex> l(this->mutex);
	return this->m_name;
}

void Resource::SetName(std::string name)
{
	std::lock_guard<std::mutex> l(this->mutex);
	this->m_name = name;
}

std::shared_ptr<Resource> ResourceList::SearchResource(std::string name)
{
	std::lock_guard<std::mutex> l(this->resourcesMutex);
	std::shared_ptr<Resource> resource = nullptr;
	for (int i = 0; i < this->resources.size(); i++) {
		if (this->resources[i]->GetName() == name)
			resource = this->resources[i];
	}
	return resource;
}

void ResourceList::AddResource(std::shared_ptr<Resource> resource)
{
	std::lock_guard<std::mutex> l(this->resourcesMutex);
	this->resources.push_back(resource);
}

void ResourceList::RemoveResource(std::shared_ptr<Resource> resource)
{
	std::lock_guard<std::mutex> l(this->resourcesMutex);
	for (int i = 0; i < this->resources.size(); i++) {
		if (this->resources[i]->GetName() == resource->GetName())
			this->resources.erase(this->resources.begin() + i);
	}
}

std::string ResourceList::GetName()
{
	std::lock_guard<std::mutex> l(this->nameMutex);
	return this->m_name;
}

void ResourceList::SetName(std::string name)
{
	std::lock_guard<std::mutex> l(this->nameMutex);
	this->m_name = name;
}
