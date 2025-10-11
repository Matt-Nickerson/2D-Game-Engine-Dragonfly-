#pragma once
#include <SFML/Graphics.hpp>
#include "Manager.h"
#include "Vector.h"
#include "Color.h"  

// Defaults for SFML window.
constexpr int  WINDOW_HORIZONTAL_PIXELS_DEFAULT = 1024;
constexpr int  WINDOW_VERTICAL_PIXELS_DEFAULT = 768;
constexpr int  WINDOW_HORIZONTAL_CHARS_DEFAULT = 80;
constexpr int  WINDOW_VERTICAL_CHARS_DEFAULT = 24;
constexpr auto WINDOW_STYLE_DEFAULT = sf::Style::Titlebar;
const sf::Color WINDOW_BACKGROUND_COLOR_DEFAULT = sf::Color::Black;


enum class Justify { LEFT, CENTER, RIGHT };

class DisplayManager : public df::Manager {
private:
    DisplayManager();
    DisplayManager(const DisplayManager&) = delete;
    DisplayManager& operator=(const DisplayManager&) = delete;

    sf::Font          m_font;
    sf::RenderWindow* m_p_window{ nullptr };

    int   m_window_horizontal_pixels{ WINDOW_HORIZONTAL_PIXELS_DEFAULT };
    int   m_window_vertical_pixels{ WINDOW_VERTICAL_PIXELS_DEFAULT };
    int   m_window_horizontal_chars{ WINDOW_HORIZONTAL_CHARS_DEFAULT };
    int   m_window_vertical_chars{ WINDOW_VERTICAL_CHARS_DEFAULT };
    float m_cell_w{ 0.f };
    float m_cell_h{ 0.f };

    // helpers
    sf::Color     toSF(df::Color c) const;                      
    sf::Vector2f  gridToPixels(const Vector& grid_xy) const;
    mutable sf::Text m_text; // Reusable text object for drawing.

public:
    static DisplayManager& getInstance();

    int  startUp() override;
    void shutDown() override;

    // Draw single character at grid location with color. Return 0 ok else -1.
    int drawCh(Vector grid_pos, char ch, df::Color color) const; 

    // Without color (defaults to white).
    int drawCh(Vector grid_pos, char ch) const {
        return drawCh(grid_pos, ch, df::COLOR_DEFAULT);        
    }

    // Draw string at grid location with justification and color.
    int drawString(Vector grid_pos, const std::string& str,
        Justify just, df::Color color) const;        // <-- df::Color

    int drawString(Vector grid_pos, const std::string& str, Justify just) const {
        return drawString(grid_pos, str, just, df::COLOR_DEFAULT);
    }

    int swapBuffers();

    int getHorizontal() const { return m_window_horizontal_chars; }
    int getVertical()   const { return m_window_vertical_chars; }
    int getHorizontalPixels() const { return m_window_horizontal_pixels; }
    int getVerticalPixels()   const { return m_window_vertical_pixels; }

    sf::RenderWindow* getWindow() const { return m_p_window; }

    void setGridSize(int cols, int rows);
    void setPixelSize(int w, int h);
};
