/*
  ==============================================================================

    VSTNodeViewUI.h
    Created: 15 Nov 2020 11:41:16pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class PluginWindow :
    public DocumentWindow,
    public VSTNode::AsyncListener
{
public:
    PluginWindow(VSTNode* processor);
    ~PluginWindow();

    WeakReference<Inspectable> inspectable;
    VSTNode* node;
    std::unique_ptr<AudioProcessorEditor> editor;

    void setVSTEditor(AudioPluginInstance * vstInstance);
    void userTriedToCloseWindow() override;
    void closeButtonPressed() override;
    void resized() override;

    void newMessage(const VSTNode::VSTEvent& e) override;

    class PluginWindowListener
    {
    public:
        virtual ~PluginWindowListener() {}
        virtual void windowClosed() {}
    };

    ListenerList<PluginWindowListener> pwListeners;
    void addPluginWindowListener(PluginWindowListener* newListener) { pwListeners.add(newListener); }
    void removePluginWindowListener(PluginWindowListener* listener) { pwListeners.remove(listener); }
};

class VSTNodeViewUI :
    public NodeViewUI<VSTNode>,
    public PluginWindow::PluginWindowListener
{
public:
    VSTNodeViewUI(VSTNode * n);
    ~VSTNodeViewUI();

    std::unique_ptr<ImageButton> editHeaderBT;
    std::unique_ptr<TargetParameterUI> midiParamUI;
    std::unique_ptr<VSTPluginParameterUI> pluginUI;
    std::unique_ptr<PluginWindow> pluginEditor;

    Rectangle<int> pluginEditorBounds;
    
    void resizedInternalHeader(Rectangle<int>& r) override;
    void resizedInternalContentNode(Rectangle<int>& r) override;
    void controllableFeedbackUpdateInternal(Controllable* c) override;
    void buttonClicked(Button* b) override;

    void windowClosed() override;
};
