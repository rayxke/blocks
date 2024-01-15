#pragma once

#include "settings/Constants.h"
#include "gui/GraphicsTimer.h"

class FilterResponseComponent: public Component, GraphicsTimer {
public:
  enum class FilterStyle { dB_12, dB_24, blend, spread, B_P_N };
  FilterStyle filterStyle;
  enum class FilterModel { analog, dirty, ladder, digital, diode, formant, comb, phaser};
  FilterModel filterModel;

  FilterResponseComponent();
  ~FilterResponseComponent() override;

  void paint(juce::Graphics&) override;
  void resized() override;
  

  Colour responseColour;
  float thickness = 2.5f;
  void setFilterType(int);
  float cutoff = 30.0f;
  float resonance = 3.0f;
  float blend = 0.0f;
  float q = 50.0f;
  float mix = 1.0f;

private:
  float resolution = 0.5f;
  Path responsePath;
  float offset = 0;
  std::string filtermodel;

  void update(const float secondsSinceLastUpdate) override;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterResponseComponent)
};
