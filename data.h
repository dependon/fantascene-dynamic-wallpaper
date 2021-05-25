#ifndef DATA_H
#define DATA_H

enum ScreenMode {
    IdCopyScreen,
    IdlayoutScreen,
    IdManualSet
};

struct MoreSetData {
    bool bisAuto = false;
    int fps = 10;
};

#endif // DATA_H
