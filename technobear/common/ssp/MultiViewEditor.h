#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "BaseEditor.h"


using namespace juce;

namespace ssp {

class BaseProcessor;

class BaseParamControl;

class ButtonControl;

class MultiViewEditor :
    public BaseEditor {

public:
    explicit MultiViewEditor(BaseProcessor *p,
                             String title, String version,
                             unsigned maxView);

    void onEncoder(unsigned enc, float v) override;
    void onEncoderSwitch(unsigned enc, bool v) override;
    void onButton(unsigned btn, bool v) override;

    void onUpButton(bool v) override;
    void onDownButton(bool v) override;
    void onLeftButton(bool v) override;
    void onRightButton(bool v) override;
    void setParamBounds(unsigned idx, std::shared_ptr<BaseParamControl>);


protected:
    using base_type = BaseEditor;

    virtual void chgParamPage(int delta, bool changeVis = false);
    virtual void setParamPage(unsigned p, bool changeVis = false);
    virtual void chgButtonPage(int delta);
    virtual void setButtonPage(unsigned p);
    virtual void chgView(int delta);
    virtual void setView(unsigned v);

    virtual ControlPage addParamPage(
        std::shared_ptr<BaseParamControl> c1,
        std::shared_ptr<BaseParamControl> c2,
        std::shared_ptr<BaseParamControl> c3,
        std::shared_ptr<BaseParamControl> c4,
        unsigned view
    );

    void addButtonPage(
        std::shared_ptr<ButtonControl> c1,
        std::shared_ptr<ButtonControl> c2,
        std::shared_ptr<ButtonControl> c3,
        std::shared_ptr<ButtonControl> c4,
        std::shared_ptr<ButtonControl> c5,
        std::shared_ptr<ButtonControl> c6,
        std::shared_ptr<ButtonControl> c7,
        std::shared_ptr<ButtonControl> c8,
        unsigned view
    );

    struct View {
        std::vector<ControlPage> controlPages_;
        std::vector<ButtonPage> buttonPages_;
    };
    std::vector<View> views_;

    bool encoderFine[4] = {false, false, false, false};

    unsigned view_ = 0;
    unsigned paramPage_ = 0;
    unsigned buttonPage_ = 0;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiViewEditor)
};

}




