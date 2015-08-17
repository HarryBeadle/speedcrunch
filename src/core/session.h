#ifndef SESSION_H
#define SESSION_H

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
    bool containsVariable(const QString & id) const;
    void removeVariable(const QString & id);
    Variable getVariable(const QString & id) const;
    QList<Variable> variablesToList() const;
    bool isBuiltInVariable(const QString &id) const;



    void addHistoryEntry(const HistoryEntry & entry);
    void insertHistoryEntry(const int index, const HistoryEntry & entry);
    void removeHistoryEntryAt(const int index);
    HistoryEntry historyEntryAt(const int index) const;
    QList<HistoryEntry> historyToList() const {return history;}

    void addUserFunction(const UserFunction func);
    void removeUserFunction(const QString str);
    void clearUserFunctions();
    bool hasUserFunction(const QString & str) const;
    QList<UserFunction> UserFunctionsToList() const;

    static void test();
};

#endif // SESSION_H
