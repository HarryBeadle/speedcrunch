#include "value.h"

Value::Value()
    : m_format(""), m_type(Invalid), m_quantity()
{
}

Value::Value(const Quantity &q)
    : m_format(""),m_type(TQuantity), m_quantity(q)
{
}

Quantity Value::ToQuantity() const
{
    return m_quantity;
}

QString Value::ToString() const
{
    switch(m_type)
    {
    case TQuantity:
            return ""; //TODO
    case Invalid:
    default:
        return "";
    }
}

void Value::setFormat(const QString &f)
{
    m_format = f;
}

void Value::serialize(QJsonObject &json) const
{
    json["type"] = m_type;
    QJsonObject data;
    switch(m_type)
    {
    case TQuantity:
        m_quantity.serialize(data);
        break;
    case Invalid:
    default:
        break;
    }
    json["data"] = data;
}

void Value::deSerialize(const QJsonObject &json)
{
    m_type = static_cast<Type>(json["type"].toInt());
    QJsonObject data = json["data"].toObject();
    switch(m_type)
    {
    case TQuantity:
        m_quantity.deSerialize(data);
        break;
    case Invalid:
    default:
        break;
    }
}

