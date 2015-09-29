#pragma once

#include "GuiComponent.h"
#include "HelpStyle.h"

class ImageComponent;
class TextComponent;
class TextureResource;
class ComponentGrid;

class HelpComponent : public GuiComponent
{
public:
	HelpComponent(Window* window);

	void clearPrompts();
	void setPrompts(const std::vector<HelpPrompt>& prompts);

	void clearTimer();
	void setTimer(int seconds);

	void render(const Eigen::Affine3f& parent) override;
	void setOpacity(unsigned char opacity) override;

	void setStyle(const HelpStyle& style);

private:
	std::shared_ptr<TextureResource> getIconTexture(const char* name);
	std::map< std::string, std::shared_ptr<TextureResource> > mIconCache;

	std::shared_ptr<ComponentGrid> mPromptsGrid;
	std::shared_ptr<ComponentGrid> mTimerGrid;
	void updatePrompts();
	void updateTimer();

	std::vector<HelpPrompt> mPrompts;
	std::vector<int> mTimer;
	HelpStyle mStyle;
};
