/*
  ==============================================================================

    ModulatorsListModel.cpp
    Created: 30 Oct 2021 12:29:38am
    Author:  Dan German

  ==============================================================================
*/

#include "gui/modulators_list_model.h"
#include "model/ModuleParameter.h"
#include "lfo_module_new.h"
#include "module_new.h"

double getAdjustedValue(double value, vital::ValueDetails details) { // this should be a static method in ValueDetails?? 
  double adjusted_value = value;
  switch (details.value_scale) {
  case vital::ValueDetails::kQuadratic:
    adjusted_value *= adjusted_value;
    break;
  case vital::ValueDetails::kCubic:
    adjusted_value *= adjusted_value * adjusted_value;
    break;
  case vital::ValueDetails::kQuartic:
    adjusted_value *= adjusted_value;
    adjusted_value *= adjusted_value;
    break;
  case vital::ValueDetails::kExponential:
    adjusted_value = powf(2.0, adjusted_value); //display_exponential_base_ will be different for each module. i think just env?
    break;
  case vital::ValueDetails::kSquareRoot:
    adjusted_value = sqrtf(std::max(adjusted_value, 0.0));
    break;
  default:
    break;
  }

  float display_multiply_ = 1.0f;

  adjusted_value += details.post_offset;
  if (details.display_invert)
    adjusted_value = 1.0 / adjusted_value;
  if (display_multiply_)
    adjusted_value *= display_multiply_;
  else
    adjusted_value *= details.display_multiply;

  return adjusted_value;
}

String formatValue(float value, vital::ValueDetails details) {
  int max_decimal_places = details.decimal_places;
  String format;
  if (details.value_scale == vital::ValueDetails::kIndexed)
    format = String(value);
  else {
    if (max_decimal_places == 0)
      format = String(std::roundf(value));
    else
    format = String(value, 3);

    int display_characters = 5;//max_display_characters_;
    if (format[0] == '-')
      display_characters += 1;

    format = format.substring(0, display_characters);
    if (format.getLastCharacter() == '.')
      format = format.removeCharacters(".");
  }

  // if (use_suffix_)
  //   return format + getDisplayDetails()->display_units;

  return format;
}

String getSliderTextFromValue(double value, vital::ValueDetails details) {
  if (details.string_lookup) {
    int lookup = vital::utils::iclamp(value, 0, details.max);
    return details.string_lookup[lookup];
  }

  double adjusted_value = getAdjustedValue(value, details);
  // return String(adjusted_value);
  return formatValue(adjusted_value, details);
}

int ModulatorsListModel::getNumRows() { return modulators.size(); }
void ModulatorsListModel::listBoxItemDoubleClicked(int row, const MouseEvent& event) { ListBoxModel::listBoxItemDoubleClicked(row, event); }
var ModulatorsListModel::getDragSourceDescription(const SparseSet<int>& rowsToDescribe) { return ListBoxModel::getDragSourceDescription(rowsToDescribe); }
void ModulatorsListModel::remove(int index) {
  modulators.erase(modulators.begin() + index);
}

Component* ModulatorsListModel::refreshComponentForRow(int rowNumber, bool isRowSelected, Component* existingComponentToUpdate) {
  ModulatorComponent* component;

  if (auto castComponent = static_cast<ModulatorComponent*>(existingComponentToUpdate))
    component = castComponent;
  else
    component = new ModulatorComponent();

  if (modulators.size() == 0) return component;
  if (rowNumber >= modulators.size()) return component;

  setupModulatorComponent(modulators[rowNumber], *component);
  component->row = rowNumber;

  return component;
}

void ModulatorsListModel::setupModulatorComponent(std::shared_ptr<model::Module> model, ModulatorComponent& component) const {
  component.title.setText(model->display_name, dontSendNotification);

  for (int i = 0; i < model->parameters_.size(); i++) {
    auto parameter = model->parameters_[i];
    if (parameter->hidden) continue;
    auto slider = component.sliders[i];

    auto value = parameter->val->value();
    slider->label.setText(parameter->display_name, dontSendNotification);

    double interval = 0.0;
    if (parameter->value_scale == ValueScale::kLinear) {
      slider->box_slider_.slider.setNumDecimalPlacesToDisplay(3);
    } else if (parameter->value_scale == ValueScale::kExponential) {
      // slider->boxSlider.slider.setSkewFactor(4, false);
    } else if (parameter->value_scale == ValueScale::kIndexed) {
      interval = 1.0;
      slider->box_slider_.slider.setNumDecimalPlacesToDisplay(0);
    }

    slider->box_slider_.slider.setRange(parameter->min, parameter->max, interval);

    if (parameter->string_lookup) {
      slider->box_slider_.slider.textFromValueFunction = [parameter](double value) { return juce::String(parameter->string_lookup[(int)value]); };
    } else {
      slider->box_slider_.slider.textFromValueFunction = {};
    }

    slider->box_slider_.slider.setValue(value, dontSendNotification);
    // slider->boxSlider.slider.getTextFromValue(value);
    // slider->boxSlider.valueLabel.setText(slider->boxSlider.slider.getTextFromValue(value), dontSendNotification);
  }

  component.delegate_ = modulator_listener;
  component.setColour(model->colour.colour);

  if (model->id.type == Model::Types::lfo) {
    component.oscillatorPainter.setVisible(true);
    // auto parameter = model.parameter(Model::LFOModule::Parameters::pWaveform);
    // auto value = parameter->audioParameter->getNormalisableRange().convertFrom0to1(parameter->audioParameter->getValue());
    // component.oscillatorPainter.setWaveformType(static_cast<OscillatorPainter::WaveformType>((int)value));
    // component.envelopePath.setVisible(false);
  } else {
    component.oscillatorPainter.setVisible(false);
    component.envelopePath.setVisible(true);
  }

  // todo: update the skew only after rate value changes... atm it looks weird
  if (model->id.type == Model::Types::lfo) {
    component.onSliderValueChange = [model, &component, this](int index, float value) { this->onLFOParameterChange(model, component, index, value); };
  } else if (model->id.type == Model::Types::adsr) {
    component.onSliderValueChange = [&component, &model](int index, float value) {
      // auto normalizedValue = model.parameters[index]->audioParameter->convertTo0to1(value);
      // switch (index) {
      // case 0: component.envelopePath.setAttack(normalizedValue); break;
      // case 1: component.envelopePath.setDecay(normalizedValue); break;
      // case 2: component.envelopePath.setSustain(normalizedValue); break;
      // case 3: component.envelopePath.setRelease(normalizedValue); break;
      // default: break;
      // }
    };
  }
}

void ModulatorsListModel::setModulators(std::vector<std::shared_ptr<model::Module>> modulators) {
  this->modulators.clear();
  this->modulators = modulators;
}

void ModulatorsListModel::onLFOParameterChange(std::shared_ptr<model::Module> module, ModulatorComponent& component, int index, float value) const {
  bool is_changing_sync = index == 2;
  if (is_changing_sync) {
    LabeledSlider* rate_slider = component.sliders[1];
    bool is_seconds = int(value) == 0;
    if (is_seconds) {
      setSliderAsFrequency(module, rate_slider);
    } else {
      setSliderAsTempo(module, rate_slider);
    }
  }

  bool is_changing_temp = index == 1;
  bool is_not_seconds = int(component.sliders[2]->box_slider_.slider.getValue()) != 0;
  if (is_changing_temp && is_not_seconds) {
    auto value = component.sliders[1]->box_slider_.slider.getValue();
    std::string value_string = std::to_string(value);
    auto integer_part_length = value_string.substr(0, value_string.find(".")).size();
    component.sliders[1]->box_slider_.slider.setNumDecimalPlacesToDisplay(4 - integer_part_length);
  }
}

void ModulatorsListModel::setSliderAsFrequency(std::shared_ptr<model::Module> module, LabeledSlider* slider) const {
  slider->label.setText("seconds", dontSendNotification);

  slider->box_slider_.slider.textFromValueFunction = [slider, module](double value) {
    // // auto value = component.sliders[1]->box_slider_.slider.getValue();
    // std::string value_string = std::to_string(value);
    // auto integer_part_length = value_string.substr(0, value_string.find(".")).size();
    // slider->box_slider_.slider.setNumDecimalPlacesToDisplay(4 - integer_part_length);
    return getSliderTextFromValue(value, *(module->parameter_map_["frequency"]));
  };

  slider->box_slider_.slider.setRange(-7.0, 9.0);
  auto value = module->parameter_map_["frequency"]->val->value();
  slider->box_slider_.slider.setValue(value, dontSendNotification);
  slider->box_slider_.valueLabel.setText(slider->box_slider_.slider.getTextFromValue(value), dontSendNotification);
}

void ModulatorsListModel::setSliderAsTempo(std::shared_ptr<model::Module> module, LabeledSlider* slider) const {
  slider->label.setText("tempo", dontSendNotification);
  slider->box_slider_.slider.textFromValueFunction = [](double value) { return strings::kSyncedFrequencyNames[int(value)]; };
  slider->box_slider_.slider.setRange(0.0, 12.0, 1.0);
  auto value = module->parameter_map_["tempo"]->val->value();
  slider->box_slider_.slider.setValue(value, dontSendNotification);
  slider->box_slider_.valueLabel.setText(slider->box_slider_.slider.getTextFromValue(value), dontSendNotification);
}