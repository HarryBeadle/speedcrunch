#ifndef SESSIONVARIABLES_H
#define SESSIONVARIABLES_H

#include <QString>
#include <QSet>
#include <QList>

#include "hmath.h"

class Variable
{
private:
    QString m_identifier;
    HNumber m_value;

public:
    Variable() : m_identifier(""), m_value(0) {}
    Variable(const QJsonObject & json);
    Variable(const QString & id, const HNumber & val) : m_identifier(id), m_value(val) {}
    Variable(const Variable & other) :  m_identifier(other.m_identifier), m_value(other.m_value) {}

    HNumber value() const {return m_value;}
    QString identifier() const {return m_identifier;}

    void setValue(const HNumber & val) {m_value = val;}
    void set_identifier(const QString & str) {m_identifier = str;}

    void serialize(QJsonObject & json) const;
    void deSerialize(const QJsonObject & json);
};

#if 0
class SessionVariables
{
private:
    QSet<Variable> m_entries;
public:
    SessionVariables() {}
    void insert(const Variable var);
    void remove(const Variable var);
    void remove(const QString identifier);
    int size() const;
    void clear();
    QList<Variable> toList();
};
#endif

#endif // SESSIONVARIABLES_H
