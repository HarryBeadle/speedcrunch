// This file is part of the SpeedCrunch project
// Copyright (C) 2004, 2005, 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2007-2009, 2013, 2014 Helder Correia <helder.pereira.correia@gmail.com>
// Copyright (C) 2015 Pol Welter <polwelter@gmail.com>
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

