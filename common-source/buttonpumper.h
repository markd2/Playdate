#ifndef BORK_BUTTONPUMPER_H
#define BORK_BUTTONPUMPER_H

#include <stdlib.h>
#include <stdbool.h>

#include "pd_api.h"


// --------------------------------------------------
// Support types

typedef enum {
    kPressed,
    kReleased
} UpDown;

typedef void ButtonPumperCallback(PDButtons, UpDown, void*);
typedef struct ButtonPumper ButtonPumper;


// --------------------------------------------------
// API

// Create a new pumper.
// Give it a callback that'll get called any time a button has changed state
// from up or down.
//
// Use like:
//    void pumpCallback(PDButtons button, UpDown state) {
//        ... look at the button, and if pressed/released
//    } // pumpCallback
//
//    ButtonPumper *pumper = buttonPumperNew(pumpCallback);
//
ButtonPumper *buttonPumperNew(ButtonPumperCallback *callback, void *context);

// Clean up a pumper when it's no longer useful
void buttonPumperDelete(ButtonPumper *pumper);

// let the pumper know the current state of pushed and released buttons.
// It will then call the callback on buttons that have changed state since the
// last pump.
//
// Use like:
//     pd->system->getButtonState(NULL, &pushed, &released);
//     buttonPumperPump(pumper, pushed, released);
//
void buttonPumperPump(ButtonPumper *pumper, PDButtons pushed, PDButtons released);


bool buttonPumperButtonIsDown(ButtonPumper *pumper, PDButtons button);


// --------------------------------------------------
// innards

struct ButtonPumper {
    PDButtons _lastPushed;
    ButtonPumperCallback *_callback;
    void *_context;
};

#endif // BORK_BUTTONPUMPER_H
