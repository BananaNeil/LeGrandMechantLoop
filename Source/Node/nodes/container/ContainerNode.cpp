/*
  ==============================================================================

    ContainerNode.cpp
    Created: 3 Dec 2020 9:42:13am
    Author:  bkupe

  ==============================================================================
*/

#include "ContainerNode.h"
#include "../../NodeManager.h"
#include "ui/ContainerNodeUI.h"

ContainerProcessor::ContainerProcessor(Node* n) :
    GenericNodeProcessor(n, true, true, true, true),
    inputID(AudioProcessorGraph::NodeID(AudioManager::getInstance()->getNewGraphID())),
    outputID(AudioProcessorGraph::NodeID(AudioManager::getInstance()->getNewGraphID()))
{
    std::unique_ptr<AudioProcessorGraph::AudioGraphIOProcessor> procIn(new AudioProcessorGraph::AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode));
    std::unique_ptr<AudioProcessorGraph::AudioGraphIOProcessor> procOut(new AudioProcessorGraph::AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode));

    graph.addNode(std::move(procIn), inputID);
    graph.addNode(std::move(procOut), outputID);

    nodeManager.reset(new NodeManager(&graph, inputID, outputID));
    addChildControllableContainer(nodeManager.get());

    nodeManager->addBaseManagerListener(this);
}

ContainerProcessor::~ContainerProcessor()
{
}

void ContainerProcessor::clearProcessor()
{
    nodeManager->clear();
}

void ContainerProcessor::initInternal()
{
    updateGraph();
}

void ContainerProcessor::onContainerParameterChanged(Parameter* p)
{
    if(p == numAudioInputs) nodeRef->setAudioInputs(numAudioInputs->intValue());
    else if(p == numAudioOutputs) nodeRef->setAudioOutputs(numAudioOutputs->intValue());
    GenericNodeProcessor::onContainerParameterChanged(p);
}

void ContainerProcessor::itemAdded(Node* n)
{
    n->addNodeListener(this);
    updateGraph();
}

void ContainerProcessor::itemRemoved(Node* n)
{
    n->removeNodeListener(this);
}

void ContainerProcessor::updateGraph()
{
    graph.suspendProcessing(true);
    graph.setPlayConfigDetails(nodeRef->numInputs, nodeRef->numOutputs, getSampleRate(), getBlockSize());
    graph.prepareToPlay(getSampleRate(), getBlockSize());
    graph.suspendProcessing(false);
}


void ContainerProcessor::updateInputsFromNodeInternal()
{
    nodeManager->setAudioInputs(nodeRef->audioInputNames);
}

void ContainerProcessor::updateOutputsFromNodeInternal()
{
    nodeManager->setAudioOutputs(nodeRef->audioOutputNames);
}

void ContainerProcessor::nodePlayConfigUpdated(Node* n)
{
    updateGraph();
}

void ContainerProcessor::onControllableFeedbackUpdate(ControllableContainer* cc, Controllable* c)
{
    if (c == nodeManager->isPlaying) nodeRef->isNodePlaying->setValue(nodeManager->isPlaying->boolValue());
    GenericNodeProcessor::onControllableFeedbackUpdate(cc, c);
}

void ContainerProcessor::updatePlayConfig()
{
    updateGraph();
    GenericNodeProcessor::updatePlayConfig();
}

void ContainerProcessor::prepareToPlay(double sampleRate, int maxBlockSize)
{
}

void ContainerProcessor::processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    graph.processBlock(buffer, midiMessages);
}

var ContainerProcessor::getJSONData()
{
    var data = GenericNodeProcessor::getJSONData();
    data.getDynamicObject()->setProperty("manager", nodeManager->getJSONData());
    return data;
}

void ContainerProcessor::loadJSONDataInternal(var data)
{
    nodeManager->loadJSONData(data.getProperty("manager", var()));
    GenericNodeProcessor::loadJSONDataInternal(data);
}

NodeViewUI* ContainerProcessor::createNodeViewUI()
{
    return new ContainerNodeViewUI(((GenericNode<ContainerProcessor>*)nodeRef.get()));
}
