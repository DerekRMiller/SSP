#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Percussa.h"

#include <atomic>

#include "plaits/dsp/voice.h"

inline float constrain(float v, float vMin, float vMax) {
    return std::max<float>(vMin, std::min<float>(vMax, v));
}

static constexpr unsigned PltsBlock = 16;

struct PltsData {
    PltsData() {
        pitch_ = 0.0f;
        harmonics_ = 0.0f;
        timbre_ = 0.5f;
        morph_ = 0.5f;

        model_ = 0.0f;
        lpg_colour_ = 0.5f;
        decay_ = 0.5f;

        freq_mod_=0.0f;
        timbre_mod_=0.0f;
        morph_mod_=0.0f;

        trig_ = 0;
    }
    ~PltsData() {
    }

    plaits::Voice voice;
    plaits::Patch patch;
    char shared_buffer[16384];

    std::atomic<float>  pitch_;
    std::atomic<float>  harmonics_;
    std::atomic<float>  timbre_;
    std::atomic<float>  morph_;

    std::atomic<float>  model_;
    std::atomic<float>  lpg_colour_;
    std::atomic<float>  decay_;

    std::atomic<float>  fm_;
    std::atomic<float>  trig_;
    std::atomic<float>  level_;

    std::atomic<float>  freq_mod_;
    std::atomic<float>  timbre_mod_;
    std::atomic<float>  morph_mod_;
};


class Plts  : public AudioProcessor
{
public:
    Plts();
    ~Plts();

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const String getName() const override;

    int getNumParameters() override;
    float getParameter (int index) override;
    void setParameter (int index, float newValue) override;

    const String getParameterName (int index) override;
    const String getParameterText (int index) override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool silenceInProducesSilenceOut() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    PltsData& data() { return data_;}


private:
    void writeToXml(juce::XmlElement& xml);
    void readFromXml(juce::XmlElement& xml);
    enum {
        I_VOCT,
        I_TRIG,
        I_LEVEL,
        I_HARMONICS,
        I_TIMBRE,
        I_MORPH,
        I_FM,
        I_MODEL,

        I_MAX
    };
    enum {
        O_OUT,
        O_AUX,
        O_MAX
    };

    PltsData data_;
    float params_[Percussa::sspLast];

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override { return true;}
    static const String getInputBusName (int channelIndex);
    static const String getOutputBusName (int channelIndex);
    static BusesProperties getBusesProperties()
    {
        BusesProperties props;
        for (auto i = 0; i < I_MAX; i++) {
            props.addBus(true, getInputBusName(i), AudioChannelSet::mono());
        }
        for (auto i = 0; i < O_MAX; i++) {
            props.addBus(false, getOutputBusName(i), AudioChannelSet::mono());
        }
        return props;
    }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Plts)
};


