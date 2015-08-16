#ifndef SESSIONHISTORY_H
#define SESSIONHISTORY_H

#include <QJsonArray>
#include <QString>
#include <QList>

#include "hmath.h"


class HistoryEntry
{
private:
    QString m_expr;
    HNumber m_result;
public:
    HistoryEntry() : m_expr(""), m_result(0) {}
    HistoryEntry(const QJsonObject & json);
    HistoryEntry(const QString & expr, const HNumber & num) : m_expr(expr), m_result(num) {}
    HistoryEntry(const HistoryEntry & other) :  m_expr(other.m_expr), m_result(other.m_result) {}

    void setExpr(const QString & e);
    void setResult(const HNumber & n);

    QString expr() const;
    HNumber result() const;

    void serialize(QJsonObject & json) const;
    void deSerialize(const QJsonObject & json);
};



#if 0
class SessionHistory
{
private:
    QList<HistoryEntry> m_entries;
public:
    SessionHistory();
    void append(const HistoryEntry entry);
    void insert(const int index, const HistoryEntry entry);
    void removeAt(const int index);
    HistoryEntry at(const int index) const;
    int size() const;
    void clear();
};
#endif

#endif // SESSIONHISTORY_H
