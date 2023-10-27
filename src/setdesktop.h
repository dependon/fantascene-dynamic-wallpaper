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
#ifndef SETDESKTOP_H
#define SETDESKTOP_H

#include <QObject>
#ifdef Q_OS_LINUX


#include <X11/Xatom.h>
#include <iostream>
#include <list>
#include <stdlib.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

class _XDisplay;

class X11MatchingPid
{

public:
    X11MatchingPid(unsigned long pid);

    const std::list<unsigned long> &result() const;
    const std::list<unsigned long> &allresult() const;
private:
    unsigned long  _pid;
    unsigned long  _atomPID;
    _XDisplay       *_display;
    std::list<unsigned long>   _result;
    std::list<unsigned long>   _allresult;
    void search(unsigned long w);
};
#endif
#endif // SETDESKTOP_H
