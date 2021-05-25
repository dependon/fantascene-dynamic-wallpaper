#ifndef DATA_H
#define DATA_H

enum ScreenMode {
    IdCopyScreen,
    IdlayoutScreen,
    IdManualSet
};

struct MoreSetData {
    int isAuto = 0;
    int fps = 10;
};

#endif // DATA_H
