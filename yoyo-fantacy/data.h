#ifndef DATA_H
#define DATA_H
#include <QString>
enum ScreenMode {
    IdCopyScreen,
    IdlayoutScreen,
    IdManualSet
};

struct MoreSetData {
    int isAuto = 0;
    int fps = 10;
    QString hwdec="gpu";
};

#endif // DATA_H
