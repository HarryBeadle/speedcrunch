// This file is part of the SpeedCrunch project
// Copyright (C) 2013 Helder Correia <helder.pereira.correia@gmail.com>
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

#include "core/numberformatter.h"

#include "core/settings.h"
#include "math/hmath.h"
#include "math/rational.h"

#include <QMap>

QString NumberFormatter::format(HNumber number)
{
    Settings* settings = Settings::instance();


    //handle units
    QString unit_name = "";
    HNumber unit(1);
    if(number.hasUnit()) {
        unit_name = ' ' + number.getUnitName();
        unit = number.getUnit();
        number.stripUnits();
        number /= unit;
    } else if(!number.isDimensionless()) {
        number.cleanDimension();
        // autogenerate unit name
        QMap<QString, Rational> dimension = number.getDimension();
        QMap<QString, Rational>::const_iterator i = dimension.constBegin();
        while (i != dimension.constEnd()) {
            QString exponent = i.value().toString();
            if(exponent.contains('/'))
                exponent = "^(" + exponent+')';
            else if(exponent == "1")
                exponent = "";
            else
                exponent = '^' + exponent;


            // TODO: replace this with a lookup to a repository
            if(i.key() == "length") {
                    unit_name += " meter";

            } else if(i.key() == "time") {
                unit_name += " second";

            } else if(i.key() == "mass") {
                unit_name += " kilogram";

            } else if(i.key() == "el. current") {
                unit_name += " ampere";

            } else if(i.key() == "amount") {
                unit_name += " mole";

            } else if(i.key() == "luminous intensity") {
                unit_name += " canedela";

            } else if(i.key() == "temperature") {
                unit_name += " kelvin";
            } else
                unit_name += " " + i.key(); // fall back to the dimension name
            unit_name += exponent;
            ++i;
        }
    }

    const char format = number.format() != 0 ? number.format() : settings->resultFormat;
    char* str = HMath::format(number, format, settings->resultPrecision);
    QString result = QString::fromLatin1(str);
    free(str);
    if(unit_name != "") {
        result.append(unit_name);
    }
    if (settings->radixCharacter() != '.')
        result.replace('.', settings->radixCharacter());
    return result;
}
