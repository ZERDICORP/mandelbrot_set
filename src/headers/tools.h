#include "config.h"

void mandelbrot(std::vector<uint8_t>& pixmap, long double dAreaWidth, long double dCenterY, long double dCenterX, int iAccuracy);
void drawAreaBorder(sf::Vector2i position, std::vector<uint8_t>& pixmap, float fZoom);
void display(sf::RenderWindow& window, sf::Texture& texture, std::vector<uint8_t>& pixmap);
void setPixelToPixmap(std::vector<uint8_t>& pixmap, int iIndex, int iR, int iG, int iB);
void saveConfig(std::map<std::string, long double>& cfg);
void displayConsoleInformation(std::map<std::string, long double>& cfg, long double dAreaWidth, long double dCenterY, long double dCenterX, int iAccuracy);

std::map<std::string, long double> readConfig(std::string sConfigPath);

EVENT_CODE eventListener(sf::RenderWindow& window);

int init(sf::RenderWindow& window);
int main();