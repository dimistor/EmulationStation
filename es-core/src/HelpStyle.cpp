#include "HelpStyle.h"
#include "ThemeData.h"
#include "Renderer.h"
#include "resources/Font.h"

HelpStyle::HelpStyle()
{
	promptsPosition = Eigen::Vector2f(Renderer::getScreenWidth() * 0.012f, Renderer::getScreenHeight() * 0.9515f);
	promptsTextColor = 0x777777FF;
	promptsIconColor = 0x777777FF;

	timerPosition = Eigen::Vector2f(Renderer::getScreenWidth() * 0.988f, Renderer::getScreenHeight() * 0.9515f);
	timerTextColor = 0x777777FF;
	timerTextSuccessColor = 0x20CF20FF;
	timerTextErrorColor = 0xCF2020FF;

	if(FONT_SIZE_SMALL != 0)
	{
		promptsFont = Font::get(FONT_SIZE_SMALL);
		timerFont = Font::get(FONT_SIZE_SMALL);
	}
	else
	{
		// XXX: this hack allows to initialize HelpStyle when renderer has zero
		// width and height, thus FONT_SIZE_SMALL == 0 at that time and call to
		// Font::get(0) fails an assertion of non-zero font size. Assigning nullptr
		// to fonts works too, but using fallback size value feels safer.
		promptsFont = Font::get(10);
		timerFont = Font::get(10);
	}
}

void HelpStyle::applyTheme(const std::shared_ptr<ThemeData>& theme, const std::string& view)
{
	auto elemHelp = theme->getElement(view, "help", "helpsystem");
	auto elemTimer = theme->getElement(view, "timer", "helpsystem");

	if(elemHelp)
	{
		if(elemHelp->has("pos"))
			promptsPosition = elemHelp->get<Eigen::Vector2f>("pos").cwiseProduct(Eigen::Vector2f((float)Renderer::getScreenWidth(), (float)Renderer::getScreenHeight()));

		if(elemHelp->has("iconColor"))
			promptsIconColor = elemHelp->get<unsigned int>("iconColor");

		if(elemHelp->has("textColor"))
		{
			promptsTextColor = elemHelp->get<unsigned int>("textColor");
			timerTextColor = elemHelp->get<unsigned int>("textColor");
		}

		if(elemHelp->has("fontPath") || elemHelp->has("fontSize"))
		{
			promptsFont = Font::getFromTheme(elemHelp, ThemeFlags::ALL, promptsFont);
			timerFont = Font::getFromTheme(elemHelp, ThemeFlags::ALL, promptsFont);
		}
	}

	// override fallback values if present
	if(elemTimer)
	{
		if(elemTimer->has("pos"))
			timerPosition = elemTimer->get<Eigen::Vector2f>("pos").cwiseProduct(Eigen::Vector2f((float)Renderer::getScreenWidth(), (float)Renderer::getScreenHeight()));

		if(elemTimer->has("textColor"))
			timerTextColor = elemTimer->get<unsigned int>("textColor");

		if(elemTimer->has("textSuccessColor"))
			timerTextSuccessColor = elemTimer->get<unsigned int>("textSuccessColor");

		if(elemTimer->has("textErrorColor"))
			timerTextErrorColor = elemTimer->get<unsigned int>("textErrorColor");

		if(elemTimer->has("fontPath") || elemTimer->has("fontSize"))
			timerFont = Font::getFromTheme(elemTimer, ThemeFlags::ALL, timerFont);
	}
}
