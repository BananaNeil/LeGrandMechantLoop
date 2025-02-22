/*
  ==============================================================================

	OSCInterface.h
	Created: 29 Oct 2016 7:07:07pm
	Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "servus/servus.h"
using namespace servus;

class OSCOutput :
	public BaseItem,
	public Thread
{
public:
	OSCOutput();
	~OSCOutput();

	bool forceDisabled;
	bool senderIsConnected;

	//SEND
	BoolParameter* useLocal;
	StringParameter* remoteHost;
	IntParameter* remotePort;
	BoolParameter* listenToOutputFeedback;
	std::unique_ptr<OSCReceiver> receiver;
	std::unique_ptr<DatagramSocket> socket;


	void setForceDisabled(bool value);

	virtual void setupSender();
	void sendOSC(const OSCMessage& m);

	virtual void run() override;

	void onContainerParameterChangedInternal(Parameter* p) override;

	//virtual InspectableEditor* getEditor(bool isRoot) override;

private:
	OSCSender sender;
	std::queue<std::unique_ptr<OSCMessage>> messageQueue;
	CriticalSection queueLock;
};

class OSCInterface :
	public Interface,
	public OSCReceiver::Listener<OSCReceiver::RealtimeCallback>,
	public Thread, //for zeroconf async creation (smoother when creating an OSC module)
	public BaseManager<OSCOutput>::ManagerListener
{
public:
	OSCInterface(var params = var());
	~OSCInterface();


	//RECEIVE
	IntParameter* localPort;
	BoolParameter* isConnected;
	OSCReceiver receiver;
	OSCSender genericSender;

	//ZEROCONF
	Servus servus;

	std::unique_ptr<EnablingControllableContainer> receiveCC;
	std::unique_ptr<BaseManager<OSCOutput>> outputManager;

	//Script
	const Identifier oscEventId = "oscEvent";

	//RECEIVE
	virtual void setupReceiver();
	float getFloatArg(OSCArgument a);
	int getIntArg(OSCArgument a);
	String getStringArg(OSCArgument a);
	Colour getColorArg(OSCArgument a);

	void processMessage(const OSCMessage& msg);
	virtual void processMessageInternal(const OSCMessage&) {}

	void itemAdded(OSCOutput* output) override;

	//SEND
	virtual void setupSenders();
	virtual void sendOSC(const OSCMessage& msg, String ip = "", int port = 0);

	//ZEROCONF
	void setupZeroConf();

	//Script
	static var sendOSCFromScript(const var::NativeFunctionArgs& args);
	static var sendOSCToFromScript(const var::NativeFunctionArgs& args);
	static var matchOSCAddrFromScript(const var::NativeFunctionArgs& args);
	static var registerOSCCallbackFromScript(const var::NativeFunctionArgs& args);

	//Thru
	static void createThruControllable(ControllableContainer* cc);


	static OSCArgument varToArgument(const var& v);
	static OSCArgument varToColorArgument(const var& v);
	static var argumentToVar(const OSCArgument& a);


	//save / load
	virtual var getJSONData() override;
	virtual void loadJSONDataInternal(var data) override;

	virtual void onContainerParameterChangedInternal(Parameter* p) override;
	virtual void onContainerNiceNameChanged() override;
	virtual void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) override;
	virtual void oscMessageReceived(const OSCMessage& message) override;
	virtual void oscBundleReceived(const OSCBundle& bundle) override;


	// Inherited via Thread
	virtual void run() override;

	//InspectableEditor * getEditor(bool isRoot) override;	

	DECLARE_TYPE("OSC")

private:
	Array<std::tuple<OSCAddressPattern, Identifier>> scriptCallbacks;
};