/*
  ==============================================================================

    NodeConnection.h
    Created: 16 Nov 2020 10:00:05am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "Node/Node.h"

class NodeConnection :
    public BaseItem,
    public Inspectable::InspectableListener,
    public Node::NodeListener
{
public:
    enum ConnectionType { AUDIO, MIDI };
    NodeConnection(Node * sourceNode = nullptr, Node * destNode = nullptr, ConnectionType ct = AUDIO);
    virtual ~NodeConnection();

    ConnectionType connectionType;
    Node * sourceNode;
    Node * destNode;

    virtual void setSourceNode(Node* node);
    virtual void setDestNode(Node* node);

    virtual void handleNodesUpdated();

    void inspectableDestroyed(Inspectable *) override;

    var getJSONData() override;
    void loadJSONDataItemInternal(var data) override;
    virtual void loadJSONDataConnectionInternal(var data) {}


    
    DECLARE_ASYNC_EVENT(NodeConnection, Connection, connection, ENUM_LIST(SOURCE_NODE_CHANGED, DEST_NODE_CHANGED, CHANNELS_CONNECTION_CHANGED))
};

class NodeAudioConnection :
    public NodeConnection
{
public:
    NodeAudioConnection(Node* sourceNode = nullptr, Node* destNode = nullptr);
    ~NodeAudioConnection();

    struct ChannelMap {
        int sourceChannel;
        int destChannel;

        inline bool operator == (const ChannelMap& c2) const { return sourceChannel == c2.sourceChannel && destChannel == c2.destChannel; }
    };
    Array<ChannelMap> channelMap;
    Array<ChannelMap> ghostChannelMap;

    void clearItem() override;
   
    void handleNodesUpdated() override;

    void connectChannels(int sourceChannel, int destChannel);
    void disconnectChannels(int sourceChannel, int destChannel, bool updateMap = true, bool notify = true);
    void createDefaultConnections();
    void clearConnections();

    void updateConnections();

    void audioInputsChanged(Node* n) override;
    void audioOutputsChanged(Node* n) override;

    var getJSONData() override;
    void loadJSONDataConnectionInternal(var data) override;

    var getChannelMapData();
    void loadChannelMapData(var data);

    class AudioConnectionListener
    {
    public:
        virtual ~AudioConnectionListener() {}
        virtual void askForConnectChannels(NodeConnection * nc, int sourceChannel, int destChannel) {}
        virtual void askForDisconnectChannels(NodeConnection* nc, int sourceChannel, int destChannel) {}
    };

    ListenerList<AudioConnectionListener> audioConnectionListeners;
    void addAudioConnectionListener(AudioConnectionListener* newListener) { audioConnectionListeners.add(newListener); }
    void removeAudioConnectionListener(AudioConnectionListener* listener) { audioConnectionListeners.remove(listener); }

    InspectableEditor* getEditor(bool isRoot) override;

};

class NodeMIDIConnection :
    public NodeConnection
{
public:
    NodeMIDIConnection(Node* sourceNode = nullptr, Node* destNode = nullptr);
    ~NodeMIDIConnection();

    void clearItem() override;

};