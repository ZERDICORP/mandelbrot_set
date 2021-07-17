#include "tools.h"

void inSet(long double dReal, long double dImaginary, int iAccuracy, int& iN)
{
	long double dTempReal = dReal;
	long double dTempImaginary = dImaginary;
	long double dTemp;
	long double dTempRealSqr = dTempReal * dTempReal;
	long double dTempImaginarySqr = dTempImaginary * dTempImaginary;

	iN = 0;

	for(; dTempRealSqr + dTempImaginarySqr <= 4.0 && iN < iAccuracy; ++iN)
	{
		dTempReal = dTempImaginary * dTempReal;
		dTempReal += dTempReal;
		dTempReal += dReal;
		dTempImaginary = dTempImaginarySqr - dTempRealSqr + dImaginary;
		dTempRealSqr = dTempReal * dTempReal;
		dTempImaginarySqr = dTempImaginary * dTempImaginary;
	}

	iN = iN == iAccuracy ? 0 : iN;
}

void mandelbrot(std::vector<uint8_t>& pixmap, long double dAreaWidth, long double dCenterY, long double dCenterX, int iAccuracy)
{
	long double dReal = (dCenterY - dAreaWidth / 2);
	long double dImaginaryMin = (dCenterX - dAreaWidth / 2);
	long double dStep = dAreaWidth / mWH;

	int iY;
	int iX;
	int iN;

	for (int i = 0; i < mWH * mWW; ++i)
	{
		iY = i / mWW;
		iX = i % mWW;

		inSet(dReal + dStep * iY, dImaginaryMin + dStep * iX, iAccuracy, iN);
		setPixelToPixmap(pixmap, mWW * iY + iX, iN, iN, iN);
	}
}

void drawAreaBorder(sf::Vector2i position, std::vector<uint8_t>& pixmap, float fZoom)
{
	for (float f = -fZoom / 2; f < fZoom / 2; f += 1.0)
	{
		if (zer::athm::inRange2D(mWH, mWW, position.y + f, position.x - fZoom / 2))
			setPixelToPixmap(pixmap, mWW * (position.y + f) + (position.x - fZoom / 2), 255, 0, 0);
		
		if (zer::athm::inRange2D(mWH, mWW, position.y + f, position.x + fZoom / 2))
			setPixelToPixmap(pixmap, mWW * (position.y + f) + (position.x + fZoom / 2), 255, 0, 0);
		
		if (zer::athm::inRange2D(mWH, mWW, position.y - fZoom / 2, position.x + f))
			setPixelToPixmap(pixmap, mWW * (position.y - fZoom / 2) + (position.x + f), 255, 0, 0);
		
		if (zer::athm::inRange2D(mWH, mWW, position.y + fZoom / 2, position.x + f))
			setPixelToPixmap(pixmap, mWW * (position.y + fZoom / 2) + (position.x + f), 255, 0, 0);
	}
}

void display(sf::RenderWindow& window, sf::Texture& texture, std::vector<uint8_t>& pixmap)
{
	texture.update(&pixmap[0]);
	window.draw(sf::Sprite(texture));
	window.display();
}

void setPixelToPixmap(std::vector<uint8_t>& pixmap, int iIndex, int iR, int iG, int iB)
{
	pixmap[iIndex * 4 + 0] = iR;
	pixmap[iIndex * 4 + 1] = iG;
	pixmap[iIndex * 4 + 2] = iB;
}

void displayConsoleInformation(std::map<std::string, long double>& cfg, long double dAreaWidth, long double dCenterY, long double dCenterX,
	int iAccuracy)
{
	system("cls");

	std::cout << "# " << mTitle << " #" << std::endl;
	std::cout << "\n[!] area width: " << dAreaWidth << std::endl;
	std::cout << "\n[!] y-coordinate: " << dCenterY << std::endl;
	std::cout << "\n[!] x-coordinate: " << dCenterX << std::endl;
	std::cout << "\n[!] accuracy: " << iAccuracy << std::endl;
	std::cout << "\n[!] keyboard buttons for control:" << std::endl;
	std::cout << "\t [ ESC ] - exit;" << std::endl;
	std::cout << "\t [ R ] - restart;" << std::endl;
	std::cout << "\t [ F3 ] - restore default configuration;" << std::endl;
	std::cout << "\n[!] zoom in: LEFT mouse click;" << std::endl;
	std::cout << "\n[!] zoom out: RIGHT mouse click;" << std::endl;
	std::cout << "\n[!] note: visit a \"" << mConfigPath << "\" file to change configuration;" << std::endl;
	std::cout << "\n[!] current configuration:" << std::endl;
	
	for (std::map<std::string, long double>::iterator p = cfg.begin(); p != cfg.end(); p++)
		std::cout << "\t" << p -> first << " = " << p -> second << ";" << std::endl;
}

std::map<std::string, long double> readConfig(std::string sConfigPath)
{
	std::map<std::string, long double> cfg;

	zer::File file(sConfigPath);
	file.read({zer::file::Modifier::lines});

	for (int i = 0; i < file.linesLen(); ++i)
	{
		std::string sLine = file.lineAt(i);
		if (sLine.find(" = ") != std::string::npos)
		{
			std::vector<std::string> lineParts = zer::athm::split(sLine, " = ");
			cfg[lineParts[0]] = stof(lineParts[1]);
		}
	}

	return cfg;
}

void saveConfig(std::map<std::string, long double>& cfg)
{
	std::ostringstream ssRes;

	for (std::map<std::string, long double>::iterator p = cfg.begin(); p != cfg.end(); p++)
	{
		/*
			Remove trailing zeros.
		*/
		std::string sNumber = std::to_string(p -> second);
		sNumber.erase(sNumber.find_last_not_of('0') + 1, std::string::npos);
		
		if (sNumber[sNumber.size() - 1] == '.')
			sNumber.erase(sNumber.size() - 1, 1);

		ssRes << p -> first << " = " << sNumber << std::endl;
	}

	zer::File file(mConfigPath);
	file.write(ssRes.str());
}

EVENT_CODE eventListener(sf::RenderWindow& window)
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			return CLOSE_EVENT_CODE;
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Escape)
				return CLOSE_EVENT_CODE;
			if (event.key.code == sf::Keyboard::R)
				return RESTART_EVENT_CODE;
			if (event.key.code == sf::Keyboard::F3)
				return RESTORE_DEFAULT_CONFIG_EVENT_CODE;
		}
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			return MOUSE_LEFT_EVENT_CODE;
		else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
			return MOUSE_RIGHT_EVENT_CODE;
	}
	return NULL_EVENT_CODE;
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(mWW, mWH), mTitle);

	sf::Cursor cursor;
	if (cursor.loadFromSystem(sf::Cursor::Cross))
		window.setMouseCursor(cursor);

	return init(window);
}