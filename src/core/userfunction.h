#ifndef USERFUNCTION_H
#define USERFUNCTION_H

#include<QString>
#include<QStringList>
#include<evaluator.h>


class UserFunction
{
private:
    QString m_name;
    QStringList m_arguments;
    QString m_expression;
    QString m_description;

public:
    QVector<HNumber> constants;
    QStringList identifiers;
    QVector<Evaluator::Opcode> opcodes;
    bool inUse;

    UserFunction(QString name, QStringList arguments, QString expression)
        : m_name(name), m_arguments(arguments), m_expression(expression) {}
    UserFunction() {}
    UserFunction(const QJsonObject & json);

    QString name() const;
    QStringList arguments() const;
    QString expression() const;
    QString description() const;

    void setName(const QString & str);
    void setArguments(const QStringList & args);
    void setExpression(const QString & expr);
    void setDescription(const QString & expr);

    void serialize(QJsonObject & json) const;
    void deSerialize(const QJsonObject & json);
    void compile();

};

#endif // USERFUNCTION_H
