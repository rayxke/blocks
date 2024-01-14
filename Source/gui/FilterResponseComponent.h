#pragma once

#include "settings/Constants.h"
#include "gui/GraphicsTimer.h"

class FilterResponseComponent: public Component, GraphicsTimer {
public:

  FilterResponseComponent();
  ~FilterResponseComponent() override;

  void paint(juce::Graphics&) override;
  void resized() override;
  

  Colour responseColour;
  float thickness = 2.5f;
  void setFilterType(int);
  float cutoff = 30.0f;
  float resonance = 3.0f;
private:
  float resolution = 0.5f;
  Path responsePath;
  float offset = 0;
  std::string filtermodel;

  void update(const float secondsSinceLastUpdate) override;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterResponseComponent)
};
