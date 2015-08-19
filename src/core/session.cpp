#include "session.h"
#include "sessionhistory.h"
#include "variable.h"

#include <QFile>
#include <QJsonDocument>
#include <functions.h>





void Session::serialize(QJsonObject &json) const
{
    // history
    QJsonArray hist_entries;
    for(int i=0; i<history.size(); ++i) {
        QJsonObject curr_entry_obj;
        history.at(i).serialize(curr_entry_obj);
        hist_entries.append(curr_entry_obj);
    }
    json["history"] = hist_entries;

    //variables
    QJsonArray var_entries;
    QHashIterator<QString, Variable> i(variables);
    while(i.hasNext()) {
        i.next();
        QJsonObject curr_entry_obj;
        i.value().serialize(curr_entry_obj);
        var_entries.append(curr_entry_obj);
    }
    json["variables"] = var_entries;


    // functions
    QJsonArray func_entries;
    QHashIterator<QString, UserFunction> j(userFunctions);
    while(j.hasNext()) {
        j.next();
        QJsonObject curr_entry_obj;
        j.value().serialize(curr_entry_obj);
        func_entries.append(curr_entry_obj);
    }
    json["functions"] = func_entries;
}

void Session::deSerialize(const QJsonObject &json, bool merge=false)
{
    if(!merge) {
        history.clear();
        variables.clear();
    }

    QJsonArray hist_obj = json["history"].toArray();
    int n = hist_obj.size();
    for(int i=0; i<n; ++i) {
        history.append(HistoryEntry(hist_obj[i].toObject()));
    }

    QJsonArray var_obj = json["variables"].toArray();
    n = var_obj.size();
    for(int i=0; i<n; ++i) {
        QJsonObject var = var_obj[i].toObject();
        variables[var["identifier"].toString()].deSerialize(var);
    }

    QJsonArray func_obj = json["functions"].toArray();
    n = func_obj.size();
    for(int i=0; i<n; ++i) {
        QJsonObject func = func_obj[i].toObject();
        userFunctions[func["name"].toString()].deSerialize(func);
    }


}

void Session::addVariable(const Variable &var)
{
    QString id = var.identifier();
    variables[id] = var;
}

bool Session::hasVariable(const QString &id) const
{
    return variables.contains(id);
}

void Session::removeVariable(const QString &id)
{
    variables.remove(id);
}

void Session::clearVariables()
{
    variables.clear();
}

Variable Session::getVariable(const QString &id) const
{
    return variables.value(id);
}

QList<Variable> Session::variablesToList() const
{
    return variables.values();
}

bool Session::isBuiltInVariable(const QString & id) const
{
    // Defining variables with the same name as existing functions is not supported for now.
    if(FunctionRepo::instance()->find(id))
        return true;
    if(!variables.contains(id))
        return false;

    return variables.value(id).type() == Variable::BuiltIn;
}

void Session::addHistoryEntry(const HistoryEntry &entry)
{
    history.append(entry);
}

void Session::insertHistoryEntry(const int index, const HistoryEntry &entry)
{
    history.insert(index, entry);
}

void Session::removeHistoryEntryAt(const int index)
{
    history.removeAt(index);
}

HistoryEntry Session::historyEntryAt(const int index) const
{
    return history.at(index);
}

void Session::addUserFunction(const UserFunction &func)
{
    QString name = func.name();
    userFunctions[name] = func;
}

void Session::removeUserFunction(const QString &str)
{
    userFunctions.remove(str);
}

void Session::clearUserFunctions()
{
    userFunctions.clear();
}

bool Session::hasUserFunction(const QString &str) const
{
    return userFunctions.contains(str);
}

const UserFunction * Session::getUserFunction(const QString &fname) const
{
    return &*userFunctions.find(fname);
}







void Session::test()
{
    Session session;

    session.addVariable(Variable("pi", HNumber("3.1141592")));
    session.addVariable(Variable("e", HNumber("2.71828182845904523536")));
    HNumber x("7.2354");
    x.setDisplayUnit("0.01", "centimeter");
    session.addVariable(Variable("x", x));
    session.addVariable(Variable("ans", HNumber(37)));

    session.addHistoryEntry(HistoryEntry("sqrt(2)^2", HNumber(2)));
    session.addHistoryEntry(HistoryEntry("6^(1/3)", HNumber("1.81712059283213965889")));
    session.addHistoryEntry(HistoryEntry("61+64", HNumber(125)));

    session.addUserFunction(UserFunction("f", QStringList("x"), "5*x"));


    QJsonObject json;
    session.serialize(json);

    QFile file("test.json");
    file.open(QIODevice::WriteOnly);
    QJsonDocument doc(json);
    file.write(doc.toJson());
    file.close();



    Session session2;
    //file = QFile("test.json");
    file.open(QIODevice::ReadOnly);
    QByteArray data = file.readAll();
    QJsonDocument ldoc(QJsonDocument::fromJson(data));
    session2.deSerialize(ldoc.object());
    file.close();

    json = QJsonObject();
    QFile file2("test2.json");
    session2.serialize(json);
    file2.open(QIODevice::WriteOnly);
    QJsonDocument doc2(json);
    file2.write(doc2.toJson());
    file2.close();


}
