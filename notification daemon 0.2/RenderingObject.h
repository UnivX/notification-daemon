#pragma once
#include <SFML/Graphics.hpp>

//TODO implement father/child position system (local position)

/*
this is a pure virtual class, is the base for rendering objects
all the memory gestion is responsibility of the derived class
*/
enum ObjectType
{
	ObjSprite,
	ObjText,
	ObjRectangle,
	ObjCircle,
};

class RenderingObject {
public:
	RenderingObject();
	~RenderingObject();
	virtual sf::Drawable* GetDrawableObject() = 0;
	virtual sf::Transformable* GetTransformableObject() = 0;
	virtual RenderingObject* CreateDynamicCopy() = 0;
	std::string GetName();
	void SetName(std::string name);
	virtual ObjectType GetType() = 0;
protected:
	std::string name;
};
/*
this class has some references to RenderingOBjects but it's not the owner
*/
class Layer{
public:
	Layer();
	void UpdatePosition();
	void AddObject(RenderingObject* object,sf::Vector2f localPosition);
	void SetObjectLocalPosition(RenderingObject* object, sf::Vector2f localPosition);
	sf::Vector2f GetObjectLocalPosition(RenderingObject* object);
	sf::Vector2f GetPosition();
	void SetPosition(sf::Vector2f position);
	std::vector<std::pair<RenderingObject*, sf::Vector2f>>* GetVector();
private:
	sf::Vector2f position;
	std::vector<std::pair<RenderingObject*, sf::Vector2f>> vector;//vector cointain objects paired with the respective local position
};

class SpriteObject : public RenderingObject {
public:
	SpriteObject(sf::Texture* texture);
	sf::Drawable* GetDrawableObject();
	sf::Transformable* GetTransformableObject();
	void SetColor(sf::Color color);
	RenderingObject* CreateDynamicCopy() { return new SpriteObject(*this); };
	ObjectType GetType() { return ObjectType::ObjSprite; };
private:
	sf::Sprite sprite;
};

class TextObject : public RenderingObject {
public:
	TextObject(sf::Font* font);
	sf::Drawable* GetDrawableObject();
	sf::Transformable* GetTransformableObject();
	void SetCharacterSize(unsigned int size);
	void SetString(std::string str);
	void SetOutLineThickness(float thickness);
	void SetFillColor(sf::Color color);
	void SetOutLineColor(sf::Color color);
	RenderingObject* CreateDynamicCopy() { return new TextObject(*this); };
	ObjectType GetType() { return ObjectType::ObjText; };
private:
	sf::Text text;
};

class RectangleObject : public RenderingObject {
public:
	RectangleObject();
	sf::Drawable* GetDrawableObject();
	sf::Transformable* GetTransformableObject();
	void SetSize(sf::Vector2f size);
	void SetOutLineThickness(float thickness);
	void SetFillColor(sf::Color color);
	void SetOutLineColor(sf::Color color);
	RenderingObject* CreateDynamicCopy() { return new RectangleObject(*this); };
	ObjectType GetType() { return ObjectType::ObjRectangle; };
private:
	sf::RectangleShape shape;
};

class CircleObject : public RenderingObject {
public:
	CircleObject();
	sf::Drawable* GetDrawableObject();
	sf::Transformable* GetTransformableObject();
	void SetSize(float radius);
	void SetOutLineThickness(float thickness);
	void SetFillColor(sf::Color color);
	void SetOutLineColor(sf::Color color);
	RenderingObject* CreateDynamicCopy() { return new CircleObject(*this); };
	ObjectType GetType() { return ObjectType::ObjCircle; };
private:
	sf::CircleShape shape;
};