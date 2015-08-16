#include "sessionhistory.h"


HistoryEntry::HistoryEntry(const QJsonObject & json)
{
    deSerialize(json);
}

QString HistoryEntry::expr() const
{
    return m_expr;
}

HNumber HistoryEntry::result() const
{
    return m_result;
}

void HistoryEntry::setExpr(const QString & e)
{
    m_expr = e;
}

void HistoryEntry::setResult(const HNumber & n)
{
    m_result = n;
}

void HistoryEntry::serialize(QJsonObject & json) const
{
    json["expression"] = m_expr;
    QJsonObject result;
    m_result.serialize(result);
    json["result"] = result;
    return;
}

void HistoryEntry::deSerialize(const QJsonObject & json)
{
    m_expr = json["expression"].toString();
    m_result = HNumber(json["result"].toObject());
    return;
}


#if 0
SessionHistory::SessionHistory()
{
    return;
}

void SessionHistory::append(const HistoryEntry entry)
{
    m_entries.push_back(entry);
}

void SessionHistory::insert(const int index, const HistoryEntry entry)
{
    m_entries.insert(index, entry);
}

void SessionHistory::removeAt(const int index)
{
    m_entries.removeAt(index);
}

HistoryEntry SessionHistory::at(const int index) const
{
    return m_entries.at(index);
}

int SessionHistory::size() const
{
    return m_entries.size();
}

void SessionHistory::clear()
{
    m_entries.clear();
}
#endif
