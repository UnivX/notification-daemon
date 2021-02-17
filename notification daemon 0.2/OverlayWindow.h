#pragma once
#include <SFML/Graphics.hpp>

#ifndef SFML_SYSTEM_LINUX
#include <windows.h>
#else
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#endif


#ifdef SFML_SYSTEM_LINUX
bool setTransparency(Window wnd, unsigned char alpha);
#endif
class OverlayWindow
{
public:
	OverlayWindow(sf::Vector2i pos, sf::Vector2i size, float alpha = 255);
	~OverlayWindow();
	void clear();
	void display();
	void draw(const sf::Drawable& obj);
	sf::RenderWindow* getWindow();
	void setPosition(sf::Vector2i pos);
	void SetSize(sf::Vector2i size);
private:
	sf::Vector2i pos, size;
	sf::RenderWindow* window;
#ifndef SFML_SYSTEM_LINUX
	HWND hwnd;
#endif
};

