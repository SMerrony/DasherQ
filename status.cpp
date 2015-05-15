#include "status.h"

Status::Status() {
    logging = false;
    connection = DISCONNECTED;
    emulation = D200;
    visLines = 24;
    visCols = 80;
    holding = false;
    dirty = true;
}
