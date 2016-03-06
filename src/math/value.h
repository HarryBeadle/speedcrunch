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
