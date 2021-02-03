#pragma once
#include <string>
#include <memory>
#include <vector>
#include <atomic>
#include <mutex>

//all the resources must be thread safe because the data will be acceded mukti thread

//base class
/*
this is the base class of the resource owners
*/
class Resource {
public:
	std::string GetName();
protected:
	void SetName(std::string name);
private:
	std::string m_name;
	std::mutex mutex;
};

//list of resources
/*
this is a list of resource, all the resource are dynamicaly allocated with shared pointers
*/
class ResourceList {
public:
	std::shared_ptr<Resource> SearchResource(std::string name);
	void AddResource(std::shared_ptr<Resource> resource);
	void RemoveResource(std::shared_ptr<Resource> resource);
	std::string GetName();
	void SetName(std::string name);
	std::vector<std::shared_ptr<Resource>> GetList();
private:
	std::string m_name;
	std::vector<std::shared_ptr<Resource>> resources;
	std::mutex nameMutex, resourcesMutex;
};
/*
this is basicaly a list of resourceList
this class is where all resources of the program are allocated
the class is based on the Singleton code pattern
*/
class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();
	ResourceList* GetListByName(std::string name);
	ResourceList* AddNewList(std::string name);
	static ResourceManager* GetInstance();
	std::vector<ResourceList*> GetList();
private:
	std::mutex listMutex;
	static ResourceManager* instance;
	std::vector<ResourceList*> list;
};

