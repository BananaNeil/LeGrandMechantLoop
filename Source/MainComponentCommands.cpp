/*
  ==============================================================================

    MainComponentCommands.cpp
    Created: 15 Nov 2020 8:49:36am
    Author:  bkupe

  ==============================================================================
*/


#include "MainComponent.h"
#include "Transport/Transport.h"
#include "Preset/PresetIncludes.h"
#include  "Engine/AudioManager.h"

namespace LGMLCommandIDs
{
	static const int showAbout = 0x60000;
	static const int gotoWebsite = 0x60001;
	static const int gotoForum = 0x60002;
	static const int gotoDocs = 0x60003;
	static const int postGithubIssue = 0x60004;
	static const int donate = 0x60005;
	static const int showWelcome = 0x60006;
	static const int gotoChangelog = 0x60007;

	static const int saveCurrentPreset = 0x61000;
	static const int loadCurrentPreset = 0x61001;

	static const int editAudioSettings = 0x62001;

	static const int playPauseTransport = 0x501;
}

void MainComponent::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
	switch (commandID)
	{
	//case LGMLCommandIDs::showAbout:
	//	result.setInfo("About...", "", "General", result.readOnlyInKeyEditor);
	//	break;

	//case LGMLCommandIDs::showWelcome:
		//result.setInfo("Show Welcome Screen...", "", "General", result.readOnlyInKeyEditor);
		//break;

	case LGMLCommandIDs::donate:
		result.setInfo("Be cool and donate", "", "General", result.readOnlyInKeyEditor);
		break;

	case LGMLCommandIDs::gotoWebsite:
		result.setInfo("Go to website", "", "Help", result.readOnlyInKeyEditor);
		break;
	//case LGMLCommandIDs::gotoForum:
	//	result.setInfo("Go to forum", "", "Help", result.readOnlyInKeyEditor);
	//	break;

	//case LGMLCommandIDs::gotoDocs:
	//	result.setInfo("Go to the Amazing Documentation", "", "Help", result.readOnlyInKeyEditor);
	//	break;

	case LGMLCommandIDs::gotoChangelog:
		result.setInfo("See the changelog", "", "Help", result.readOnlyInKeyEditor);
		break;


	case LGMLCommandIDs::postGithubIssue:
		result.setInfo("Post an issue on github", "", "Help", result.readOnlyInKeyEditor);
		break;

	case LGMLCommandIDs::saveCurrentPreset:
		result.setInfo("Save Current Preset", "", "Edit", result.readOnlyInKeyEditor);
		result.addDefaultKeypress(KeyPress::createFromDescription("s").getKeyCode(), ModifierKeys::commandModifier | ModifierKeys::altModifier);
		break;

	case LGMLCommandIDs::loadCurrentPreset:
		result.setInfo("Load Current Preset", "", "Edit", result.readOnlyInKeyEditor);
		result.addDefaultKeypress(KeyPress::createFromDescription("l").getKeyCode(), ModifierKeys::commandModifier | ModifierKeys::altModifier);
		break;

	case LGMLCommandIDs::editAudioSettings:
		result.setInfo("Audio Settings...", "", "File", result.readOnlyInKeyEditor);
		result.addDefaultKeypress(KeyPress::createFromDescription("l").getKeyCode(), ModifierKeys::commandModifier | ModifierKeys::altModifier);
		break;

	case LGMLCommandIDs::playPauseTransport:
		result.setInfo("Play / Pause Transport", "", "Edit", 0);
		result.addDefaultKeypress(KeyPress::spaceKey, ModifierKeys::noModifiers);
		break;

	default:
		OrganicMainContentComponent::getCommandInfo(commandID, result);
		break;
	}
}



void MainComponent::getAllCommands(Array<CommandID>& commands) {

	OrganicMainContentComponent::getAllCommands(commands);

	const CommandID ids[] = {

		LGMLCommandIDs::saveCurrentPreset,
		LGMLCommandIDs::loadCurrentPreset,
		LGMLCommandIDs::playPauseTransport,
		LGMLCommandIDs::editAudioSettings,
		//LGMLCommandIDs::showAbout,
		//LGMLCommandIDs::showWelcome,
		LGMLCommandIDs::donate,
		LGMLCommandIDs::gotoWebsite,
		//LGMLCommandIDs::gotoForum,
		//LGMLCommandIDs::gotoDocs,
		LGMLCommandIDs::gotoChangelog,
		LGMLCommandIDs::postGithubIssue,
	};

	commands.addArray(ids, numElementsInArray(ids));
	//for (int i = 0; i < Guider::getInstance()->factory.defs.size(); ++i) commands.add(LGMLCommandIDs::guideStart + i);
}


PopupMenu MainComponent::getMenuForIndex(int topLevelMenuIndex, const String& menuName)
{
	PopupMenu menu = OrganicMainContentComponent::getMenuForIndex(topLevelMenuIndex, menuName);

	if (menuName == "Edit")
	{
		menu.addCommandItem(&getCommandManager(), LGMLCommandIDs::playPauseTransport);
		menu.addCommandItem(&getCommandManager(), LGMLCommandIDs::saveCurrentPreset);
		menu.addCommandItem(&getCommandManager(), LGMLCommandIDs::loadCurrentPreset);
	}else if (menuName == "Help")
	{
		//menu.addCommandItem(&getCommandManager(), LGMLCommandIDs::showAbout);
		//menu.addCommandItem(&getCommandManager(), LGMLCommandIDs::showWelcome);
		menu.addCommandItem(&getCommandManager(), LGMLCommandIDs::donate);
		menu.addSeparator();
		menu.addCommandItem(&getCommandManager(), LGMLCommandIDs::gotoWebsite);
		//menu.addCommandItem(&getCommandManager(), LGMLCommandIDs::gotoForum);
		//menu.addCommandItem(&getCommandManager(), LGMLCommandIDs::gotoDocs);
		menu.addCommandItem(&getCommandManager(), LGMLCommandIDs::gotoChangelog);
		menu.addCommandItem(&getCommandManager(), LGMLCommandIDs::postGithubIssue);

	}
	else if (menuName == "Guides")
	{
		/*	for (int i = 0; i < Guider::getInstance()->factory.defs.size(); ++i)
			{
				menu.addCommandItem(&getCommandManager(), LGMLCommandIDs::guideStart + i);
			}

			menu.addSeparator();
			menu.addCommandItem(&getCommandManager(), LGMLCommandIDs::exitGuide);
			*/
	}

	return menu;
}

void MainComponent::fillFileMenuInternal(PopupMenu& menu)
{
	menu.addSeparator();
	menu.addCommandItem(&getCommandManager(), LGMLCommandIDs::editAudioSettings);
}

bool MainComponent::perform(const InvocationInfo& info)
{

	//if (info.commandID >= LGMLCommandIDs::guideStart && info.commandID < LGMLCommandIDs::guideStart + 100)
	//{
	//	Guider::getInstance()->launchGuideAtIndex(info.commandID - LGMLCommandIDs::guideStart);
	//	return true;
	//}

	switch (info.commandID)
	{


		//case LGMLCommandIDs::showAbout:
		//{
		//	AboutWindow w;
		//	DialogWindow::showModalDialog("About", &w, getTopLevelComponent(), Colours::transparentBlack, true);
		//}
		//break;

		//case LGMLCommandIDs::showWelcome:
		//{
		//	WelcomeScreen w;
		//	DialogWindow::showModalDialog("Welcome", &w, getTopLevelComponent(), Colours::black, true);
		//}
		//break;

	case LGMLCommandIDs::donate:
		URL("https://www.paypal.me/benkuper").launchInDefaultBrowser();
		break;

	case LGMLCommandIDs::gotoWebsite:
		URL("http://benjamin.kuperberg.fr/lgml").launchInDefaultBrowser();
		break;

	//case LGMLCommandIDs::gotoForum:
	//	URL("http://benjamin.kuperberg.fr/lgml/forum").launchInDefaultBrowser();
	//	break;

	//case LGMLCommandIDs::gotoDocs:
	//	URL("https://benjamin.kuperberg.fr/lgml/docs").launchInDefaultBrowser();
	//	break;

	case LGMLCommandIDs::gotoChangelog:
		URL("https://benjamin.kuperberg.fr/lgml/releases/changelog.html").launchInDefaultBrowser();
		break;

	case LGMLCommandIDs::postGithubIssue:
		URL("https://github.com/benkuper/LeGrandMechantLoop/issues").launchInDefaultBrowser();
		break;

	case LGMLCommandIDs::playPauseTransport:
		Transport::getInstance()->togglePlayTrigger->trigger();
		break;

	case LGMLCommandIDs::saveCurrentPreset:
		RootPresetManager::getInstance()->saveCurrentTrigger->trigger();
		break;

	case LGMLCommandIDs::loadCurrentPreset:
		RootPresetManager::getInstance()->loadCurrentTrigger->trigger();
		break;

	case LGMLCommandIDs::editAudioSettings:
		AudioManager::getInstance()->selectThis();
		break;

		//case LGMLCommandIDs::exitGuide:
		//	Guider::getInstance()->setCurrentGuide(nullptr);
		//	break;

	default:
		return OrganicMainContentComponent::perform(info);
	}

	return true;
}

StringArray MainComponent::getMenuBarNames()
{
	StringArray names = OrganicMainContentComponent::getMenuBarNames();
	//names.add("Guides");
	names.add("Help");
	return names;
}
