#include "OverlayWindow.h"

#ifdef SFML_SYSTEM_LINUX
bool setTransparency(Window wnd, unsigned char alpha)
{
    Display* display = XOpenDisplay(NULL);
    unsigned long opacity = (0xffffffff / 0xff) * alpha;
    Atom property = XInternAtom(display, "_NET_WM_WINDOW_OPACITY", false);
        if (property != None)
        {
            XChangeProperty(display, wnd, property, XA_CARDINAL, 32, PropModeReplace, (unsigned char*)&opacity, 1);
            XFlush(display);
            XCloseDisplay(display);
            return true;
        }
        else
        {
            XCloseDisplay(display);
            return false;
        }
    }
#endif

OverlayWindow::OverlayWindow(sf::Vector2i pos, sf::Vector2i size, float alpha)
{
	this->pos = pos;
	this->size = size;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 16;
	this->window = new sf::RenderWindow(sf::VideoMode(size.x, size.y, 32), "Transparent Window", 0L, settings);
#ifndef SFML_SYSTEM_LINUX
	this->hwnd = GetActiveWindow();  // Win32 API
	//set transparency
	SetWindowLongA(this->hwnd, GWL_EXSTYLE, GetWindowLong(this->hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_NOACTIVATE);
	SetLayeredWindowAttributes(this->hwnd, RGB(0, 0, 0), alpha, LWA_ALPHA | LWA_COLORKEY);
#else
	setTransparency(this->window->getSystemHandle(), alpha);
#endif
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
#ifndef SFML_SYSTEM_LINUX
	SetWindowPos(this->hwnd, HWND_TOPMOST, this->pos.x, this->pos.y, this->size.x, this->size.y, SWP_NOACTIVATE);
#else
	Display* display = XOpenDisplay(NULL);
    XCloseDisplay(display);
	XRaiseWindow(display, this->window->getSystemHandle());
#endif
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
