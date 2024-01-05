
#include "gui/FilterResponseComponent.h"



FilterResponseComponent::FilterResponseComponent(): responseColour(Colours::white.withAlpha(0.15f)) {
  setInterceptsMouseClicks(false, false);
  start();
}

void FilterResponseComponent::resized() {}

void FilterResponseComponent::paint(juce::Graphics& g) {
  int componentCenterY = getHeight() / 2;

  for (int i = 0; i < 4; i++) {
      float xOffset = i * 3.0f;
      float yOffset = i * 1.5f;

      for (float x = xOffset; x < getWidth(); x += resolution) {
      float y = x + offset;
      float halfHeight = getHeight() / 2.0f - thickness;
      float mappedY = jmap(y, -1.0f, 1.0f, componentCenterY - halfHeight, componentCenterY + halfHeight);

      auto point = Point<float>(x - xOffset, mappedY + yOffset);

      if (x == xOffset) { 
          paths[i].startNewSubPath(point);
      } else {
          paths[i].lineTo(point);
      }
  }  
}

  for (int i = 4 - 1; i >= 0; i--) {
    float alpha = std::lerp(1.0f, 0.01f, i / float(4));

    g.setColour(responseColour.withAlpha(alpha));
    g.strokePath(paths[i], PathStrokeType(thickness));
    paths[i].clear();
  }
}


void FilterResponseComponent::update(const float secondsSinceLastUpdate) {
}