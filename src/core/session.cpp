#include "session.h"
#include "sessionhistory.h"
#include "sessionvariables.h"

#include <QFile>
#include<QJsonDocument>





void Session::serialize(QJsonObject &json) const
{
    // history
    QJsonObject hist_obj;
    hist_obj["count"] = history.size();  //currently reduntant, only the length of the QJsonArray is considered
    QJsonArray hist_entries;
    for(int i=0; i<history.size(); ++i) {
        QJsonObject curr_entry_obj;
        history.at(i).serialize(curr_entry_obj);
        hist_entries.append(curr_entry_obj);
    }
    hist_obj["entries"] = hist_entries;
    json["history"] = hist_obj;

    //variables
    QJsonObject var_obj;
    var_obj["count"] = variables.size();  // reduntant, see above
    QJsonArray var_entries;
    QMapIterator<QString, Variable> i(variables);
    while(i.hasNext()) {
        i.next();
        QJsonObject curr_entry_obj;
        i.value().serialize(curr_entry_obj);
        var_entries.append(curr_entry_obj);
    }
    var_obj["entries"] = var_entries;
    json["variables"] = var_obj;
}

void Session::deSerialize(const QJsonObject &json, bool merge=false)
{
    if(!merge) {
        history.clear();
        variables.clear();
    }

    QJsonObject hist_obj = json["history"].toObject();
    int n = hist_obj["count"].toInt();
    QJsonArray entries = hist_obj["entries"].toArray();
    for(int i=0; i<n; ++i) {
        history.append(HistoryEntry(entries[i].toObject()));
    }

    QJsonObject var_obj = json["variables"].toObject();
    n = var_obj["count"].toInt();
    entries = var_obj["entries"].toArray();
    for(int i=0; i<n; ++i) {
        QJsonObject var = entries[i].toObject();
        variables[var["identifier"].toString()].deSerialize(var);
    }

}

void Session::addVariable(const Variable &var)
{
    QString id = var.identifier();
    variables[id] = var;
}

bool Session::containsVariable(const QString &id) const
{
    return variables.contains(id);
}

void Session::removeVariable(const QString &id)
{
    variables.remove(id);
}

Variable Session::getVariable(const QString &id) const
{
    return variables.value(id);
}

QList<Variable> Session::variablesToList() const
{
    return variables.values();
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
