#ifndef SESSION_H
#define SESSION_H

#include "hmath.h"
#include "sessionhistory.h"
#include "sessionvariables.h"
#include <QList>
#include <QMap>
#include <QString>
#include <QJsonArray>



class Session {
private:
    typedef QList<HistoryEntry> History ;
    typedef QMap<QString, Variable> VariableContainer;
    History history;
    VariableContainer variables;

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


    void addHistoryEntry(const HistoryEntry & entry);
    void insertHistoryEntry(const int index, const HistoryEntry & entry);
    void removeHistoryEntryAt(const int index);
    HistoryEntry historyEntryAt(const int index) const;
    QList<HistoryEntry> historyToList() const {return history;}

    static void test();
};

#endif // SESSION_H
