#pragma once


#include "../JuceLibraryCode/JuceHeader.h"

#include "PluginProcessor.h"
#include "ssp/BarParamEditor.h"
#include "ssp/VuMeter.h"

class PluginEditor : public ssp::BarParamEditor {
public:
    explicit PluginEditor(PluginProcessor &);
    ~PluginEditor() override = default;

    void paint(Graphics &) override;
    void resized() override;
protected:
    using base_type = ssp::BarParamEditor;
private:
    ssp::StereoVuMeter outVu_;

    PluginProcessor &processor_;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};

