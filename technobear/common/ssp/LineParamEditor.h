#pragma once

#include "BaseEditor.h"

namespace ssp {

class LineParamEditor : public BaseEditor {
public:
    LineParamEditor(BaseProcessor *p, String title, String version);

protected:
    using base_type=ssp::BaseEditor;

    void paint(Graphics &) override;

    void onUpButton(bool v) override;
    void onDownButton(bool v) override;
    void setParamBounds(unsigned idx, std::shared_ptr<BaseParamControl>);
    ControlPage addParamPage(
        std::shared_ptr<BaseParamControl> c1,
        std::shared_ptr<BaseParamControl> c2,
        std::shared_ptr<BaseParamControl> c3,
        std::shared_ptr<BaseParamControl> c4
    ) override;

};

}