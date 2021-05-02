
#include "ValueButton.h"

ValueButton::ValueButton(const String &label,
                         std::function<void(bool v)> cb,
                         const Colour fg, const Colour bg,
                         bool def)
    : label_(label), value_(def), callback_(cb), fg_(fg), bg_(bg) {
}

void ValueButton::valueChanged(bool b) {
    repaint();
    if(callback_) callback_(b);
}

void ValueButton::value(bool v) {
    if (v != value_) {
        value_ = v;
        valueChanged(value_);
    }

}

bool ValueButton::toggle() {
    value(!value_);
    return value_;
}


void ValueButton::paint(Graphics &g) {
    const int w = getWidth();
    const int h = getHeight();
    g.setFont(Font(Font::getDefaultMonospacedFontName(), 36, Font::plain));


    if (!value_) {
        g.setColour(bg_);
        g.fillRect(0, 0 + 1, w - 2, h - 2);
        g.setColour(fg_);
    } else {
        g.setColour(fg_);
        g.fillRect(0, 0 + 1, w - 2, h - 2);
        g.setColour(bg_);
    }

    g.drawText(label_, 0, 0, w, h, Justification::centred);
}

