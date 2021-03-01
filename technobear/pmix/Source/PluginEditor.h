#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "PluginProcessor.h"
#include "ssp/BaseEditor.h"
#include "ssp/VuMeter.h"
#include "MonoChannel.h"
#include "StereoChannel.h"

class PluginEditor : public ssp::BaseEditor {
public:
    explicit PluginEditor(PluginProcessor &);
    ~PluginEditor() override = default;

    void paint(Graphics &) override;
    void resized() override;
    void timerCallback() override;

    void onEncoder(unsigned enc, float v) override;
    void onEncoderSwitch(unsigned enc, bool v) override;
    void onButton(unsigned btn, bool v) override;
    void onLeftButton(bool v) override;
    void onRightButton(bool v) override;
    void onUpButton(bool v) override;
    void onDownButton(bool v) override;
    void onLeftShiftButton(bool v) override;
    void onRightShiftButton(bool v) override;
protected:
    using base_type = ssp::BaseEditor;

private:
    void channelEncoderButton(unsigned c, bool v);
    void channelEncoder(unsigned c, float v);
    void channelButton(unsigned c, unsigned i, bool v);
    bool buttonState(unsigned c, unsigned i);
    void labelButtons();

    enum TrackSelect {
        IN_14,
        IN_58,
        OUT_14,
        TS_MAX
    } activeTracks_, curTracks_;

    void trackSelect(TrackSelect ts, bool active);

    unsigned butMode_;
    unsigned encMode_;


    static constexpr unsigned POLL_TIME = 50; // mSec
    static constexpr unsigned BUT_COUNTER = POLL_TIME * 10; // 0.5 sec
    unsigned buttonHeldCount_[TS_MAX];


    MonoChannel inTracks_[PluginProcessor::IN_T_MAX];
    StereoChannel outTracks_[(PluginProcessor::OUT_T_MAX / 2)];


    SSPParam params_[4];

    PluginProcessor &processor_;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};

