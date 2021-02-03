#include "OverlayWindow.h"



OverlayWindow::OverlayWindow(sf::Vector2i pos, sf::Vector2i size, float alpha)
{
	this->pos = pos;
	this->size = size;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 16;
	this->window = new sf::RenderWindow(sf::VideoMode(size.x, size.y, 32), "Transparent Window", sf::Style::None, settings);
	this->hwnd = GetActiveWindow();  // Win32 API
	//set transparency
	SetWindowLongA(this->hwnd, GWL_EXSTYLE, GetWindowLong(this->hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_NOACTIVATE);
	SetLayeredWindowAttributes(this->hwnd, RGB(0, 0, 0), alpha, LWA_ALPHA | LWA_COLORKEY);
}


OverlayWindow::~OverlayWindow()
{
	if (this->window != nullptr) {
		this->window->close();
		delete this->window;
	}
}

void OverlayWindow::clear()
{
	this->window->clear(sf::Color::Transparent);
}

void OverlayWindow::display()
{
	SetWindowPos(this->hwnd, HWND_TOPMOST, this->pos.x, this->pos.y, this->size.x, this->size.y, SWP_NOACTIVATE);
	this->window->display();
}

void OverlayWindow::draw(const sf::Drawable & obj)
{
	this->window->draw(obj);
}

sf::RenderWindow * OverlayWindow::getWindow()
{
	return this->window;
}

void OverlayWindow::setPosition(sf::Vector2i pos)
{
	this->pos = pos;
}

void OverlayWindow::SetSize(sf::Vector2i size)
{
	this->size = size;
}
