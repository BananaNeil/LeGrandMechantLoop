/*
  ==============================================================================

    SpatItemUI.cpp
    Created: 21 Nov 2020 6:32:53pm
    Author:  bkupe

  ==============================================================================
*/

#include "SpatItemUI.h"

SpatItemUI::SpatItemUI(SpatItem* item) :
    InspectableContentComponent(item),
    item(item)
{
    setRepaintsOnMouseActivity(true);
    item->addAsyncContainerListener(this);
}

SpatItemUI::~SpatItemUI()
{
    if (!inspectable.wasObjectDeleted()) item->removeAsyncContainerListener(this);
}

void SpatItemUI::mouseDown(const MouseEvent& e)
{
    posAtMouseDown = item->position->getPoint();
}

bool SpatItemUI::hitTest(int x, int y)
{
    Rectangle<int> center = getLocalBounds().withSizeKeepingCentre(20,20);
    return center.contains(x, y);
}

void SpatItemUI::paint(Graphics& g)
{
    
    float size = item->weight->floatValue() * getWidth();
    g.setColour(NORMAL_COLOR.withAlpha(.4f));
    g.fillEllipse(getLocalBounds().toFloat().withSizeKeepingCentre(size, size));

    g.setColour(NORMAL_COLOR.withAlpha(.6f));
    g.drawEllipse(getLocalBounds().toFloat(),1);

    Rectangle<float> center = getLocalBounds().withSizeKeepingCentre(16, 16).toFloat();
    Colour c = isMouseOverOrDragging() ? HIGHLIGHT_COLOR : NORMAL_COLOR;
    g.setColour(c.withAlpha(.6f));
    g.fillEllipse(center);

    g.setColour(TEXT_COLOR);
    g.drawText(String(item->index + 1), center, Justification::centred);
}

void SpatItemUI::newMessage(const ContainerAsyncEvent& e)
{
    if (e.targetControllable == item->weight)
    {
        repaint();
    }
}
