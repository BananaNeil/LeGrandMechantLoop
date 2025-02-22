/*
  ==============================================================================

	AudioManager.h
	Created: 15 Nov 2020 8:44:58am
	Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

#define AUDIO_GRAPH_INPUT_ID 1
#define AUDIO_GRAPH_OUTPUT_ID 2
#define MIDI_GRAPH_INPUT_ID 3
#define MIDI_GRAPH_OUTPUT_ID 4
#define GRAPH_START_ID 5


class AudioManager :
	public ControllableContainer,
	public AudioIODeviceCallback,
	public AudioIODeviceType::Listener,
	public ChangeListener,
	public EngineListener
{
public:
	juce_DeclareSingleton(AudioManager, true);

	AudioManager();
	~AudioManager();

	AudioDeviceManager am;
	AudioProcessorGraph graph;
	AudioProcessorPlayer player;
	int graphIDIncrement; //This will be incremented and assign to each node that is created, in the node constructor

	double currentSampleRate;
	int currentBufferSize;

	int numAudioInputs;
	int numAudioOutputs;

	std::unique_ptr<XmlElement> lastUserState;
	String targetDeviceName;

	int getNewGraphID();

	void updateGraph();


	virtual void audioDeviceIOCallbackWithContext
#if RPISAFEMODE
	(const float** inputChannelData,
		int numInputChannels,
		float** outputChannelData,
		int numOutputChannels,
		int numSamples,
		const AudioIODeviceCallbackContext& context) override;
#else
		//7.0.3
		(const float* const* inputChannelData,
			int numInputChannels,
			float* const* outputChannelData,
			int numOutputChannels,
			int numSamples,
			const AudioIODeviceCallbackContext& context) override;
#endif


	virtual void loadAudioConfig();

	virtual void audioDeviceAboutToStart(AudioIODevice* device) override;
	virtual void audioDeviceStopped() override;

	virtual void audioDeviceListChanged() override;
	virtual void changeListenerCallback(ChangeBroadcaster* source) override;

	StringArray getInputChannelNames() const;
	StringArray getOutputChannelNames() const;

	void stop();

	void startLoadFile() override;
	void endLoadFile() override;

	String getCurrentDeviceDescription();

	class AudioManagerListener
	{
	public:
		virtual ~AudioManagerListener() {}
		virtual void audioSetupChanged() {}
	};

	ListenerList<AudioManagerListener> audioManagerListeners;
	void addAudioManagerListener(AudioManagerListener* newListener) { audioManagerListeners.add(newListener); }
	void removeAudioManagerListener(AudioManagerListener* listener) { audioManagerListeners.remove(listener); }

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	InspectableEditor* getEditorInternal(bool isRoot, Array<Inspectable*> inspectables = {}) override;
};