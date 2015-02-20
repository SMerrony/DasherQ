#ifndef CELL_H
#define CELL_H

class Cell {
public:

    unsigned char charValue;
    bool blink, dim, reverse, underscore, protect;

    Cell();

    void set( unsigned char cv, bool bl, bool dm, bool rev, bool under, bool prot );
    void clearToSpace();
    void clearToSpaceIfUnprotected();
    void copy( Cell fromCell );
};

#endif // CELL_H
