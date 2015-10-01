#include "components/HelpComponent.h"
#include "Renderer.h"
#include "Settings.h"
#include "Log.h"
#include "Util.h"
#include "components/ImageComponent.h"
#include "components/TextComponent.h"
#include "components/ComponentGrid.h"
#include <boost/assign.hpp>
#include "animations/LambdaAnimation.h"

#define OFFSET_X 12 // move the entire thing right by this amount (px)
#define OFFSET_Y 12 // move the entire thing up by this amount (px)

#define ICON_TEXT_SPACING 8 // space between [icon] and [text] (px)
#define ENTRY_SPACING 16 // space between [text] and next [icon] (px)

using namespace Eigen;

static const std::map<std::string, const char*> ICON_PATH_MAP = boost::assign::map_list_of
	("up/down", ":/help/dpad_updown.svg")
	("left/right", ":/help/dpad_leftright.svg")
	("up/down/left/right", ":/help/dpad_all.svg")
	("a", ":/help/button_a.svg")
	("b", ":/help/button_b.svg")
	("x", ":/help/button_x.svg")
	("y", ":/help/button_y.svg")
	("l", ":/help/button_l.svg")
	("r", ":/help/button_r.svg")
	("start", ":/help/button_start.svg")
	("select", ":/help/button_select.svg");

HelpComponent::HelpComponent(Window* window) : GuiComponent(window), mTimerSeconds(0)
{
}

void HelpComponent::clearPrompts()
{
	mPrompts.clear();
	updatePrompts();
}

void HelpComponent::setPrompts(const std::vector<HelpPrompt>& prompts)
{
	mPrompts = prompts;
	updatePrompts();
}

void HelpComponent::clearTimer()
{
	mTimer.clear();
	updateTimer();
}

void HelpComponent::setTimer(unsigned int seconds)
{
	if(!mTimer.empty() && mTimerSeconds == seconds)
		return;

	int s = seconds % 60;
	int m = (int)(seconds / 60) % 60;
	int h = (int)(seconds / (60 * 60));

	mTimer.clear();
	mTimer.push_back(s);
	mTimer.push_back(m);
	if(h > 0) mTimer.push_back(h);
	bool increased = seconds > mTimerSeconds;
	mTimerSeconds = seconds;

	if(increased)
		updateTimer(TIMER_STATUS_SUCCESS);
	else
		updateTimer();
}

void HelpComponent::setTimerStatus(TimerStatus status)
{
	updateTimer(status);
}

void HelpComponent::setStyle(const HelpStyle& style)
{
	mStyle = style;
	updatePrompts();
	updateTimer();
}

void HelpComponent::updatePrompts()
{
	if(!Settings::getInstance()->getBool("ShowHelpPrompts") || mPrompts.empty())
	{
		mPromptsGrid.reset();
		return;
	}

	std::shared_ptr<Font>& font = mStyle.promptsFont;

	mPromptsGrid = std::make_shared<ComponentGrid>(mWindow, Vector2i(mPrompts.size() * 4, 1));
	// [icon] [spacer1] [text] [spacer2]

	std::vector< std::shared_ptr<ImageComponent> > icons;
	std::vector< std::shared_ptr<TextComponent> > labels;

	float width = 0;
	const float height = round(font->getLetterHeight() * 1.25f);
	for(auto it = mPrompts.begin(); it != mPrompts.end(); it++)
	{
		auto icon = std::make_shared<ImageComponent>(mWindow);
		icon->setImage(getIconTexture(it->first));
		icon->setColorShift(mStyle.promptsIconColor);
		icon->setResize(Eigen::Vector2f(0, height));
		icons.push_back(icon);

		auto lbl = std::make_shared<TextComponent>(mWindow, strToUpper(it->second), font, mStyle.promptsTextColor);
		labels.push_back(lbl);

		width += icon->getSize().x() + lbl->getSize().x() + ICON_TEXT_SPACING + ENTRY_SPACING;
	}

	mPromptsGrid->setSize(width, height);
	for(unsigned int i = 0; i < icons.size(); i++)
	{
		const int col = i*4;
		mPromptsGrid->setColWidthPerc(col, icons.at(i)->getSize().x() / width);
		mPromptsGrid->setColWidthPerc(col + 1, ICON_TEXT_SPACING / width);
		mPromptsGrid->setColWidthPerc(col + 2, labels.at(i)->getSize().x() / width);

		mPromptsGrid->setEntry(icons.at(i), Vector2i(col, 0), false, false);
		mPromptsGrid->setEntry(labels.at(i), Vector2i(col + 2, 0), false, false);
	}

	mPromptsGrid->setPosition(Eigen::Vector3f(mStyle.promptsPosition.x(), mStyle.promptsPosition.y(), 0.0f));
	//mPromptsGrid->setPosition(OFFSET_X, Renderer::getScreenHeight() - mPromptsGrid->getSize().y() - OFFSET_Y);
}

void HelpComponent::updateTimer(TimerStatus status)
{
	if(Settings::getInstance()->isFreePlay() || mTimer.empty())
	{
		mTimerGrid.reset();
		return;
	}

	std::shared_ptr<Font>& font = mStyle.timerFont;

	mTimerGrid = std::make_shared<ComponentGrid>(mWindow, Vector2i(mTimer.size() * 2, 1));
	// [] [h] | [colon] [m] | [colon] [s]

	std::vector< std::shared_ptr<TextComponent> > labels;

	float width = 0;
	const float height = round(font->getLetterHeight() * 1.25f);
	for(auto it = mTimer.begin(); it != mTimer.end(); it++)
	{
		std::stringstream ss;
		ss << std::setw(2) << std::setfill('0') << *it;
		auto lbl = std::make_shared<TextComponent>(mWindow, ss.str(), font, mStyle.timerTextColor);
		auto cln = std::make_shared<TextComponent>(mWindow, ":", font, mStyle.timerTextColor);
		labels.push_back(lbl);
		labels.push_back(cln);

		width += lbl->getSize().x() + cln->getSize().x();
	}

	mTimerGrid->setSize(width, height);

	std::reverse(labels.begin(), labels.end());
	for(unsigned int i = 1; i < labels.size(); i++)
	{
		mTimerGrid->setColWidthPerc(i, labels.at(i)->getSize().x() / width);
		mTimerGrid->setEntry(labels.at(i), Vector2i(i, 0), false, false);
	}

	mTimerGrid->setPosition(Eigen::Vector3f(mStyle.timerPosition.x() - width, mStyle.timerPosition.y(), 0.0f));

	if(status != TIMER_STATUS_NONE)
	{
		cancelAnimation(0);

		unsigned int defaultColor = mStyle.timerTextColor;
		unsigned int statusColor;
		switch (status) {
			case TIMER_STATUS_SUCCESS:
				statusColor = mStyle.timerTextSuccessColor;
				break;
			case TIMER_STATUS_ERROR:
				statusColor = mStyle.timerTextErrorColor;
				break;
		}

		auto colorFunc = [labels, defaultColor, statusColor](float t) {
			float step = smoothStep(0, 1, t);
			unsigned int color = lerpColor(defaultColor, statusColor, step);
			for(auto it = labels.begin(); it != labels.end(); it++)
				(*it)->setColor(color);
		};

		int ANIM_DURATION = 500;
		setAnimation(new LambdaAnimation(colorFunc, ANIM_DURATION), 0, [this, ANIM_DURATION, colorFunc] {
			setAnimation(new LambdaAnimation(colorFunc, ANIM_DURATION), 0, nullptr, true);
		});
	}
}

std::shared_ptr<TextureResource> HelpComponent::getIconTexture(const char* name)
{
	auto it = mIconCache.find(name);
	if(it != mIconCache.end())
		return it->second;

	auto pathLookup = ICON_PATH_MAP.find(name);
	if(pathLookup == ICON_PATH_MAP.end())
	{
		LOG(LogError) << "Unknown help icon \"" << name << "\"!";
		return nullptr;
	}
	if(!ResourceManager::getInstance()->fileExists(pathLookup->second))
	{
		LOG(LogError) << "Help icon \"" << name << "\" - corresponding image file \"" << pathLookup->second << "\" misisng!";
		return nullptr;
	}

	std::shared_ptr<TextureResource> tex = TextureResource::get(pathLookup->second);
	mIconCache[std::string(name)] = tex;
	return tex;
}

void HelpComponent::setOpacity(unsigned char opacity)
{
	GuiComponent::setOpacity(opacity);

	for(unsigned int i = 0; i < mPromptsGrid->getChildCount(); i++)
	{
		mPromptsGrid->getChild(i)->setOpacity(opacity);
	}
}

void HelpComponent::render(const Eigen::Affine3f& parentTrans)
{
	Eigen::Affine3f trans = parentTrans * getTransform();

	if(mPromptsGrid)
		mPromptsGrid->render(trans);

	if(mTimerGrid)
		mTimerGrid->render(trans);
}
