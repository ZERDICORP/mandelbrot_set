#include "config.h"
#include "tools.h"

bool zooming(sf::Vector2i& position, std::vector<uint8_t>& pixmap, long double& dAreaWidth, long double& dCenterY, long double& dCenterX,
	int iAccuracy, auto scalingArea)
{
	dCenterY += dAreaWidth / mWH * position.y - dAreaWidth / 2;
	dCenterX += dAreaWidth / mWW * position.x - dAreaWidth / 2;
	
	scalingArea(dAreaWidth);

	mandelbrot(pixmap, dAreaWidth, dCenterY, dCenterX, iAccuracy);

	return 0;
}

int loop(sf::RenderWindow& window, std::map<std::string, long double>& cfg)
{
	bool bNeedToUpdateConsole = true;
	bool bZooming = false;

	int iAccuracy = cfg["accuracy"];
	int iAccuracyScalingCount = 0;

	float fAreaWidthScalingFactor = cfg["areaWidthScalingFactor"];

	long double dAreaWidth = cfg["areaWidth"];
	long double dCenterY = cfg["y"];
	long double dCenterX = cfg["x"];

	std::vector<uint8_t> pixmap(mMapLength * 4, 255);

	sf::Texture texture;
	texture.create(mWW, mWH);

	mandelbrot(pixmap, dAreaWidth, dCenterY, dCenterX, iAccuracy);

	while (window.isOpen())
	{
		display(window, texture, pixmap);

		if (bNeedToUpdateConsole)
		{
			displayConsoleInformation(cfg, dAreaWidth, dCenterY, dCenterX, iAccuracy);
			bNeedToUpdateConsole = false;
		}

		sf::Vector2i position = sf::Mouse::getPosition(window);

		switch (eventListener(window))
		{
			case (CLOSE_EVENT_CODE):
				window.close();
				break;

			case RESTART_EVENT_CODE:
				init(window);
				break;

			case MOUSE_LEFT_EVENT_CODE:
				if (!bZooming && zer::athm::inRange2D(mWH, mWW, position.y, position.x))
				{
					/*
						Increase accuracy.
					*/
					iAccuracyScalingCount++;
					if (iAccuracyScalingCount >= cfg["accuracyScalingFrequency"])
					{
						iAccuracy *= cfg["accuracyScalingFactor"];
						iAccuracyScalingCount = 0;
					}
					
					bNeedToUpdateConsole = true;
					bZooming = true;

					drawAreaBorder(position, pixmap, mWW * fAreaWidthScalingFactor);
					display(window, texture, pixmap);
					
					bZooming = zooming(position, pixmap, dAreaWidth, dCenterY, dCenterX, iAccuracy, [&fAreaWidthScalingFactor](long double& dAreaWidth){
						dAreaWidth *= fAreaWidthScalingFactor;
					});
				}
				break;

			case MOUSE_RIGHT_EVENT_CODE:
				if (!bZooming && zer::athm::inRange2D(mWH, mWW, position.y, position.x))
				{
					/*
						Decrease accuracy.
					*/
					iAccuracyScalingCount--;
					if (iAccuracyScalingCount <= 0)
					{
						iAccuracy /= cfg["accuracyScalingFactor"];
						iAccuracyScalingCount = cfg["accuracyScalingFrequency"];
					}

					bNeedToUpdateConsole = true;
					bZooming = true;
					
					bZooming = zooming(position, pixmap, dAreaWidth, dCenterY, dCenterX, iAccuracy, [&fAreaWidthScalingFactor](long double& dAreaWidth){
						dAreaWidth /= fAreaWidthScalingFactor;
					});
				}
				break;
			
			/*
				Setting & Save default ".cfg" values.
			*/
			case RESTORE_DEFAULT_CONFIG_EVENT_CODE:
				cfg["areaWidth"] = mDefaultAreaWidth;
				cfg["areaWidthScalingFactor"] = mDefaultAreaWidthScalingFactor;
				cfg["y"] = mDefaultY;
				cfg["x"] = mDefaultX;
				cfg["accuracy"] = mDefaultAccuracy;
				cfg["accuracyScalingFrequency"] = mDefaultAccuracyScalingFrequency;
				cfg["accuracyScalingFactor"] = mDefaultAccuracyScalingFactor;
				
				saveConfig(cfg);
				
				bNeedToUpdateConsole = true;
				break;
		}
	}

	return 0;
}

int init(sf::RenderWindow& window)
{
	std::map<std::string, long double> cfg = readConfig(mConfigPath);

	return loop(window, cfg);
}