// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2005-2006 Johan Thelin <e8johan@gmail.com>
// Copyright (C) 2007-2013 Helder Correia <helder.pereira.correia@gmail.com>
// Copyright (C) 2015 Pol Welter <polwelter@gmail.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#ifndef CORE_SETTINGS_H
#define CORE_SETTINGS_H

#include <QtCore/QPoint>
#include <QtCore/QSize>
#include <QtCore/QStringList>
#include <QtCore/QList>

QString getDataPath();

class Settings {
public:
    static Settings* instance();

    void load();
    void save();

    char radixCharacter() const; // 0: Automatic.
    void setRadixCharacter(char c = 0);
    bool isRadixCharacterAuto() const;

    bool parseAllRadixChar;
    bool strictDigitGrouping;
    bool complexNumbers;

    char angleUnit; // 'r': radian; 'd': degree.

    char resultFormat; // See HMath documentation.
    int resultPrecision; // Ditto.

    bool autoAns;
    bool autoCalc;
    bool autoCompletion;
    int digitGrouping;
    bool sessionSave;
    bool leaveLastExpression;
    bool syntaxHighlighting;
    bool systemTrayIconVisible;
    bool windowAlwaysOnTop;
    bool autoResultToClipboard;
    bool windowPositionSave;

    bool constantsDockVisible;
    bool functionsDockVisible;
    bool historyDockVisible;
    bool formulaBookDockVisible;
    bool statusBarVisible;
    bool variablesDockVisible;
    bool userFunctionsDockVisible;
    bool windowOnfullScreen;
    bool bitfieldVisible;

    QString colorScheme;
    QString displayFont;

    QString language;

    //QStringList history;
    //QStringList historyResults;
    //QStringList variables;
    //QList<QStringList> userFunctions;

    QByteArray windowState;
    QByteArray windowGeometry;

private:
    Settings();
    Q_DISABLE_COPY(Settings)
};

#endif
