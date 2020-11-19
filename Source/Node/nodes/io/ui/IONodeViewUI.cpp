/*
  ==============================================================================

    IONodeViewUI.cpp
    Created: 15 Nov 2020 11:39:57pm
    Author:  bkupe

  ==============================================================================
*/

#include "IONodeViewUI.h"

IONodeViewUI::IONodeViewUI(GenericAudioNode<IOProcessor>* n) :
    GenericAudioNodeViewUI(n)
{
    updateUI();
}

IONodeViewUI::~IONodeViewUI()
{
}

void IONodeViewUI::nodeInputsChanged()
{
    NodeViewUI::nodeInputsChanged();
    if (!audioNode->processor->isInput) updateUI();
}

void IONodeViewUI::nodeOutputsChanged()
{
    NodeViewUI::nodeOutputsChanged();
    if(audioNode->processor->isInput) updateUI();
}

void IONodeViewUI::updateUI()
{
    for (auto& i : rmsUI) removeChildComponent(i);
    for (auto& i : gainUI) removeChildComponent(i);
    rmsUI.clear();
    gainUI.clear();

    int numChannels = audioNode->processor->isInput ? audioNode->numOutputs : audioNode->numInputs;

    for (int i = 0; i < numChannels; i++)
    {
        FloatSliderUI* s = ((FloatParameter *)processor->rmsCC.controllables[i])->createSlider();
        s->orientation = s->VERTICAL;
        s->showLabel = false;
        s->showValue = false;
        addAndMakeVisible(s);
        rmsUI.add(s);

        FloatSliderUI* gs = ((FloatParameter*)processor->gainCC.controllables[i])->createSlider();
        gs->orientation = gs->VERTICAL;
        addAndMakeVisible(gs);
        gainUI.add(gs);
    }

    resized();
}

void IONodeViewUI::resizedInternalContent(Rectangle<int>& r)
{
    for (int i=0;i<rmsUI.size();i++)
    {
        Rectangle<int> trackR = r.removeFromLeft(40).reduced(6);
        Rectangle<int> btR = trackR.removeFromBottom(20);
        trackR.removeFromBottom(2);

        rmsUI[i]->setBounds(trackR.removeFromRight(8));
        trackR.removeFromRight(2);
        gainUI[i]->setBounds(trackR);
    }
}