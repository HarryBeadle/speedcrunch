#include "sessionvariables.h"


Variable::Variable(const QJsonObject &json)
{
    deSerialize(json);
}

void Variable::serialize(QJsonObject &json) const
{
    json["identifier"] = m_identifier;
    QJsonObject value;
    m_value.serialize(value);
    json["value"] = value;
    return;
}

void Variable::deSerialize(const QJsonObject &json)
{
    m_identifier = json["identifier"].toString();
    m_value = HNumber(json["value"].toObject());
    return;
}

