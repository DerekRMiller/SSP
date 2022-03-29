#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "PluginProcessor.h"
#include "ssp/MultiBarEditor.h"

class PluginEditor : public ssp::MultiBarEditor {
public:
    explicit PluginEditor(PluginProcessor &);
    ~PluginEditor() override = default;

protected:
    using base_type = ssp::MultiBarEditor;

    void drawView(Graphics &) override;
    void resized() override;

    void onLeftShiftButton(bool v) override;
    void onRightShiftButton(bool v) override;
    void onUpButton(bool v) override;
    void onDownButton(bool v) override;
    void onLeftButton(bool v) override;
    void onRightButton(bool v) override;
private:

    PluginProcessor &processor_;

    static constexpr unsigned L_CLRS = 3;

    unsigned currentLayer_=0;

    enum CVButtonMode {
        B_GATEACCESS,
        B_GLIDE,
        MAX_CV_BUTTON_MODE
    } cvButtonMode_;

    enum EncoderMode {
        E_CV,
        E_FUN,
        MAX_ENCODER_MODE
    } encoderMode_;


    juce::Colour clrs_[L_CLRS];
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};



