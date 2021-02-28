
#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "ssp/BaseParamControl.h"
#include "ssp/ButtonControl.h"

using pcontrol_type = ssp::BarParamControl;
using bcontrol_type = ssp::ButtonControl;

PluginEditor::PluginEditor(PluginProcessor &p)
    : base_type(&p,
                String(JucePlugin_Name) + " : " + String(JucePlugin_Desc),
                JucePlugin_VersionString),
      processor_(p) {

    addParamPage(
        std::make_shared<pcontrol_type>(processor_.params_.inSel),
        std::make_shared<pcontrol_type>(processor_.params_.outSel),
        nullptr,
        nullptr
    );

    addButtonPage(
        std::make_shared<bcontrol_type>(processor_.params_.active, 24, Colours::lightskyblue),
        std::make_shared<bcontrol_type>(processor_.params_.soft, 24, Colours::orange),
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr
    );

    setSize(1600, 480);
}

void PluginEditor::drawGrid(Graphics &g) {
    int x = 1000;
    int y = 30;
    int lx = 100;
    int fh = 18;
    int sz = fh;
    int sp = fh;

    unsigned insel, outsel;
    processor_.getLastSel(insel, outsel);

    g.setFont(Font(Font::getDefaultMonospacedFontName(), fh, Font::plain));
    g.setColour(Colours::red);
    g.drawText("Outputs", x + lx, y, (8 * sz) + (7 * sp), fh, Justification::centred);
    y += sp;
    g.drawText("Inputs", x, y, lx, fh, Justification::left);
    for (unsigned xi = 0; xi < PluginProcessor::MAX_SIG_OUT; xi++) {
        if (processor_.isOutputEnabled(xi)) {
            g.setColour(Colours::red);
        } else {
            g.setColour(Colours::grey);
        }
        char chout[2] = "A";
        chout[0] = 'A' + xi;
        g.drawText(String(chout), x + lx + xi * (sz + sp), y, sp + sz, fh, Justification::left);
    }
    y += sp;

    for (unsigned yi = 0; yi < PluginProcessor::MAX_SIG_IN; yi++) {
        if (processor_.isInputEnabled(yi)) {
            g.setColour(Colours::red);
        } else {
            g.setColour(Colours::grey);
        }
        g.drawText(String(yi + 1), x, y, lx, fh, Justification::left);
        for (unsigned xi = 0; xi < PluginProcessor::MAX_SIG_OUT; xi++) {
            g.setColour(Colours::white);

            if (xi == outsel && yi == insel) {
                g.fillRect(x + lx + xi * (sz + sp), y, sz, sz);
            } else {
                g.drawRect(x + lx + xi * (sz + sp), y, sz, sz, 1);
            }
        }
        y += sp + sz;
    }
}

void PluginEditor::paint(Graphics &g) {
    base_type::paint(g);
    drawGrid(g);
}

