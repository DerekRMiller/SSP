#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "ssp/BaseParamControl.h"
#include "ssp/ButtonControl.h"

//using pcontrol_type = ssp::SimpleParamControl;
//using pcontrol_type = ssp::LineParamControl;
using pcontrol_type = ssp::BarParamControl;
using bcontrol_type = ssp::ButtonControl;

PluginEditor::PluginEditor(PluginProcessor &p)
    : base_type(&p,
                String(JucePlugin_Name) + " : " + String(JucePlugin_Desc),
                JucePlugin_VersionString),
      processor_(p) {

    addParamPage(
        std::make_shared<pcontrol_type>(processor_.params_.position),
        std::make_shared<pcontrol_type>(processor_.params_.size),
        std::make_shared<pcontrol_type>(processor_.params_.density),
        std::make_shared<pcontrol_type>(processor_.params_.texture)
    );

    addParamPage(
        std::make_shared<pcontrol_type>(processor_.params_.mix),
        std::make_shared<pcontrol_type>(processor_.params_.spread),
        std::make_shared<pcontrol_type>(processor_.params_.feedback),
        std::make_shared<pcontrol_type>(processor_.params_.reverb)
    );

    addParamPage(
        std::make_shared<pcontrol_type>(processor_.params_.pitch),
        std::make_shared<pcontrol_type>(processor_.params_.mode, 1.0f, 1.0f),
        std::make_shared<pcontrol_type>(processor_.params_.in_gain),
        nullptr
    );


    addButtonPage(
        std::make_shared<bcontrol_type>(processor_.params_.freeze, 24, Colours::lightskyblue),
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr
    );

//    //TODO - need for resize to be called
//    // but perhaps resize functionality should be moved
//    // as part of init?

    inVu_.init("In");
    outVu_.init("Out");

    float inL, inR, outL, outR;
    processor_.getRMS(inL, inR, outL, outR);
    inVu_.level(inL, inR);
    outVu_.level(outL, outR);

    addAndMakeVisible(inVu_);
    addAndMakeVisible(outVu_);

    setSize(1600, 480);
}

void PluginEditor::timerCallback() {
    base_type::timerCallback();
}

void PluginEditor::onEncoder(unsigned enc, float v) {
    base_type::onEncoder(enc, v);
}

void PluginEditor::onEncoderSwitch(unsigned enc, bool v) {
    base_type::onEncoderSwitch(enc, v);
}


void PluginEditor::onButton(unsigned int btn, bool v) {
    base_type::onButton(btn, v);
}


void PluginEditor::paint(Graphics &g) {
    float inL, inR, outL, outR;
    processor_.getRMS(inL, inR, outL, outR);
    inVu_.level(inL, inR);
    outVu_.level(outL, outR);

    base_type::paint(g);
}


void PluginEditor::resized() {
    const unsigned h = 150;
    const unsigned sp = 10;
    const unsigned vuStart = 1500;
    const unsigned vuW = 45;
    unsigned y = 50;
    unsigned x = vuStart;
    inVu_.setBounds(x, y, vuW, h);
    x += vuW + sp;
    outVu_.setBounds(x, y, vuW, h);
}
