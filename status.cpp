#include "status.h"

Status::Status() {
    logging = false;
    connection = DISCONNECTED;
    emulation = D200;
    holding = false;
    dirty = true;
}
