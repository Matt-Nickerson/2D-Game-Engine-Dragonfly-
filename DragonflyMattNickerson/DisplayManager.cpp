#include "DisplayManager.h"
#include "LogManager.h"
#include "Color.h"  
#include "Event.h"
const char* WINDOW_TITLE_DEFAULT = "Dragonfly";
const char* FONT_FILE_DEFAULT = "df-font.ttf";

DisplayManager::DisplayManager()
	: m_text(m_font, "", 30)
{
    setType("DisplayManager");
	m_text.setFont(m_font);
}

DisplayManager& DisplayManager::getInstance() {
    static DisplayManager inst;
    return inst;
}


int DisplayManager::startUp() {
    if (isStarted()) return 0;

    // Create window
    const sf::Vector2u size(
        static_cast<unsigned>(m_window_horizontal_pixels),
        static_cast<unsigned>(m_window_vertical_pixels));

    const sf::VideoMode mode(size); 

    m_p_window = new sf::RenderWindow(
        mode,
        WINDOW_TITLE_DEFAULT,
        WINDOW_STYLE_DEFAULT);

    if (!m_p_window || !m_p_window->isOpen()) {
        df::LogManager::getInstance().writeLog("DisplayManager: window creation failed\n");
        return -1;
    }

    // Load font
    if (!m_font.openFromFile(FONT_FILE_DEFAULT)) {
        df::LogManager::getInstance().writeLog("DisplayManager: failed to open font '%s'\n", FONT_FILE_DEFAULT);
        delete m_p_window;
        m_p_window = nullptr;
        return -1;
    }

    // Prepare text object sized to grid
    const float approx_px_per_char =
        static_cast<float>(m_window_vertical_pixels) / m_window_vertical_chars;
    m_text.setFont(m_font);
    m_text.setCharacterSize(static_cast<unsigned int>(approx_px_per_char));

    // Derive cell sizes (monospace assumed)
    m_cell_w = static_cast<float>(m_window_horizontal_pixels) / m_window_horizontal_chars;
    m_cell_h = static_cast<float>(m_window_vertical_pixels) / m_window_vertical_chars;

    m_p_window->clear(WINDOW_BACKGROUND_COLOR_DEFAULT);

	// Finish up
    df::Manager::startUp();
    df::LogManager::getInstance().writeLog(
        "DisplayManager started (%dx%d px, %dx%d chars)\n",
        m_window_horizontal_pixels, m_window_vertical_pixels,
        m_window_horizontal_chars, m_window_vertical_chars);
    return 0;
}
// Shutdown.
void DisplayManager::shutDown() {
    df::LogManager::getInstance().writeLog("DisplayManager shutting down\n");
    if (m_p_window) {
        m_p_window->close();
        delete m_p_window;
        m_p_window = nullptr;
    }
    df::Manager::shutDown();
}

// Convert df::Color to sf::Color.
sf::Color DisplayManager::toSF(df::Color c) const {
    return df::toSFColor(c);
}

// Convert grid coordinates to pixel coordinates (top-left of cell).
sf::Vector2f DisplayManager::gridToPixels(const Vector& grid_xy) const {
    // Top-left of the grid cell
    return sf::Vector2f(grid_xy.getX() * m_cell_w, grid_xy.getY() * m_cell_h);
}

// Draw single character at grid location with color. Return 0 ok else -1.
int DisplayManager::drawCh(Vector grid_pos, char ch, df::Color color) const {
    if (!m_p_window) return -1;
	m_text.setFont(m_font);
    m_text.setString(sf::String(ch));
    m_text.setFillColor(toSF(color));

    sf::Vector2f px = gridToPixels(grid_pos);
    px.y += (m_cell_h * 0.1f);
    m_text.setPosition(px);

    m_p_window->draw(m_text);
    return 0;
}

// Draw string at grid location with justification and color.
int DisplayManager::drawString(Vector grid_pos, const std::string& str,
    Justify just, df::Color color) const {
    if (!m_p_window) return -1;
    if (str.empty())   return 0;

    int start_x = static_cast<int>(grid_pos.getX());
    const int y = static_cast<int>(grid_pos.getY());

    switch (just) {
    case Justify::LEFT:   break;
    case Justify::CENTER: start_x -= static_cast<int>(str.size()) / 2; break;
    case Justify::RIGHT:  start_x -= static_cast<int>(str.size()) - 1; break;
    }

    int x = start_x;
    for (char ch : str) {
        drawCh(Vector(static_cast<float>(x), static_cast<float>(y)), ch, color);
        ++x;
    }
    return 0;
}

// Swap front and back buffers.
int DisplayManager::swapBuffers() {
    if (!m_p_window) return -1;


    m_p_window->display();
    m_p_window->clear(WINDOW_BACKGROUND_COLOR_DEFAULT);
    return 0;
}

// Set grid size (cols, rows). If <= 0, keep current.
void DisplayManager::setGridSize(int cols, int rows) {
    if (cols > 0) m_window_horizontal_chars = cols;
    if (rows > 0) m_window_vertical_chars = rows;

    m_cell_w = static_cast<float>(m_window_horizontal_pixels) / m_window_horizontal_chars;
    m_cell_h = static_cast<float>(m_window_vertical_pixels) / m_window_vertical_chars;

    const float approx_px_per_char =
        static_cast<float>(m_window_vertical_pixels) / m_window_vertical_chars;
    m_text.setCharacterSize(static_cast<unsigned int>(approx_px_per_char));
}

// Set pixel size (width, height). If <= 0, keep current.
void DisplayManager::setPixelSize(int w, int h) {
    if (w > 0) m_window_horizontal_pixels = w;
    if (h > 0) m_window_vertical_pixels = h;

    if (m_p_window) {
        const sf::Vector2u size(
            static_cast<unsigned>(m_window_horizontal_pixels),
            static_cast<unsigned>(m_window_vertical_pixels));

        const sf::VideoMode mode(size); // or (size, 32)

        m_p_window->create(mode, WINDOW_TITLE_DEFAULT, WINDOW_STYLE_DEFAULT);
    }

    const float approx_px_per_char =
        static_cast<float>(m_window_vertical_pixels) / m_window_vertical_chars;
    m_text.setCharacterSize(static_cast<unsigned int>(approx_px_per_char));
}
