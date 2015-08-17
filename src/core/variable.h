#ifndef VARIABLE_H
#define VARIABLE_H

#include <QString>
#include <QSet>
#include <QList>

#include "hmath.h"

class Variable
{
public:
    enum Type { BuiltIn, UserDefined };
private:
    QString m_identifier;
    HNumber m_value;
    Type m_type;
public:
    Variable() : m_identifier(""), m_value(0), m_type(UserDefined) {}
    Variable(const QJsonObject & json);
    Variable(const QString & id, const HNumber & val, Type t = UserDefined) : m_identifier(id), m_value(val), m_type(t) {}
    Variable(const Variable & other) :  m_identifier(other.m_identifier), m_value(other.m_value), m_type(other.m_type) {}

    HNumber value() const {return m_value;}
    QString identifier() const {return m_identifier;}
    Type type() const {return m_type;}

    void setValue(const HNumber & val) {m_value = val;}
    void set_identifier(const QString & str) {m_identifier = str;}
    void set_type(const Type t) {m_type = t;}

    void serialize(QJsonObject & json) const;
    void deSerialize(const QJsonObject & json);
    bool operator==(const Variable& other) const { return m_identifier == other.m_identifier; }
};

#endif // VARIABLE_H
