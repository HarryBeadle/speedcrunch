// This file is part of the SpeedCrunch project
// Copyright (C) 2004, 2005, 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2007-2009, 2013, 2014 Helder Correia <helder.pereira.correia@gmail.com>
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

#ifndef CORE_SESSION_H
#define CORE_SESSION_H

#include "hmath.h"
#include "sessionhistory.h"
#include "variable.h"
#include "userfunction.h"
#include <QList>
#include <QHash>
#include <QString>
#include <QJsonArray>



class Session {
private:
    typedef QList<HistoryEntry> History ;
    typedef QHash<QString, Variable> VariableContainer;
    typedef QHash<QString, UserFunction> FunctionContainer;
    History history;
    VariableContainer variables;
    FunctionContainer userFunctions;

public:
    Session() {}
    Session(QJsonObject & json);

    void load();
    void save();

    void serialize(QJsonObject &json) const;
    void deSerialize(const QJsonObject & json, bool merge);


    void addVariable(const Variable & var);
    bool hasVariable(const QString & id) const;
    void removeVariable(const QString & id);
    void clearVariables();
    Variable getVariable(const QString & id) const;
    QList<Variable> variablesToList() const;
    bool isBuiltInVariable(const QString &id) const;

    void addHistoryEntry(const HistoryEntry & entry);
    void insertHistoryEntry(const int index, const HistoryEntry & entry);
    void removeHistoryEntryAt(const int index);
    HistoryEntry historyEntryAt(const int index) const;
    QList<HistoryEntry> historyToList() const {return history;}

    void addUserFunction(const UserFunction & func);
    void removeUserFunction(const QString & str);
    void clearUserFunctions();
    bool hasUserFunction(const QString & str) const;
    QList<UserFunction> UserFunctionsToList() const;
    const UserFunction * getUserFunction(const QString & fname) const;

    static void test();
};

#endif // CORE_SESSION_H
