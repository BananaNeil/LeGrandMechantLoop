/*
  ==============================================================================

    LooperNodeViewUI.h
    Created: 15 Nov 2020 8:43:15am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class LooperNodeViewUI :
    public NodeViewUI<LooperNode>
{
public:
    LooperNodeViewUI(LooperNode * n);
    ~LooperNodeViewUI();

    std::unique_ptr<TriggerButtonUI> recUI;
    std::unique_ptr<TriggerButtonUI> clearUI;

    std::unique_ptr<TriggerButtonUI> clearAllUI;
    std::unique_ptr<TriggerButtonUI> stopAllUI;
    std::unique_ptr<TriggerButtonUI> playAllUI;

    void updateTracksUI();

    std::unique_ptr<TargetParameterUI> midiParamUI;
    OwnedArray<LooperTrackUI> tracksUI;

    void controllableFeedbackUpdateInternal(Controllable* c) override;

    virtual void viewFilterUpdated() override;

    void resizedInternalHeader(Rectangle<int>& r) override;
    void resizedInternalContentNode(Rectangle<int> &r) override;
};
