
#include "gui/FilterResponseComponent.h"
#include "util/Utils.h"



FilterResponseComponent::FilterResponseComponent(): responseColour(Colours::white.withAlpha(0.9f)) {
  setInterceptsMouseClicks(false, false);
  start();
  for (int i = 0; i < 4; i++)
    paths[i] = Path();
}

FilterResponseComponent::~FilterResponseComponent() { }

void FilterResponseComponent::resized() {}

void FilterResponseComponent::paint(juce::Graphics& g) {
  int componentCenterY = getHeight() / 2;
  int componentCenterX = getWidth() / 2;
  //g.setColour(responseColour.withAlpha(alpha));
  //g.drawRect(componentCenterX, componentCenterY, getHeight()*2, getWidth()*2);

  for (int i = 0; i < 4; i++) {
      float xOffset = i * 3.0f;
      float yOffset = i * 1.5f;

      for (float x = xOffset; x < getWidth(); x += resolution) {
      float width = (float)getWidth();
      float pi = static_cast<const float>(std::acos(-1));
      float fullCircleInRadians = 2 * pi;
      float unit = fullCircleInRadians / width * 3.0f;
      float y = sin(unit * (x + offset));
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

  for (int i = 1-1; i >= 0; i--) {
    float alpha = std::lerp(1.0f, 0.01f, i / float(1));
    g.setColour(responseColour.withAlpha(0.9f));
    g.strokePath(paths[i], PathStrokeType(thickness));
    paths[i].clear();
  }
}


void FilterResponseComponent::update(const float secondsSinceLastUpdate) {
  offset += 30.0f * secondsSinceLastUpdate;
  repaint();
}

void FilterResponseComponent::setWaveformType() {
  setFPS(60);
  repaint();
}