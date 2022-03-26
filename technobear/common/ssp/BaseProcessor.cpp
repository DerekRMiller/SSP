#include "BaseProcessor.h"

#include <juce_core/juce_core.h>
#include <juce_audio_devices/juce_audio_devices.h>

#include <assert.h>

namespace ssp {

#ifdef __APPLE__
static constexpr bool defIOState = true;
#else
// linux etc
static constexpr bool defIOState = false;
#endif


BaseProcessor::BaseProcessor(
    const AudioProcessor::BusesProperties &ioLayouts,
    juce::AudioProcessorValueTreeState::ParameterLayout pl)
    : AudioProcessor(ioLayouts), apvts(*this, nullptr, "state", std::move(pl)) {
    addListener(this);
}

BaseProcessor::~BaseProcessor() {
    if (midiInDevice_) {
        midiInDevice_->stop();
    }
    if (midiOutDevice_) {
        midiOutDevice_->stopBackgroundThread();
    }
    removeListener(this);
}

void BaseProcessor::prepareToPlay(double newSampleRate, int estimatedSamplesPerBlock) {
    ;
}

void BaseProcessor::releaseResources() {
    if (midiInDevice_) {
        midiInDevice_->stop();
        midiOutDevice_ = nullptr;
    }
    if (midiOutDevice_) {
        midiOutDevice_->stopBackgroundThread();
        midiOutDevice_ = nullptr;
    }
}


void BaseProcessor::addBaseParameters(AudioProcessorValueTreeState::ParameterLayout &params) {
}

void BaseProcessor::init() {
    for (unsigned i = 0; i < numOut; i++) {
        onOutputChanged(i, defIOState);
    }
    for (unsigned i = 0; i < numIn; i++) {
        onInputChanged(i, defIOState);
    }
}


void BaseProcessor::MidiAutomation::store(XmlElement *xml) {
    xml->setAttribute("paramId", paramIdx_);
    xml->setAttribute("scale", scale_);
    xml->setAttribute("offset", offset_);
    xml->setAttribute("midi.channel", midi_.channel_);
    xml->setAttribute("midi.num", midi_.num_);
    xml->setAttribute("midi.type", midi_.type_);
}

void BaseProcessor::MidiAutomation::recall(XmlElement *xml) {
    paramIdx_ = xml->getIntAttribute("paramId", -1);
    scale_ = xml->getDoubleAttribute("scale", 1.0f);
    offset_ = xml->getDoubleAttribute("offset", 0.0f);
    midi_.channel_ = xml->getIntAttribute("midi.channel", 0);
    midi_.num_ = xml->getIntAttribute("midi.num", -1);
    midi_.type_ = static_cast<Midi::Type>(xml->getIntAttribute("midi.type", Midi::T_MAX));
}

static const char *MIDI_TAG_IN_DEV = "MIDI_IN";
static const char *MIDI_TAG_OUT_DEV = "MIDI_OUT";
static const char *MIDI_TAG_CHANNEL = "MIDI_CH";
static const char *MIDI_TAG_NOTE_INPUT = "MIDI_NOTE_IN";

void BaseProcessor::midiFromXml(juce::XmlElement *xml) {
    auto midiin = xml->getStringAttribute(MIDI_TAG_IN_DEV).toStdString();
    auto midiout = xml->getStringAttribute(MIDI_TAG_OUT_DEV).toStdString();
    setMidiIn(midiin);
    setMidiOut(midiout);

    midiChannel_ = xml->getIntAttribute(MIDI_TAG_CHANNEL, 0);
    noteInput_ = xml->getBoolAttribute(MIDI_TAG_NOTE_INPUT, false);

    midiAutomation_.clear();
    auto amXml = xml->getChildByName("Automation");
    if (amXml) {
        for (int idx = 0; idx < amXml->getNumChildElements(); idx++) {
            auto pxml = amXml->getChildElement(idx);
            if (pxml) {
                MidiAutomation am;
                am.recall(pxml);
                if (am.valid()) {
                    midiAutomation_[am.paramIdx_] = am;
                }
            }
        }
    }
}

void BaseProcessor::midiToXml(juce::XmlElement *xml) {
    xml->setAttribute(MIDI_TAG_IN_DEV, midiInDeviceId_);
    xml->setAttribute(MIDI_TAG_OUT_DEV, midiOutDeviceId_);
    xml->setAttribute(MIDI_TAG_CHANNEL, midiChannel_);
    xml->setAttribute(MIDI_TAG_NOTE_INPUT, noteInput_);

    auto amXml = xml->createNewChildElement("Automation");
    for (auto &ap: midiAutomation_) {
        auto &a = ap.second;
        if (a.paramIdx_ != -1) {
            auto pxml = amXml->createNewChildElement("P_" + String(a.paramIdx_));
            a.store(pxml);
        }
    }
}


void BaseProcessor::customFromXml(juce::XmlElement *xml) {

}

void BaseProcessor::customToXml(juce::XmlElement *xml) {

}

static const char *VST_XML_TAG = "VST";
static const char *MIDI_XML_TAG = "MIDI";
static const char *CUSTOM_XML_TAG = "CUSTOM";


void BaseProcessor::getStateInformation(MemoryBlock &destData) {
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xmlVst = std::make_unique<XmlElement>(VST_XML_TAG);

    // first save state tree
    std::unique_ptr<juce::XmlElement> xmlState(state.createXml());

    // clean this xml by removing SSP parameters (encoders and alike)
    std::unique_ptr<XmlElement> cleanStateXml = std::make_unique<XmlElement>(xmlState->getTagName());
    for (int i = 0; i < xmlState->getNumChildElements(); i++) {
        XmlElement *p = xmlState->getChildElement(i);
        XmlElement *ne = new XmlElement(*p);
        cleanStateXml->addChildElement(ne);
    }


    std::unique_ptr<juce::XmlElement> xmlMidi = std::make_unique<XmlElement>(MIDI_XML_TAG);
    std::unique_ptr<juce::XmlElement> xmlCustom = std::make_unique<XmlElement>(CUSTOM_XML_TAG);


    midiToXml(xmlMidi.get());
    customToXml(xmlCustom.get());

    if (xmlState) xmlVst->addChildElement(cleanStateXml.release());
    if (xmlMidi) xmlVst->addChildElement(xmlMidi.release());
    if (xmlCustom) xmlVst->addChildElement(xmlCustom.release());
    copyXmlToBinary(*xmlVst, destData);
}


void BaseProcessor::setStateInformation(const void *data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml.get() != nullptr) {
        if (xml->hasTagName(apvts.state.getType())) {
            // backwards compat
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
        } else if (xml->hasTagName(VST_XML_TAG)) {
            auto xmlMidi = xml->getChildByName(MIDI_XML_TAG);
            if (xmlMidi != nullptr) midiFromXml(xmlMidi);

            auto xmlCustom = xml->getChildByName(CUSTOM_XML_TAG);
            if (xmlCustom != nullptr) customFromXml(xmlCustom);

            auto xmlState = xml->getChildByName(apvts.state.getType()); //STATE
            if (xmlState != nullptr) apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}


void BaseProcessor::onInputChanged(unsigned i, bool b) {
    if (i < numIn) inputEnabled[i] = b;
}

void BaseProcessor::onOutputChanged(unsigned i, bool b) {
    if (i < numOut) outputEnabled[i] = b;
}

void BaseProcessor::setMidiIn(std::string id) {
    if (id == midiInDeviceId_) return;

    if (!id.empty()) {
        if (midiInDevice_) {
            midiInDevice_->stop();
            midiInDevice_ = nullptr;
        }

        midiInDevice_ = MidiInput::openDevice(id, this);
        if (midiInDevice_ && midiInDevice_->getIdentifier().toStdString() == id) {
            midiInDevice_->start();
            Logger::writeToLog(getName() + ": MIDI IN OPEN -> " + id);
            midiInDeviceId_ = id;
        } else {
            Logger::writeToLog(getName() + ": MIDI IN FAILED -> " + id);
        }
    } else {
        midiInDevice_ = nullptr;
    }
}

void BaseProcessor::setMidiOut(std::string id) {
    if (id == midiOutDeviceId_) return;

    if (!id.empty()) {
        if (midiOutDevice_) {
            midiOutDevice_->stopBackgroundThread();
            midiOutDevice_ = nullptr;
        }

        midiOutDevice_ = MidiOutput::openDevice(id);
        if (midiOutDevice_ && midiOutDevice_->getIdentifier().toStdString() == id) {
            midiOutDevice_->startBackgroundThread();
            Logger::writeToLog(getName() + ": MIDI OUT OPEN -> " + id);
            midiOutDeviceId_ = id;
        } else {
            Logger::writeToLog(getName() + ": MIDI OUT FAILED -> " + id);
        }
    } else {
        midiOutDevice_ = nullptr;
    }
}

void BaseProcessor::automateParam(int idx, const MidiAutomation &a, const MidiMessage &msg) {
    auto &plist = getParameters();
    if (idx < plist.size()) {
        auto p = plist[idx];

        float val = p->getValue();
        if (msg.isController()) {
            val = float(msg.getControllerValue()) / 127.0f;
        } else if (msg.isNoteOn()) {
            val = msg.getFloatVelocity();
        } else if (msg.isNoteOff()) {
            val = 0.0f;
        }

        val = (val * a.scale_) + a.offset_;

        if (p->getValue() != val) {
            p->beginChangeGesture();
            p->setValueNotifyingHost(val);
            p->endChangeGesture();
        }
    }
}

void BaseProcessor::handleMidi(const MidiMessage &msg) {
    if (midiChannel_ == 0 || msg.getChannel() == midiChannel_) {
        if (midiLearn_) {
            if (lastLearn_.paramIdx_ >= 0) {
                if (msg.isController()) {
                    auto &m = lastLearn_.midi_;
                    m.type_ = MidiAutomation::Midi::T_CC;
                    m.num_ = msg.getControllerNumber();
                    m.channel_ = msg.getChannel();

                    midiAutomation_[lastLearn_.paramIdx_] = lastLearn_;
                    lastLearn_.reset();
                }
            }
        }

        for (auto &ap: midiAutomation_) {
            auto &a = ap.second;
            if ((msg.isController() && a.midi_.type_ == MidiAutomation::Midi::T_CC)
                && (msg.getControllerNumber() == a.midi_.num_)
                ) {
                automateParam(a.paramIdx_, a, msg);
            } else if ((msg.isNoteOnOrOff() && a.midi_.type_ == MidiAutomation::Midi::T_NOTE)
                       && (msg.getNoteNumber() == a.midi_.num_)) {
                automateParam(a.paramIdx_, a, msg);
            }
        }

        if (noteInput_ && msg.isNoteOnOrOff()) {
            if (msg.isNoteOn()) {
                midiNoteInput(msg.getNoteNumber(), msg.getVelocity());
            } else {
                midiNoteInput(msg.getNoteNumber(), 0);
            }
        }
    }
}


void BaseProcessor::handleIncomingMidiMessage(MidiInput *source, const MidiMessage &message) {
//    Logger::writeToLog("MidiCallback -> " + message.getDescription());
    handleMidi(message);
}


void BaseProcessor::midiLearn(bool b) {
    if (midiLearn_ != b) {
        midiLearn_ = b;
        if (midiLearn_) {
            lastLearn_.reset();
        }
    }
}

void BaseProcessor::audioProcessorParameterChanged(AudioProcessor *processor,
                                                   int parameterIndex,
                                                   float v) {

    assert(processor == this);
    if (midiLearn_) {
        lastLearn_.paramIdx_ = parameterIndex;
    } else {
        if (midiOutDevice_ != nullptr) {
            if (midiAutomation_.find(parameterIndex) != midiAutomation_.end()) {
                auto &a = midiAutomation_[parameterIndex];
                switch (a.midi_.type_) {
                    case MidiAutomation::Midi::T_CC : {
                        auto msg = MidiMessage::controllerEvent(a.midi_.channel_, a.midi_.num_, uint8(v * 127));
                        midiOutDevice_->sendMessageNow(msg);
                        break;
                    }
                    case MidiAutomation::Midi::T_NOTE : {
                        if (v > 0.0f) {
                            auto msg = MidiMessage::noteOn(a.midi_.channel_, a.midi_.num_, uint8(v * 127));
                            midiOutDevice_->sendMessageNow(msg);
                        } else {
                            auto msg = MidiMessage::noteOff(a.midi_.channel_, a.midi_.num_);
                            midiOutDevice_->sendMessageNow(msg);
                        }
                        break;
                    }
                    case MidiAutomation::Midi::T_PRESSURE : {
                        auto msg = MidiMessage::channelPressureChange(a.midi_.channel_, uint8(v * 127));
                        midiOutDevice_->sendMessageNow(msg);
                        break;
                    }
                    default: {
                        break;
                    }
                }
            }
        }
    }
}

}
