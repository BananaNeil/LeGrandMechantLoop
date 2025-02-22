/*
  ==============================================================================

	VSTNode.cpp
	Created: 15 Nov 2020 8:42:29am
	Author:  bkupe

  ==============================================================================
*/

#include "Node/NodeIncludes.h"

VSTNode::VSTNode(var params) :
	Node(getTypeString(), params, true, true, true, true, true, true),
	currentPreset(nullptr),
	macrosCC("Macros"),
	antiMacroFeedback(false),
	isSettingVST(false),
	prevWetDry(1),
	vstNotifier(5)
{
	numAudioInputs->canBeDisabledByUser = true;
	numAudioInputs->setEnabled(false);
	numAudioOutputs->canBeDisabledByUser = true;
	numAudioOutputs->setEnabled(false);

	pluginParam = new VSTPluginParameter("VST", "The VST to use");
	ControllableContainer::addParameter(pluginParam);

	dryWet = addFloatParameter("Dry Wet", "Wet Dry Ratio. 1 is totally wet, 0 is totally dry", 1, 0, 1);
	disableOnDry = addBoolParameter("Disable On Dry", "If checked, the VST will be disabled when the wet dry parameter is 0", true);

	clearBufferOnDisable = addBoolParameter("Clear Buffer On Disable", "If checked, this will clear the buffer when the vst is disable. This allows to avoid long reverb staying when re-enabling for instance.", true);
	presetEnum = addEnumParameter("Preset", "Load a preset");
	numMacros = addIntParameter("Num Macros", "Choose the number of macros you want for this VST", 0, 0);
	autoActivateMacroIndex = addIntParameter("Auto Bypass Macro", "Index of the macro that automatically bypasses the VST if value is in the range", 1, 1, 1, false);
	autoActivateMacroIndex->canBeDisabledByUser = true;
	autoActivateRange = addPoint2DParameter("Auto Bypass Range", "Range in which the VST is bypassed", false);
	autoActivateRange->setBounds(0, 0, 1, 1);
	autoActivateRange->setPoint(0, 0);

	addChildControllableContainer(&macrosCC);


	setIOFromVST(); //force nothing

	viewUISize->setPoint(200, 150);
}

VSTNode::~VSTNode()
{
}

void VSTNode::clearItem()
{
	Node::clearItem();
	setupVST(nullptr);
}


void VSTNode::setupVST(PluginDescription* description)
{
	ScopedSuspender sp(processor);

	isSettingVST = true;


	if (vst != nullptr)
	{
		vst->releaseResources();
	}

	if (vstParamsCC != nullptr)
	{
		removeChildControllableContainer(vstParamsCC.get());
		vstParamsCC.reset();
	}

	vstNotifier.addMessage(new VSTEvent(VSTEvent::VST_REMOVED, this));
	vstNotifier.triggerAsyncUpdate();

	presets.clear();

	if (description == nullptr)
	{
		vst.reset();
	}
	else
	{
		try
		{
			String errorMessage;
			int sampleRate = processor->getSampleRate() != 0 ? processor->getSampleRate() : Transport::getInstance()->sampleRate;
			int blockSize = processor->getBlockSize() != 0 ? processor->getBlockSize() : Transport::getInstance()->blockSize;

			jassert(sampleRate > 0 && blockSize > 0);

			vst = VSTManager::getInstance()->formatManager->createPluginInstance(*description, sampleRate, blockSize, errorMessage);

			if (errorMessage.isNotEmpty())
			{
				NLOGERROR(niceName, "VST Load error : " << errorMessage);
			}
		}
		catch (std::exception e)
		{
			NLOGERROR(niceName, "Error while loading plugin : " << e.what());
		}

		if (vst != nullptr)
		{
			vst->setPlayHead(Transport::getInstance());

		}

		setIOFromVST();

		if (vst != nullptr)
		{
			vstParamsCC.reset(new VSTParameterContainer(vst.get()));
			vstParamsCC->setMaxMacros(numMacros->intValue());
			addChildControllableContainer(vstParamsCC.get());
		}
	}

	isSettingVST = false;
	updatePlayConfig();
	updatePresetEnum();

	vstNotifier.addMessage(new VSTEvent(VSTEvent::VST_SET, this));
}

void VSTNode::setIOFromVST()
{
	if (vst != nullptr)
	{
		vst->enableAllBuses();
		auto layout = vst->getBusesLayout();
		int targetInputs = numAudioInputs->enabled ? numAudioInputs->intValue() : jmax(layout.getMainInputChannels(), vst->getTotalNumInputChannels());
		int targetOutputs = numAudioOutputs->enabled ? numAudioOutputs->intValue() : jmax(layout.getMainOutputChannels(), vst->getTotalNumOutputChannels());

		setAudioInputs(targetInputs);// vst->getTotalNumInputChannels());
		setAudioOutputs(targetOutputs);// vst->getTotalNumOutputChannels());
		setMIDIIO(vst->acceptsMidi(), vst->producesMidi());
	}
	else
	{
		setAudioInputs(2);//2 for basic setup without having to put a vst vst->getTotalNumInputChannels());
		setAudioOutputs(2);//2 for basic setup without having to put a vst// vst->getTotalNumOutputChannels());
		setMIDIIO(true, true); //for automatic connection
	}

}

String VSTNode::getVSTState()
{
	if (vst == nullptr) return "";

	MemoryBlock b;
	vst->getStateInformation(b);
	return b.toBase64Encoding();
}

void VSTNode::setVSTState(const String& data)
{
	if (vst == nullptr) return;
	if (data.isEmpty()) return;

	MemoryBlock b;
	if (b.fromBase64Encoding(data))
	{
		GenericScopedLock lock(vstStateLock);
		vst->setStateInformation(b.getData(), b.getSize());
	}
}

void VSTNode::updatePresetEnum(const String& setPresetName)
{
	String nameToChoose = setPresetName;
	if (nameToChoose.isEmpty())
	{
		if ((int)presetEnum->getValueData() < 1000) nameToChoose = presetEnum->getValueKey();
		else nameToChoose = "None";
	}

	presetEnum->clearOptions();
	presetEnum->addOption("None", 0, false);
	for (int i = 0; i < presets.size(); i++) presetEnum->addOption(presets[i]->name, i + 1);
	presetEnum->addOption("Save Current", 1000);
	presetEnum->addOption("Add New", 1001);
	presetEnum->addOption("Delete current", 1002);

	presetEnum->setValueWithKey(nameToChoose);
}

void VSTNode::updateMacros()
{
	if (vstParamsCC != nullptr) vstParamsCC->setMaxMacros(numMacros->intValue());

	while (macrosCC.controllables.size() > numMacros->intValue())
	{
		macrosCC.removeControllable(macrosCC.controllables[macrosCC.controllables.size() - 1]);
	}

	while (macrosCC.controllables.size() < numMacros->intValue())
	{
		FloatParameter* p = macrosCC.addFloatParameter("Macro " + String(macrosCC.controllables.size() + 1), "Macro", 0, 0, 1);
		p->isCustomizableByUser = true;
	}

	autoActivateMacroIndex->setRange(1, jmax(numMacros->intValue(), 2));
}

void VSTNode::checkAutoBypass()
{
	bool shouldBypass = false;
	if (autoActivateMacroIndex->enabled && autoActivateMacroIndex->intValue() <= macrosCC.controllables.size())
	{
		if (Parameter* p = ((Parameter*)macrosCC.controllables[autoActivateMacroIndex->intValue() - 1]))
		{
			float val = p->floatValue();
			shouldBypass |= val >= autoActivateRange->x && val <= autoActivateRange->y;
		}
	}

	if (disableOnDry->boolValue()) shouldBypass |= dryWet->floatValue() == 0;

	enabled->setValue(!shouldBypass);
}

void VSTNode::updatePlayConfigInternal()
{
	Node::updatePlayConfigInternal();

	if (vst != nullptr && !isSettingVST)
	{
		//int sampleRate = processor->getSampleRate() != 0 ? processor->getSampleRate() : Transport::getInstance()->sampleRate;
		//int blockSize = processor->getBlockSize() != 0 ? processor->getBlockSize() : Transport::getInstance()->blockSize;
		if (processor->getSampleRate() > 0 && processor->getBlockSize() > 0)
		{
			vst->setRateAndBufferSizeDetails(processor->getSampleRate(), processor->getBlockSize());
			vst->prepareToPlay(processor->getSampleRate(), processor->getBlockSize());

		}
	}
}

void VSTNode::onContainerParameterChangedInternal(Parameter* p)
{
	Node::onContainerParameterChangedInternal(p);

	if (p == pluginParam) setupVST(pluginParam->getPluginDescription());
	else if (p == numAudioInputs || p == numAudioOutputs) setIOFromVST();
	else if (p == presetEnum)
	{
		int d = presetEnum->getValueData();
		if (d == 1000)
		{
			if (currentPreset != nullptr)
			{
				currentPreset->data = getVSTState();
			}

			presetEnum->setValueWithKey(currentPreset != nullptr ? currentPreset->name : "None");
		}
		else if (d == 1001)
		{
			AlertWindow* nameWindow(new AlertWindow("Add a preset", "Set the name for the new preset", AlertWindow::AlertIconType::NoIcon));

			nameWindow->addTextEditor("name", "New preset", "Name");
			nameWindow->addButton("OK", 1, KeyPress(KeyPress::returnKey));
			nameWindow->addButton("Cancel", 0, KeyPress(KeyPress::escapeKey));

			nameWindow->enterModalState(true, ModalCallbackFunction::create([=](int result)
				{
					if (result)
					{
						String pName = nameWindow->getTextEditorContents("name");
						presets.add(new VSTPreset({ pName, getVSTState() }));
						updatePresetEnum(pName);

					}

					delete nameWindow;
				}));
		}
		else if (d == 1002)
		{
			presets.removeObject(currentPreset);
			currentPreset = nullptr;
			updatePresetEnum();
		}
		else if (d == 0)
		{
			currentPreset = nullptr;
		}
		else
		{
			currentPreset = presets[(int)presetEnum->getValueData() - 1];
			setVSTState(currentPreset->data);
		}
	}
	else if (p == numMacros)
	{
		updateMacros();
	}
	else if (p == autoActivateMacroIndex || p == autoActivateRange || p == dryWet || p == disableOnDry) checkAutoBypass();
}

void VSTNode::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
{
	Node::onControllableFeedbackUpdateInternal(cc, c);

	if (cc == vstParamsCC.get())
	{
		if (VSTParameterLink* pLink = dynamic_cast<VSTParameterLink*>(c))
		{
			if (pLink->macroIndex >= 0 && pLink->macroIndex < numMacros->intValue())
			{

				if (!antiMacroFeedback)
				{
					antiMacroFeedback = true;
					((FloatParameter*)macrosCC.controllables[pLink->macroIndex])->setValue(pLink->param->floatValue());
					antiMacroFeedback = false;
				}
			}
		}
	}
	else if (cc == &macrosCC)
	{
		int index = macrosCC.controllables.indexOf(c);
		if (autoActivateMacroIndex->enabled && index == autoActivateMacroIndex->intValue() - 1) checkAutoBypass();

		if (vstParamsCC != nullptr)
		{
			HashMap<int, VSTParameterLink*>::Iterator it(vstParamsCC->idParamMap);
			while (it.next())
			{
				if (it.getValue()->macroIndex == index) it.getValue()->param->setValue(((FloatParameter*)c)->floatValue());
			}
		}

	}
}

void VSTNode::onControllableStateChanged(Controllable* c)
{
	Node::onControllableStateChanged(c);
	if (c == numAudioInputs || c == numAudioOutputs) setIOFromVST();
	else if (c == autoActivateMacroIndex) autoActivateRange->setEnabled(autoActivateMacroIndex->enabled);
	if (c == autoActivateMacroIndex || c == autoActivateRange) checkAutoBypass();
}

void VSTNode::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
	if (vst != nullptr && sampleRate > 0 && maximumExpectedSamplesPerBlock > 0) vst->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
	if (sampleRate != 0) midiCollector.reset(sampleRate);
}

void VSTNode::processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	float weight = dryWet->floatValue();

	if (prevWetDry == weight)
	{
		if (weight == 1) processVSTBlock(buffer, midiMessages, false);
		else if (weight == 0) processBlockBypassed(buffer, midiMessages);
	}
	else
	{
		AudioBuffer<float> vstBuffer(buffer.getNumChannels(), buffer.getNumSamples());
		vstBuffer.clear();
		processVSTBlock(vstBuffer, midiMessages, false);

		buffer.applyGainRamp(0, buffer.getNumSamples(), 1 - prevWetDry, 1 - weight);

		for (int c = 0; c < buffer.getNumChannels(); c++) buffer.addFromWithRamp(c, 0, vstBuffer.getReadPointer(c), buffer.getNumSamples(), prevWetDry, weight);
	}

	prevWetDry = weight;

}

void VSTNode::processBlockBypassed(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	if (getNumAudioInputs() == 0) buffer.clear();

	processVSTBlock(buffer, midiMessages, true);
}

void VSTNode::processVSTBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages, bool bypassed)
{
	if (vst != nullptr)
	{
		if (!bypassed)
		{
			GenericScopedTryLock lock(vstStateLock);
			if (lock.isLocked()) vst->processBlock(buffer, midiMessages);
		}

	}
}

void VSTNode::bypassInternal()
{
	if (clearBufferOnDisable->boolValue())
	{
		if (vst != nullptr)
		{
			vst->reset();
		}
	}
}

var VSTNode::getJSONData()
{
	var data = Node::getJSONData();
	if (vst != nullptr) data.getDynamicObject()->setProperty("vstState", getVSTState());

	if (vstParamsCC != nullptr) data.getDynamicObject()->setProperty("vstParams", vstParamsCC->getJSONData());
	data.getDynamicObject()->setProperty("macros", macrosCC.getJSONData());

	if (presets.size() > 0)
	{
		var presetData = new DynamicObject();
		for (auto& p : presets) presetData.getDynamicObject()->setProperty(p->name, p->data);

		data.getDynamicObject()->setProperty("presets", presetData);
	}


	return data;
}

void VSTNode::loadJSONDataItemInternal(var data)
{
	Node::loadJSONDataItemInternal(data);

	setVSTState(data.getProperty("vstState", ""));

	if (vstParamsCC != nullptr) vstParamsCC->loadJSONData(data.getProperty("vstParams", var()));

	macrosCC.loadJSONData(data.getProperty("macros", var()));

	presets.clear();
	var presetData = data.getProperty("presets", var());
	if (presetData.isObject())
	{
		NamedValueSet pData = presetData.getDynamicObject()->getProperties();
		for (auto& pd : pData) presets.add(new VSTPreset({ pd.name.toString(), pd.value }));
	}
	updatePresetEnum();
}

BaseNodeViewUI* VSTNode::createViewUI()
{
	return new VSTNodeViewUI(this);
}
