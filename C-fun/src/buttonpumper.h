#include <stdlib.h>
#include "pd_api.h"

typedef enum {
    kPressed,
    kReleased
} UpDown;

typedef void ButtonPumperCallback(PDButtons, UpDown);

typedef struct ButtonPumper ButtonPumper;

ButtonPumper *buttonPumperNew(ButtonPumperCallback *callback);
void buttonPumperPump(ButtonPumper *pumper, PDButtons pushed, PDButtons released);


// --------------------------------------------------

struct ButtonPumper {
    PDButtons _lastPushed;
    ButtonPumperCallback *_callback;
};

