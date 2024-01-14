
#include "gui/FilterResponseComponent.h"
#include "util/Utils.h"



FilterResponseComponent::FilterResponseComponent(): responseColour(Colours::white.withAlpha(0.9f)) {
  setInterceptsMouseClicks(false, false);
  start();
}

FilterResponseComponent::~FilterResponseComponent() { }

void FilterResponseComponent::resized() {}

void FilterResponseComponent::paint(juce::Graphics& g) {
  int componentCenterY = getHeight() / 2;
  int componentCenterX = getWidth() / 2;
  float halfHeight = getHeight() / 2.0f - thickness;
  Rectangle<int> mywindow = getBounds().removeFromLeft(getWidth()/3);
  g.setColour(juce::Colours::green);
  auto offset = 60.0f;
  auto scaledCutoff = (cutoff-offset)/10.0f;
  for (float x = 0; x < getWidth(); x += resolution) {
        float cutoffPoint = (float) componentCenterX + scaledCutoff;
        float y = componentCenterY - halfHeight;
        auto point = Point<float>(x, y);
        if (x == 0){
            responsePath.startNewSubPath(point);
        } else if (x <= cutoffPoint){
          if (abs(x - cutoffPoint) < resolution) {
            point.setY(componentCenterY + halfHeight);
          }
          responsePath.lineTo(point);
        }     
  }
  g.strokePath(responsePath, PathStrokeType(thickness));
  responsePath.clear();
}


void FilterResponseComponent::update(const float secondsSinceLastUpdate) {
  offset += 30.0f * secondsSinceLastUpdate;
  repaint();
}

void FilterResponseComponent::setFilterType(int filtertype){
  switch(filtertype)
  {
    case 0:
      filtermodel = "analog";
      break;
    case 1:
      filtermodel = "dirty";
      break;
    case 2:
      filtermodel = "ladder";
      break;
    case 3:
      filtermodel = "digital";
      break;
    case 4:
      filtermodel = "diode";
      break;
    case 5:
      filtermodel = "formant";
      break;
    case 6:
      filtermodel = "comb";
      break;
    case 7:
      filtermodel = "phaser";
      break;
    default:
      break;

  }
}