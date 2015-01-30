#include "cell.h"

Cell::Cell() {
    clearToSpace();
}

void Cell::set( unsigned char cv, bool bl, bool dm, bool rev, bool under, bool prot ) {
    charValue = cv;
    blink = bl;
    dim = dm;
    reverse = rev;
    underscore = under;
    protect = prot;
}

void Cell::clearToSpace() {
    charValue = ' ';
    blink = false;
    dim = false;
    reverse = false;
    underscore = false;
    protect = false;
}

void Cell::clearToSpaceIfUnprotected() {
    if (!protect) {
        clearToSpace();
    }
}

void Cell::copy( Cell fromCell ) {
    charValue = fromCell.charValue;
    blink = fromCell.blink;
    dim = fromCell.dim;
    reverse = fromCell.reverse;
    underscore = fromCell.underscore;
    protect = fromCell.protect;
}
