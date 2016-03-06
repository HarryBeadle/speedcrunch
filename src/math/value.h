// value.h
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

#ifndef VALUE_H
#define VALUE_H

#include "math/quantity.h"

class QString;
class QJsonObject;


class Value
{
public:
    Value();
    Value(const Quantity & q);
    enum Type {Invalid, TQuantity};

    Quantity ToQuantity() const;
    QString ToString() const;
    void setFormat(const QString & f);

    void serialize(QJsonObject & json) const;
    void deSerialize(const QJsonObject & json);

private:
    QString m_format;
    Type m_type;
    union {
        Quantity m_quantity;
    };
};

#endif // VALUE_H
