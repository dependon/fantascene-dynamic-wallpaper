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

Atom getAtom(Display* display, const char* atomName) {
    return XInternAtom(display, atomName, False);
}

std::vector<Window> getChildren(Display* display, Window parent) {
    std::vector<Window> children;
    Window child, root, *childList;
    unsigned int numChildren;

    XQueryTree(display, parent, &root, &parent, &childList, &numChildren);
    for (unsigned int i = 0; i < numChildren; ++i) {
        children.push_back(childList[i]);
    }
    XFree(childList);
    return children;
}

void traverseWindows(Display* display, Window parent, std::vector<WindowInfo>& desktopWindows) {
    Atom netWmWindowType = getAtom(display, "_NET_WM_WINDOW_TYPE");
    Atom netWmWindowTypeDesktop = getAtom(display, "_NET_WM_WINDOW_TYPE_DESKTOP");
    Atom wmName = getAtom(display, "_NET_WM_NAME");
    Atom utf8String = getAtom(display, "UTF8_STRING");

    std::vector<Window> children = getChildren(display, parent);
    for (Window child : children) {
        Atom type;
        int format;
        unsigned long nitems, bytesAfter;
        unsigned char *prop = nullptr;

        if (XGetWindowProperty(display, child, netWmWindowType, 0, 4, False, XA_ATOM, &type, &format, &nitems, &bytesAfter, &prop) == Success) {
            if (type == XA_ATOM && format == 32 && nitems > 0) {
                Atom* atoms = reinterpret_cast<Atom*>(prop);
                for (unsigned long j = 0; j < nitems; ++j) {
                    if (atoms[j] == netWmWindowTypeDesktop) {
                        // Got a desktop window, now get its name
                        if (XGetWindowProperty(display, child, wmName, 0, LONG_MAX, False, AnyPropertyType, &type, &format, &nitems, &bytesAfter, &prop) == Success) {
                            if (format == 8) { // Assuming UTF-8 string
                                std::string name(reinterpret_cast<char*>(prop), nitems);

                                // You can add logic here to get the PID if needed
                                pid_t pid = 0; // PID retrieval is not straightforward with X11

                                desktopWindows.push_back({child, name, pid});
                            }
                            if(prop)
                            {
                                XFree(prop); // Always free the prop after usage
                                prop = nullptr;
                            }
                        }
                        break; // No need to check further atoms for this window
                    }
                }
                if(prop)
                {
                    XFree(prop); // Always free the prop after usage
                    prop = nullptr;
                }

            }
        }

        // Recursively traverse child windows
        traverseWindows(display, child, desktopWindows);
    }
}

std::vector<WindowInfo> getAllDesktopWindows() {
    Display*  display = XOpenDisplay(0);
    std::vector<WindowInfo> desktopWindows;
    Window root = DefaultRootWindow(display);
    traverseWindows(display, root, desktopWindows);
    return desktopWindows;
}


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


