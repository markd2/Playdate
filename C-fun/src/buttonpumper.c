#include "buttonpumper.h"

#include "memory.h"


ButtonPumper *buttonPumperNew(ButtonPumperCallback *callback) {
    ButtonPumper *moi = pdMalloc(sizeof(ButtonPumper));

    moi->_lastPushed = 0;
    moi->_callback = callback;

    return moi;
} // new


void buttonPumperPump(ButtonPumper *moi, PDButtons pushed, PDButtons released) {
    PDButtons buttons[] = {
        kButtonLeft,
        kButtonRight,
        kButtonUp,
        kButtonDown,
        kButtonB,
        kButtonA
    };
    for (int i = 0; i < sizeof(buttons) / sizeof(*buttons); i++) {
        int mask = buttons[i];

        if (pushed & mask) {
            // if not pushed
            if (!(moi->_lastPushed & mask)) {
                moi->_lastPushed |= mask;
                moi->_callback(mask, kPressed);
                continue;
            }
        }

        if (released & mask) {
            // if pushed
            if (moi->_lastPushed & mask) {
                moi->_lastPushed &= ~mask;
                moi->_callback(mask, kReleased);
                continue;
            }
        }
    }
    
} // pump
