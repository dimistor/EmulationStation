#include "components/HelpComponent.h"
#include "Renderer.h"
#include "Settings.h"
#include "Log.h"
#include "Util.h"
#include "components/ImageComponent.h"
#include "components/TextComponent.h"
#include "components/ComponentGrid.h"
#include <boost/assign.hpp>

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

HelpComponent::HelpComponent(Window* window) : GuiComponent(window), mTimer(-1)
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
	mTimer = -1;
	updateTimer();
}

void HelpComponent::setTimer(int seconds)
{
	mTimer = seconds;
	updateTimer();
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

void HelpComponent::updateTimer()
{
	if(!Settings::getInstance()->getBool("ShowHelpPrompts") /*|| mTimer < 0*/)
	{
		mTimerGrid.reset();
		return;
	}

	std::shared_ptr<Font>& font = mStyle.timerFont;

	float width = 0;
	const float height = round(font->getLetterHeight() * 1.25f);

	mTimerGrid = std::make_shared<ComponentGrid>(mWindow, Vector2i(1, 1));
	auto timer = std::make_shared<TextComponent>(mWindow, "04:32", font, mStyle.timerTextColor);
	width += timer->getSize().x();
	mTimerGrid->setSize(width, height);
	mTimerGrid->setColWidthPerc(0, 1.0f);
	mTimerGrid->setEntry(timer, Vector2i(0, 0), false, false);

	mTimerGrid->setPosition(Eigen::Vector3f(mStyle.timerPosition.x() - width, mStyle.timerPosition.y(), 0.0f));
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
