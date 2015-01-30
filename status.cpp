#include "status.h"

Status::Status() {
    logging = false;
    connection = DISCONNECTED;
    emulation = D200;
    control_pressed = false;
    shift_pressed = false;
    holding = false;
    dirty = true;
}
