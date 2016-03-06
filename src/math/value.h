#ifndef VALUE_H
#define VALUE_H

#include "math/quantity.h"

class QString;


class Value
{
public:
    Value();
    enum Type {TQuantity};

    Quantity ToQuantity();
    QString ToString();

private:
    QString m_format;
    union {
        Quantity m_quantity;
      };
};

#endif // VALUE_H
