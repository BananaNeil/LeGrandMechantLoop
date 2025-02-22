/*
  ==============================================================================

	MIDIInterface.h
	Created: 20 Dec 2016 12:35:26pm
	Author:  Ben

  ==============================================================================
*/

#pragma once

class MIDIInterface :
	public Interface,
	public MidiInputCallback
{
public:
	MIDIInterface(var params = var());
	virtual ~MIDIInterface();

	MIDIDeviceParameter* midiParam;

	MIDIInputDevice* inputDevice;
	MIDIOutputDevice* outputDevice;

	MIDIClock clock;
	BoolParameter* enableClock;
	BoolParameter* autoFeedback;


	BoolParameter* isConnected;


	//Script
	const Identifier noteOnEventId = "noteOnEvent";
	const Identifier noteOffEventId = "noteOffEvent";
	const Identifier ccEventId = "ccEvent";
	const Identifier sysexEventId = "sysExEvent";
	const Identifier pitchWheelEventId = "pitchWheelEvent";
	const Identifier channelPressureId = "channelPressureEvent";
	const Identifier afterTouchId = "afterTouchEvent";

	const Identifier sendNoteOnId = "sendNoteOn";
	const Identifier sendNoteOffId = "sendNoteOff";
	const Identifier sendCCId = "sendCC";
	const Identifier sendSysexId = "sendSysex";
	const Identifier sendProgramChangeId = "sendProgramChange";
	const Identifier sendPitchWheelId = "sendPitchWheel";
	const Identifier sendChannelPressureId = "sendChannelPressure";
	const Identifier sendAfterTouchId = "sendAfterTouch";

	bool useGenericControls;

	virtual void clearItem() override;

	virtual void sendMessage(const MidiMessage& m);
	virtual void sendMidiBuffer(const MidiBuffer& buffer);
	virtual void sendNoteOn(int channel, int pitch, int velocity);
	virtual void sendNoteOff(int channel, int pitch);
	virtual void sendControlChange(int channel, int number, int value);
	virtual void sendSysex(Array<uint8> data);
	virtual void sendProgramChange(int channel, int number);
	virtual void sendPitchWheel(int channel, int value);
	virtual void sendChannelPressure(int channel, int value);
	virtual void sendAfterTouch(int channel, int note, int value);
	virtual void sendFullFrameTimecode(int hours, int minutes, int seconds, int frames, MidiMessage::SmpteTimecodeType timecodeType);

	void sendMidiMachineControlCommand(MidiMessage::MidiMachineControlCommand command);

	void onContainerParameterChangedInternal(Parameter* p) override;
	void updateMIDIDevices();

	virtual void handleIncomingMidiMessage(MidiInput* source,
		const MidiMessage& message) override;

	virtual void noteOnReceived(const int& channel, const int& pitch, const int& velocity);
	virtual void noteOffReceived(const int& channel, const int& pitch, const int& velocity);
	virtual void controlChangeReceived(const int& channel, const int& number, const int& value);
	virtual void sysExReceived(const MidiMessage& msg);
	virtual void fullFrameTimecodeReceived(const MidiMessage& msg);
	virtual void pitchWheelReceived(const int& channel, const int& value);
	virtual void channelPressureReceived(const int& channel, const int& value);
	virtual void afterTouchReceived(const int& channel, const int& note, const int& value);
	virtual void midiMessageReceived(const MidiMessage& msg);

	//Script
	static var sendNoteOnFromScript(const var::NativeFunctionArgs& args);
	static var sendNoteOffFromScript(const var::NativeFunctionArgs& args);
	static var sendCCFromScript(const var::NativeFunctionArgs& args);
	static var sendSysexFromScript(const var::NativeFunctionArgs& args);
	static var sendProgramChangeFromScript(const var::NativeFunctionArgs& args);
	static var sendPitchWheelFromScript(const var::NativeFunctionArgs& args);
	static var sendChannelPressureFromScript(const var::NativeFunctionArgs& args);
	static var sendAfterTouchFromScript(const var::NativeFunctionArgs& args);


	void loadJSONDataInternal(var data) override;


	class MIDIInterfaceListener
	{
	public:
		virtual ~MIDIInterfaceListener() {}
		virtual void deviceChanged(MIDIInterface*) {}
		virtual void noteOnReceived(MIDIInterface*, const int& channel, const int& pitch, const int& velocity) {}
		virtual void noteOffReceived(MIDIInterface*, const int& channel, const int& pitch, const int& velocity) {}
		virtual void controlChangeReceived(MIDIInterface*, const int& channel, const int& number, const int& value) {}
		virtual void sysExReceived(MIDIInterface*, const MidiMessage& msg) {}
		virtual void fullFrameTimecodeReceived(MIDIInterface*, const MidiMessage& msg) {}
		virtual void pitchWheelReceived(MIDIInterface*, const int& channel, const int& value) {}
		virtual void channelPressureReceived(MIDIInterface*, const int& channel, const int& value) {}
		virtual void afterTouchReceived(MIDIInterface*, const int& channel, const int& note, const int& value) {}
		virtual void midiMessageReceived(MIDIInterface*, const MidiMessage& msg) {}
	};

	ListenerList<MIDIInterfaceListener> midiInterfaceListeners;
	void addMIDIInterfaceListener(MIDIInterfaceListener* newListener) { midiInterfaceListeners.add(newListener); }
	void removeMIDIInterfaceListener(MIDIInterfaceListener* listener) { midiInterfaceListeners.remove(listener); }



	DECLARE_TYPE("MIDI")

	//InspectableEditor * getEditor(bool isRoot) override;
};