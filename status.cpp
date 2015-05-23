#include "status.h"
#include "terminal.h"

Status::Status() {
    logging = false;
    connection = DISCONNECTED;
    emulation = D200;
    visLines = Terminal::DEFAULT_LINES;
    visCols = Terminal::DEFAULT_COLS;
    holding = false;
    dirty = true;
}
