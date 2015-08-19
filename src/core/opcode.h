#ifndef CORE_OPCODE_H
#define CORE_OPCODE_H


class Opcode
{
public:
    enum  Type { Nop, Load, Ref, Function, Add, Sub, Neg, Mul, Div, Pow,
           Fact, Modulo, IntDiv, LSh, RSh, BAnd, BOr, Conv };

    Type type;
    unsigned index;

    // TODO: this is only needed for Conv Op. Maybe refactor this to a smarter place?
    // TODO: only keep a pointer to the string
    QString text;

    Opcode() : type(Nop), index(0) {}
    Opcode(Type t) : type(t), index(0) {}
    Opcode(Type t, QString txt) : type(t), index(0), text(txt) {}
    Opcode(Type t, unsigned i): type(t), index(i) {}
};

#endif // CORE_OPCODE_H
