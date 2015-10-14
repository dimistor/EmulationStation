#pragma once

#include "GuiComponent.h"
#include "HelpStyle.h"

class ImageComponent;
class TextComponent;
class TextureResource;
class ComponentGrid;

enum TimerStatus { TIMER_STATUS_NONE, TIMER_STATUS_SUCCESS, TIMER_STATUS_ERROR };

class HelpComponent : public GuiComponent
{
public:
	HelpComponent(Window* window);

	void clearPrompts();
	void setPrompts(const std::vector<HelpPrompt>& prompts);

	void clearTimer();
	int setTimer(unsigned int seconds);
	void setTimerStatus(TimerStatus status);

	void render(const Eigen::Affine3f& parent) override;
	void setOpacity(unsigned char opacity) override;

	void setStyle(const HelpStyle& style);

private:
	std::shared_ptr<TextureResource> getIconTexture(const char* name);
	std::map< std::string, std::shared_ptr<TextureResource> > mIconCache;


	std::shared_ptr<ComponentGrid> mPromptsGrid;
	std::shared_ptr<ComponentGrid> mTimerGrid;
	void updatePrompts();
	void updateTimer(TimerStatus status = TIMER_STATUS_NONE);

	std::vector<HelpPrompt> mPrompts;
	std::vector<unsigned int> mTimer;
	unsigned int mTimerSeconds;
	HelpStyle mStyle;
};
