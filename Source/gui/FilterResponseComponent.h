#pragma once

#include "settings/Constants.h"
#include "gui/GraphicsTimer.h"

class FilterResponseComponent: public Component, GraphicsTimer {
public:

  FilterResponseComponent();
  ~FilterResponseComponent() override;

  void paint(juce::Graphics&) override;
  void resized() override;
  void setUnison(int unison);

  Colour responseColour;
  float thickness = 2.5f;

  float cutoff = 30.0f;
  float resonance = 3.0f;
private:
  float resolution = 0.5f;
  Path path;
  float offset = 0;

  void update(const float secondsSinceLastUpdate) override;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterResponseComponent)
};
