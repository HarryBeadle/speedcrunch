// value.cpp
// Groundwork for multi-type support
//
// This file is part of the SpeedCrunch project
// Copyright (C) 2016 Pol Welter.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

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
    switch(m_type) {
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
    switch(m_type) {
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
    switch(m_type) {
        case TQuantity:
            m_quantity.deSerialize(data);
            break;
        case Invalid:
        default:
            break;
    }
}

