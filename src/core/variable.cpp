#include "variable.h"


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
    json["type"] = (m_type==UserDefined) ? "User" : "BuiltIn";
    return;
}

void Variable::deSerialize(const QJsonObject &json)
{
    m_identifier = json["identifier"].toString();
    QString str = json["type"].toString();
    m_type = (str=="User") ? UserDefined : BuiltIn;
    m_value = CNumber(json["value"].toObject());
    return;
}

