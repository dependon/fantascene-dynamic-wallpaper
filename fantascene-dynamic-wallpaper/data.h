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
    QString hwdec = "gpu";
    bool isShowDesktopIcon = true;
    bool isTop = true;
    bool isEventPenetration = false;
    double m_DesktopTransparency = 0.99;
    double m_WallpaperTransparency = 0.99;
};

#endif // DATA_H
