#pragma once

#include <Eigen/Dense>
#include <memory>
#include <string>

class ThemeData;
class Font;

struct HelpStyle
{
	Eigen::Vector2f promptsPosition;
	unsigned int promptsTextColor;
	unsigned int promptsIconColor;
	std::shared_ptr<Font> promptsFont;

	Eigen::Vector2f timerPosition;
	unsigned int timerTextColor;
	std::shared_ptr<Font> timerFont;

	HelpStyle(); // default values
	void applyTheme(const std::shared_ptr<ThemeData>& theme, const std::string& view);
};
