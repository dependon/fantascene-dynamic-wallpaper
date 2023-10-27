/*
 * Copyright (C) 2020 ~ 2022 LiuMingHang.
 *
 * Author:     LiuMingHang <liuminghang0821@gmail.com>
 *
 * Maintainer: LiuMingHang <liuminghang0821@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <setdesktop.h>
#include <X11/Xlib.h>

X11MatchingPid::X11MatchingPid(unsigned long pid)
    : _pid(pid)
{
    _display = XOpenDisplay(0);
    Window wRoot = XDefaultRootWindow(_display);
    // Get the PID property atom.
    _atomPID = XInternAtom(_display, "_NET_WM_PID", True);
    if (_atomPID == None) {
        //            std::cout << "No such atom" << std::endl;
        return;
    }

    search(wRoot);
}

const std::list<Window> &X11MatchingPid::result() const { return _result; }

const std::list<Window> &X11MatchingPid::allresult() const { return _allresult; }

void X11MatchingPid::search(unsigned long w)
{
    // Get the PID for the current Window.
    Atom           type;
    int            format;
    unsigned long  nItems;
    unsigned long  bytesAfter;
    unsigned char *propPID = 0;
    if (Success == XGetWindowProperty(_display, w, _atomPID, 0, 1, False, XA_CARDINAL,
                                      &type, &format, &nItems, &bytesAfter, &propPID)) {
        if (propPID != 0) {
            _allresult.push_back(w);
            // If the PID matches, add this window to the result set.
            if (_pid == *((unsigned long *)propPID))
                _result.push_back(w);
            if ((unsigned long *)propPID != 0) {
                unsigned long xx = *(unsigned long *)propPID;
                //                    std::cout << xx;
            }

            XFree(propPID);
        }
    }

    // Recurse into child windows.
    Window    wRoot;
    Window    wParent;
    Window   *wChild;
    unsigned  nChildren;
    if (0 != XQueryTree(_display, w, &wRoot, &wParent, &wChild, &nChildren)) {
        for (unsigned i = 0; i < nChildren; i++)
            search(wChild[i]);
    }
}
