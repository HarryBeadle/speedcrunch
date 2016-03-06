// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2005, 2006 Johan Thelin <e8johan@gmail.com>
// Copyright (C) 2007, 2008, 2009, 2010, 2013 Helder Correia <helder.pereira.correia@gmail.com>
// Copyright (C) 2009 Wolf Lammen <ookami1@gmx.de>
// Copyright (C) 2014 Tey <teyut@free.fr>
// Copyright (C) 2015 Pol Welter <polwelter@gmail.com>
// Copyright (C) 2015 Hadrien Theveneau <theveneau@gmail.com>
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

#include "core/evaluator.h"
#include "core/session.h"
#include "core/settings.h"
#include "math/rational.h"
#include "math/units.h"

#include <QCoreApplication>
#include <QStack>

//#define EVALUATOR_DEBUG
#define MAX_STACK_DEPTH 100
#define ALLOW_IMPLICIT_MULT


#ifdef EVALUATOR_DEBUG
#include <QFile>
#include <QTextStream>


QTextStream& operator<<(QTextStream& s, CNumber num)
{
    char* str = CMath::format(num, 'f');
    s << str;
    free(str);
    return s;
}
#endif // EVALUATOR_DEBUG

static Evaluator* s_evaluatorInstance = 0;

static void s_deleteEvaluator()
{
    delete s_evaluatorInstance;
}

const CNumber& Evaluator::checkOperatorResult(const CNumber& n)
{
    switch (n.error()) {
    case Success: break;
    case NoOperand:
        if(m_assignFunc == false)
            m_error = Evaluator::tr("cannot operate on a NaN");
        break;
    case Underflow:
        m_error = Evaluator::tr("underflow - tiny result is out of SpeedCrunch's number range");
        break;
    case Overflow:
        m_error = Evaluator::tr("overflow - huge result is out of SpeedCrunch's number range");
        break;
    case ZeroDivide:
        m_error = Evaluator::tr("division by zero");
        break;
    case OutOfLogicRange:
        m_error = Evaluator::tr("overflow - logic result exceeds maximum of 256 bits");
        break;
    case OutOfIntegerRange:
        m_error = Evaluator::tr("overflow - integer result exceeds maximum limit for integers");
        break;
    case TooExpensive:
        m_error = Evaluator::tr("too time consuming - computation was rejected");
        break;
    case DimensionMismatch:
        m_error = Evaluator::tr("dimension mismatch - quantities with different dimensions cannot be compared, added, etc.");
        break;
    case InvalidDimension:
        m_error = Evaluator::tr("invalid dimension - operation might require dimensionless arguments");
        break;
    case EvalUnstable:
        m_error = Evaluator::tr("Computation aborted - encountered numerical instability");
        break;
    default:;
    }
    return n;
}

QString Evaluator::stringFromFunctionError(Function* function)
{
    if (!function->error())
        return QString();

    QString result = QString::fromLatin1("<b>%1</b>: ");

    switch (function->error()) {
    case Success: break;
    case InvalidParamCount:
        result += Evaluator::tr("wrong number of arguments");
        break;
    case NoOperand:
        result += Evaluator::tr("does not take NaN as an argument");
        break;
    case Overflow:
        result += Evaluator::tr("overflow - huge result is out of SpeedCrunch's number range");
        break;
    case Underflow:
        result += Evaluator::tr("underflow - tiny result is out of SpeedCrunch's number range");
        break;
    case OutOfLogicRange:
        result += Evaluator::tr("overflow - logic result exceeds maximum of 256 bits");
        break;
    case OutOfIntegerRange:
        result += Evaluator::tr("result out of range");
        break;
    case ZeroDivide:
        result += Evaluator::tr("division by zero");
        break;
    case EvalUnstable:
        result += Evaluator::tr("Computation aborted - encountered numerical instability");
        break;
    case OutOfDomain:
        result += Evaluator::tr("undefined for argument domain");
        break;
    case TooExpensive:
        result += Evaluator::tr("computation too expensive");
        break;
    case InvalidDimension:
        result += Evaluator::tr("invalid dimension - function might require dimensionless arguments");
        break;
    case DimensionMismatch:
        result += Evaluator::tr("dimension mismatch - quantities with different dimensions cannot be compared, added, etc.");
        break;
    case IONoBase:
    case BadLiteral:
    case InvalidPrecision:
    case InvalidParam:
        result += Evaluator::tr("internal error, please report a bug");
        break;
    default:
        result += Evaluator::tr("error");
        break;
    };

    return result.arg(function->identifier());
}

class TokenStack : public QVector<Token>
{
public:
    TokenStack();

    bool isEmpty() const;
    unsigned itemCount() const;
    Token pop();
    void push(const Token& token);
    const Token& top();
    const Token& top(unsigned index);

private:
    void ensureSpace();
    int topIndex;
};

const Token Token::null;

// Helper function: return operator of given token text e.g. "*" yields Operator::Asterisk.
static Token::Op matchOperator(const QString& text)
{
    Token::Op result = Token::InvalidOp;

    if (text.length() == 1) {
        QChar p = text.at(0);
        switch(p.unicode()) {
        case '+': result = Token::Plus; break;
        case '-': result = Token::Minus; break;
        case '*': result = Token::Asterisk; break;
        case '/': result = Token::Slash; break;
        case '^': result = Token::Caret; break;
        case ';': result = Token::Semicolon; break;
        case '(': result = Token::LeftPar; break;
        case ')': result = Token::RightPar; break;
        case '!': result = Token::Exclamation; break;
        case '=': result = Token::Equal; break;
        case '\\': result = Token::Backslash; break;
        case '&': result = Token::Ampersand; break;
        case '|': result = Token::Pipe; break;
        default: result = Token::InvalidOp;
        }
    } else if (text.length() == 2) {
        if (text == "**")
            result = Token::Caret;
        else if(text == "<<")
          result = Token::LeftShift;
        else if(text == ">>")
          result = Token::RightShift;
        else if(text == "->")
            result = Token::RightArrow;
        else if(text == "in")
            result = Token::RightArrow;
    }
#if 0
    else if (text.length() == 3) {
        if (text == "mod")
            result = Token::Modulo;
    }
#endif
    else if(text.startsWith("->"))
        result = Token::RightArrow;

   return result;
}

// Helper function: give operator precedence e.g. "+" is 300 while "*" is 500.
static int opPrecedence(Token::Op op)
{
    int prec;
    switch(op) {
    case Token::Exclamation: prec = 800; break;
    case Token::Caret: prec = 700; break;
    case Token::Asterisk:
    case Token::Slash: prec = 500; break;
    case Token::Modulo:
    case Token::Backslash: prec = 600; break;
    case Token::Plus:
    case Token::Minus: prec = 300; break;
    case Token::LeftShift:
    case Token::RightShift: prec = 200; break;
    case Token::Ampersand: prec = 100; break;
    case Token::Pipe: prec = 50; break;
    case Token::RightArrow: prec = 0;
    case Token::RightPar:
    case Token::Semicolon: prec = -100; break;
    case Token::LeftPar: prec = -200; break;
    default: prec = -200; break;
    }
    return prec;
}

Token::Token(Type type, const QString& text, int pos)
{
    m_type = type;
    m_text = text;
    m_pos = pos;
}

Token::Token(const Token& token)
{
    m_type = token.m_type;
    m_text = token.m_text;
    m_pos = token.m_pos;
}

Token& Token::operator=(const Token& token)
{
    m_type = token.m_type;
    m_text = token.m_text;
    m_pos = token.m_pos;
    return*this;
}

CNumber Token::asNumber() const
{
    QString text = m_text;
    return isNumber() ? CNumber((const char*)text.toLatin1()) : CNumber(0);
}

Token::Op Token::asOperator() const
{
    return isOperator() ? matchOperator(m_text) : InvalidOp;
}

QString Token::description() const
{
    QString desc;

    switch (m_type) {
    case stxNumber: desc = "Number"; break;
    case stxIdentifier: desc = "Identifier"; break;
    case stxOpenPar:
    case stxClosePar:
    case stxSep:
    case stxOperator: desc = "Operator"; break;
    default: desc = "Unknown"; break;
    }

    while (desc.length() < 10)
        desc.prepend(' ');
    desc.prepend("  ");
    desc.prepend(QString::number(m_pos));
    desc.append(" : ").append(m_text);

    return desc;
}

TokenStack::TokenStack() : QVector<Token>()
{
    topIndex = 0;
    ensureSpace();
}

bool TokenStack::isEmpty() const
{
    return topIndex == 0;
}

unsigned TokenStack::itemCount() const
{
    return topIndex;
}

void TokenStack::push(const Token& token)
{
    ensureSpace();
    (*this)[topIndex++] = token;
}

Token TokenStack::pop()
{
    return topIndex > 0 ? Token(at(--topIndex)) : Token();
}

const Token& TokenStack::top()
{
    return top(0);
}

const Token& TokenStack::top(unsigned index)
{
    return (topIndex > (int)index) ? at(topIndex - index - 1) : Token::null;
}

void TokenStack::ensureSpace()
{
    int length = size();
    while (topIndex >= length) {
        length += 10;
        resize(length);
    }
}

// Helper function: return true for valid identifier character.
static bool isIdentifier(QChar ch)
{
    return ch.unicode() == '_' || ch.unicode() == '$' || ch.isLetter();
}

// Helper function: return true for valid radix characters.
bool Evaluator::isRadixChar(const QChar &ch)
{
    if (Settings::instance()->parseAllRadixChar)
        return ch.unicode() == '.' || ch.unicode() == ',';
    // There exist more than 2 radix characters actually:
    //   U+0027 ' apostrophe
    //   U+002C , comma
    //   U+002E . full stop
    //   U+00B7 · middle dot
    //   U+066B ٫ arabic decimal separator
    //   U+2396 ⎖ decimal separator key symbol

    return ch.unicode() == Settings::instance()->radixCharacter();
}

/* Only match known thousand separators: 
 *   U+0020   space
 *   U+0027 ' apostrophe
 *   U+002C , comma
 *   U+002E . full stop
 *   U+005F _ low line
 *   U+00B7 · middle dot
 *   U+066B ٫ arabic decimal separator
 *   U+066C ٬ arabic thousands separator
 *   U+02D9 ˙ dot above
 *   U+2396 ⎖ decimal separator key symbol
 */
static QRegExp s_separatorStrictRE("[ ',\\._\\x00B7\\x066B\\x066C\\x02D9\\x2396]");

// Match everything that is not alphanumeric or an operator or NUL.
static QRegExp s_separatorRE("[^a-zA-Z0-9\\+\\-\\*/\\^;\\(\\)%!=\\\\&\\|<>\\?#\\x0000]");

// Helper function: return true for valid thousand separator characters.
bool Evaluator::isSeparatorChar(const QChar &ch)
{
    if (isRadixChar(ch))
        return false;

    if (Settings::instance()->strictDigitGrouping)
        return s_separatorStrictRE.exactMatch(ch);
    else
        return s_separatorRE.exactMatch(ch);
}

Evaluator* Evaluator::instance()
{
    if (!s_evaluatorInstance) {
        s_evaluatorInstance = new Evaluator;
        qAddPostRoutine(s_deleteEvaluator);
    }
    return s_evaluatorInstance;
}

Evaluator::Evaluator()
{
    reset();
}

void Evaluator::initializeBuiltInVariables()
{
    setVariable(QLatin1String("e"), CMath::e(), Variable::BuiltIn);
    setVariable(QString::fromUtf8("ℯ"), CMath::e(), Variable::BuiltIn);

    setVariable(QLatin1String("pi"), CMath::pi(), Variable::BuiltIn);
    setVariable(QString::fromUtf8("π"), CMath::pi(), Variable::BuiltIn);

    if(Settings::instance()->complexNumbers) {
        setVariable(QLatin1String("j"), CMath::i(), Variable::BuiltIn);
    }
    else if(hasVariable("j")) {
        unsetVariable("j", true);
    }

#if 0
    setVariable(QString::fromUtf8("meter"), Units::meter(), Variable::BuiltIn);
    setVariable(QString::fromUtf8("second"), Units::second(), Variable::BuiltIn);
    setVariable(QString::fromUtf8("kilogram"), Units::kilogram(), Variable::BuiltIn);
    setVariable(QString::fromUtf8("ampere"), Units::ampere(), Variable::BuiltIn);
    setVariable(QString::fromUtf8("mole"), Units::mole(), Variable::BuiltIn);
    setVariable(QString::fromUtf8("candela"), Units::candela(), Variable::BuiltIn);
    setVariable(QString::fromUtf8("kelvin"), Units::kelvin(), Variable::BuiltIn);

    setVariable(QString::fromUtf8("newton"), Units::newton(), Variable::BuiltIn);
    setVariable(QString::fromUtf8("joule"), Units::joule(), Variable::BuiltIn);
    setVariable(QString::fromUtf8("watt"), Units::watt(), Variable::BuiltIn);
    setVariable(QString::fromUtf8("pascal"), Units::pascal(), Variable::BuiltIn);
    setVariable(QString::fromUtf8("coulomb"), Units::coulomb(), Variable::BuiltIn);
    setVariable(QString::fromUtf8("volt"), Units::volt(), Variable::BuiltIn);
    setVariable(QString::fromUtf8("ohm"), Units::ohm(), Variable::BuiltIn);
    setVariable(QString::fromUtf8("watt"), Units::watt(), Variable::BuiltIn);
    setVariable(QString::fromUtf8("farad"), Units::farad(), Variable::BuiltIn);
    setVariable(QString::fromUtf8("watt"), Units::watt(), Variable::BuiltIn);
    setVariable(QString::fromUtf8("tesla"), Units::tesla(), Variable::BuiltIn);
    setVariable(QString::fromUtf8("weber"), Units::weber(), Variable::BuiltIn);
    setVariable(QString::fromUtf8("henry"), Units::henry(), Variable::BuiltIn);
    setVariable(QString::fromUtf8("siemens"), Units::siemens(), Variable::BuiltIn);
#endif
}

void Evaluator::setExpression(const QString& expr)
{
    m_expression = expr;
    m_dirty = true;
    m_valid = false;
    m_error = QString();
}

QString Evaluator::expression() const
{
    return m_expression;
}

// Returns the validity. Note: empty expression is always invalid.
bool Evaluator::isValid()
{
    if (m_dirty) {
        Tokens tokens = scan(m_expression);
        if (!tokens.valid())
            compile(tokens);
        else
            m_valid = false;
    }
    return m_valid;
}

void Evaluator::reset()
{
    m_expression = QString();
    m_dirty = true;
    m_valid = false;
    m_error = QString();
    m_constants.clear();
    m_codes.clear();
    m_assignId = QString();
    m_assignFunc = false;
    m_assignArg.clear();
    m_session = NULL;
    m_stack_depth=0;

    initializeBuiltInVariables();
}

void Evaluator::setSession(Session *s)
{
    m_session = s;
}

const Session *Evaluator::session()
{
    return m_session;
}

QString Evaluator::error() const
{
    return m_error;
}

// Returns list of token for the expression.
// This triggers again the lexical analysis step. It is however preferable
// (even when there's small performance penalty) because otherwise we need to
// store parsed tokens all the time which serves no good purpose.
Tokens Evaluator::tokens() const
{
    return scan(m_expression);
}

Tokens Evaluator::scan(const QString& expr) const
{
    // Result.
    Tokens tokens;

    // Parsing state.
    enum { Start, Finish, Bad, InNumber, InHexa, InOctal, InBinary, InDecimal, InExpIndicator,
           InExponent, InIdentifier } state;

    // Initialize variables.
    state = Start;
    int i = 0;
    QString ex = expr;
    QString tokenText;
    int tokenStart = 0;
    Token::Type type;
    bool numberFrac = false;

    // Force a terminator.
    ex.append(QChar());

    // Main loop.
    while (state != Bad && state != Finish && i < ex.length()) {
        QChar ch = ex.at(i);

        switch (state) {
        case Start:
            tokenStart = i;
            // State variables reset
            numberFrac = false;

            // Skip any whitespaces.
            if (ch.isSpace())
                ++i;
            else if (ch == '?') // Comment.
                state = Finish;
            else if (ch.isDigit()) // Check for number.
                state = InNumber;
            else if (ch == '#') { // Simple hexadecimal notation.
                tokenText.append("0x");
                state = InHexa;
                ++i;
            } else if (isRadixChar(ch)) { // Radix character?
                tokenText.append('.');  // Issue 151.
                state = InDecimal;
                ++i;
            } else if (ch.isNull()) // Terminator character.
                state = Finish;
            else if (isIdentifier(ch)) // Identifier or alphanumeric operator
                state = InIdentifier;
            else { // Look for operator match.
                int op;
                QString s;
#if 0
                // Check for three-char operator.
                s.append(ch).append(ex.at(i+1)).append(ex.at(i+2));
                op = matchOperator(s);

                // Check for two-char operator.
                if (op == Token::InvalidOp)
#endif
                {
                    s = QString(ch).append(ex.at(i+1));
                    op = matchOperator(s);
                }

                // Check for one-char operator.
                if (op == Token::InvalidOp) {
                    s = QString(ch);
                    op = matchOperator(s);
                }

                // Any matched operator?
                if (op != Token::InvalidOp) {
                    switch(op) {
                        case Token::LeftPar: type = Token::stxOpenPar; break;
                        case Token::RightPar: type = Token::stxClosePar; break;
                        case Token::Semicolon: type = Token::stxSep; break;
                        default: type = Token::stxOperator;
                    }
                    int len = s.length();
                    i += len;
                    tokens.append(Token(type, s.left(len), tokenStart));
                }
                else
                    state = Bad;
            }

            //// Beginning with unknown alphanumeric?  Could be identifier or function.
            //if (state == Bad && isIdentifier(ch))
            //    state = InIdentifier;
            break;

        /* Manage both identifier and alphanumeric operators */
        case InIdentifier:
            // Consume as long as alpha, dollar sign, underscore, or digit.
            if (isIdentifier(ch) || ch.isDigit())
                tokenText.append(ex.at(i++));
            else { // We're done with identifier.
                // if token is an operator
                if (matchOperator(tokenText)) {
                    tokens.append(Token(Token::stxOperator, tokenText, tokenStart));
                }
                // else, normal identifier
                else {
                    tokens.append(Token(Token::stxIdentifier, tokenText, tokenStart));
                }
                tokenStart = i;
                tokenText = "";
                state = Start;
            }
            break;

        case InNumber:
            if (ch.isDigit()) // Consume as long as it's a digit.
                tokenText.append(ex.at(i++));
            else if (isRadixChar(ch)) { // Convert to '.'.
                tokenText.append('.');
                ++i;
                state = InDecimal;
            }
            else if (ch.toUpper() == 'E') { // Exponent?
                tokenText.append('E');
                ++i;
                state = InExpIndicator;
            } else if (ch.toUpper() == 'X' && tokenText == "0") { // Normal hexadecimal notation.
                tokenText.append('x');
                ++i;
                state = InHexa;
            } else if (ch.toUpper() == 'B' && tokenText == "0") { // Binary notation.
                tokenText.append('b');
                ++i;
                state = InBinary;
            } else if (ch.toUpper() == 'O' && tokenText == "0") { // Octal notation.
                tokenText.append('o'); ++i; state = InOctal;
            } else if (ch.toUpper() == 'D' && tokenText == "0") { // Explicit decimal notation.
                tokenText = ""; // We also need to get rid of the leading zero.
                ++i;
            } else if (isSeparatorChar(ch)) // Ignore thousand separators
                ++i;
            else { // We're done with integer number.
                tokens.append(Token(Token::stxNumber, tokenText, tokenStart));
                tokenText = "";
                state = Start;
            }
            break;

        case InHexa:
            if (ch.isDigit() || (ch >= 'A' && ch < 'G') || (ch >= 'a' && ch < 'g'))
                tokenText.append(ex.at(i++).toUpper());
            else if (!numberFrac && (isRadixChar(ch))) {
                // Allow a unique fractionnal part.
                tokenText.append('.');
                ++i;
                numberFrac = true;
            } else if (isSeparatorChar(ch)) // Ignore thousand separators
                ++i;
            else { // We're done with hexadecimal number.
                tokens.append(Token(Token::stxNumber, tokenText, tokenStart));
                tokenText = "";
                state = Start;
            }
            break;

        case InBinary:
            if (ch == '0' || ch == '1')
                tokenText.append(ex.at(i++));
            else if (!numberFrac && (isRadixChar(ch))) {
                // Allow a unique fractionnal part.
                tokenText.append('.');
                ++i;
                numberFrac = true;
            } else if (isSeparatorChar(ch)) // Ignore thousand separators
                ++i;
            else { // We're done with binary number.
                tokens.append(Token(Token::stxNumber, tokenText, tokenStart));
                tokenText = "";
                state = Start;
            }
            break;

        case InOctal:
            if (ch >= '0' && ch < '8')
                tokenText.append(ex.at(i++));
            else if (!numberFrac && (isRadixChar(ch))) {
                // Allow a unique fractionnal part.
                tokenText.append('.');
                ++i;
                numberFrac = true;
            } else if (isSeparatorChar(ch)) // Ignore thousand separators
                ++i;
            else { // We're done with octal number.
                tokens.append(Token(Token::stxNumber, tokenText, tokenStart));
                tokenText = ""; state = Start;
            }
            break;

        case InDecimal:
            if (ch.isDigit()) // Consume as long as it's a digit.
                tokenText.append(ex.at(i++));
            else if (ch.toUpper() == 'E') { // Exponent?
                tokenText.append('E');
                ++i;
                state = InExpIndicator;
            } else if (isSeparatorChar(ch)) // Ignore thousand separators
                ++i;
            else { // We're done with floating-point number.
                tokens.append(Token(Token::stxNumber, tokenText, tokenStart));
                tokenText = "";
                state = Start;
            };
            break;

        case InExpIndicator:
            if (ch == '+' || ch == '-') // Possible + or - right after E.
                tokenText.append(ex.at(i++));
            else if (ch.isDigit()) // Consume as long as it's a digit.
                state = InExponent;
            else if (isSeparatorChar(ch)) // Ignore thousand separators
                ++i;
            else // Invalid thing here.
                state = Bad;
            break;

        case InExponent:
            if (ch.isDigit()) // Consume as long as it's a digit.
                tokenText.append(ex.at(i++));
            else if (isSeparatorChar(ch)) // Ignore thousand separators
                ++i;
            else { // We're done with floating-point number.
                tokens.append(Token(Token::stxNumber, tokenText, tokenStart));
                tokenText = "";
                state = Start;
            };
            break;

        case Bad:
            tokens.setValid(false);
            break;

        default:
            break;
        };
    }

    if (state == Bad)
        // Invalidating here too, because usually when we set state to Bad, the case Bad won't be run.
        tokens.setValid(false);


    // Find the correct text for the conversion operator (->)
    // Advance up to the next operator with lower precedence, excluding higher precedence ops in parenthesis.
    // WARNING: This assumes that there is no UNARY operator with lower precedence than the conversion operator.
    for(int i=0; i<tokens.length(); ++i) {
        //Find conversion operator
        if(!tokens.at(i).isOperator() || tokens.at(i).asOperator()!=Token::RightArrow)
            continue;
        Token & conv_token = tokens[i];

        bool must_fence = false;
        int parens = 0;
        QString expr = "";

        //Make sure the next token cannot be mistake for part of a number
        if(tokens.length()>i+1) {
            QString s = tokens.at(i+1).text().toUpper();
            if((QString("1234567890XBOED").contains(s[0]) || isRadixChar(s[0])) &&
                  (s.length()<=1 || QString("0123456789abcdef").contains(s[1])))
                must_fence=true;
        }
        while(++i<tokens.length()) {
            if(tokens.at(i).isOperator()) {
                int prec = opPrecedence(tokens.at(i).asOperator());
                if(tokens.at(i).asOperator() == Token::LeftPar)
                    ++parens;
                else if(tokens.at(i).asOperator() == Token::RightPar)
                    --parens;
                else if (prec <= opPrecedence(Token::Asterisk) && !parens)
                    must_fence = true;
                //break if all parenthesis are matched and a lower priority op has been found
                else if(parens==0 && prec <= opPrecedence(Token::RightArrow))
                    break;
            }
            if(i>1 && (tokens.at(i-1).isIdentifier() || tokens.at(i-1).isNumber()) && (tokens.at(i).isIdentifier() || tokens.at(i).isNumber())) {
                expr.append(" "); // Add a space to emphasize implicit multiplication
            }
            expr.append(tokens.at(i).text());
        }
        if(must_fence)
            expr = "(" + expr + ")";
        // We must replace "in" by "->" to manage alphanumeric unit conversion operator "in"
        // addUnit does it
        conv_token.addUnit(expr);
    }
    return tokens;
}

void Evaluator::compile(const Tokens& tokens)
{
#ifdef EVALUATOR_DEBUG
    QFile debugFile("eval.log");
    debugFile.open(QIODevice::WriteOnly);
    QTextStream dbg(&debugFile);
#endif

    // Initialize variables.
    m_dirty = false;
    m_valid = false;
    m_codes.clear();
    m_constants.clear();
    m_identifiers.clear();
    m_error = QString();

    // Sanity check.
    if (tokens.count() == 0)
        return;

    TokenStack syntaxStack;
    QStack<int> argStack;
    unsigned argCount = 1;

    for (int i = 0; i <= tokens.count(); ++i) {
        // Helper token: InvalidOp is end-of-expression.
        Token token = (i < tokens.count()) ? tokens.at(i) : Token(Token::stxOperator);
        Token::Type tokenType = token.type();
        if (tokenType >= Token::stxOperator)
            tokenType = Token::stxOperator;

#ifdef EVALUATOR_DEBUG
        dbg << "\n";
        dbg << "Token: " << token.description() << "\n";
#endif

        // Unknown token is invalid.
        if (tokenType == Token::stxUnknown)
            break;

        // Try to apply all parsing rules.
        if (1) { // TODO: Remove this
#ifdef EVALUATOR_DEBUG
            dbg << "  Checking rules..." << "\n";
#endif
            // Repeat until no more rule applies.
            bool argHandled = false;
            while (true) {
                bool ruleFound = false;

                // Rule for function last argument: id (arg) -> arg.
                if (!ruleFound && syntaxStack.itemCount() >= 4) {
                    Token par2 = syntaxStack.top();
                    Token arg = syntaxStack.top(1);
                    Token par1 = syntaxStack.top(2);
                    Token id = syntaxStack.top(3);
                    if (par2.asOperator() == Token::RightPar && !arg.isOperator()
                         && par1.asOperator() == Token::LeftPar && id.isIdentifier())
                    {
                        ruleFound = true;
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.push(Token::stxIdentifier);  // Fixes issue 538: 3 sin (3 pi) was evaluated but not 3 sin (3)
                        m_codes.append(Opcode(Opcode::Function, argCount));
#ifdef EVALUATOR_DEBUG
                        dbg << "    Rule for function last argument " << argCount << " \n";
#endif
                        argCount = argStack.empty() ? 0 : argStack.pop();
                    }
                }

                // Are we entering a function? If token is operator, and stack already has: id (arg
                if (!ruleFound && !argHandled && tokenType == Token::stxOperator
                     && syntaxStack.itemCount() >= 3)
                {
                    Token arg = syntaxStack.top();
                    Token par = syntaxStack.top(1);
                    Token id = syntaxStack.top(2);
                    if (!arg.isOperator() && par.asOperator() == Token::LeftPar
                         && id.isIdentifier())
                    {
                        ruleFound = true;
                        argStack.push(argCount);
                        argCount = 1;
#ifdef EVALUATOR_DEBUG
                        dbg << "  Entering function " << argCount << " \n";
#endif
                        break;
                    }
                }

                // Rule for postfix operators: Y POSTFIX -> Y
                // Condition: Y is not an operator, POSTFIX is a postfix op.
                // Since we evaluate from left to right, we need not check precedence at this point.
                if (!ruleFound && syntaxStack.itemCount() >= 2) {
                    Token postfix = syntaxStack.top();
                    Token y = syntaxStack.top(1);
                    if (postfix.isOperator() && !y.isOperator())
                        switch (postfix.asOperator()) {
                            case Token::Exclamation:
                                ruleFound = true;
                                syntaxStack.pop();
                                m_codes.append(Opcode(Opcode::Fact));
                                break;
                            default:;
                        }
#ifdef EVALUATOR_DEBUG
                        if(ruleFound) dbg << "    Rule for postfix operator " << postfix.text() << "\n";
#endif
                }

#ifdef ALLOW_IMPLICIT_MULT
                /* Rule #2 for implicit multiplication with parentheses: NUMBER ( NUMBER )     -> IDENTIFIER
                 *                                                       NUMBER ( IDENTIFIER ) -> IDENTIFIER
                 * Action: Treat as A * B.
                 * Note that this rule must be before parenthesis rule.
                 */
                if(!ruleFound && syntaxStack.itemCount() >= 4) {
                    Token par2 = syntaxStack.top();
                    Token b    = syntaxStack.top(1);
                    Token par1 = syntaxStack.top(2);
                    Token a = syntaxStack.top(3);

                    if((b.isNumber() || b.isIdentifier()) && a.isNumber()
                            && par1.asOperator() == Token::LeftPar
                            && par2.asOperator() == Token::RightPar)
                    {
                        ruleFound = true;
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.push(Token::stxIdentifier);
                        m_codes.append(Opcode::Mul);
#ifdef EVALUATOR_DEBUG
                        dbg << "    Rule #2 for implicit multiplication" << "\n";
#endif
                    }
                }
#endif

                // Rule for parenthesis: (Y) -> Y.
                if (!ruleFound && syntaxStack.itemCount() >= 3) {
                    Token right = syntaxStack.top();
                    Token y = syntaxStack.top(1);
                    Token left = syntaxStack.top(2);
                    if (!y.isOperator() && right.asOperator() == Token::RightPar
                         && left.asOperator() == Token::LeftPar)
                    {
                        ruleFound = true;
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.push(y);
#ifdef EVALUATOR_DEBUG
                        dbg << "    Rule for (Y) -> Y" << "\n";
#endif
                    }
                }

                // Rule for simplified syntax for function e.g. "sin pi" or "cos 1.2"
                // i.e no need for parentheses like "sin(pi)" or "cos(1.2)".
                if (!ruleFound && syntaxStack.itemCount() >= 2) {
                    Token arg = syntaxStack.top();
                    Token id = syntaxStack.top(1);
                    if (!arg.isOperator() && id.isIdentifier()
                         && FunctionRepo::instance()->find(id.text()))
                    {
                        ruleFound = true;
                        m_codes.append(Opcode(Opcode::Function, 1));
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.push(arg);
#ifdef EVALUATOR_DEBUG
                        dbg << "    Rule for simplified function syntax; function " << id.text() << "\n";
#endif
                    }
                }

                // Rule for unary operator in simplified function syntax. This handles case like "sin -90".
                if (!ruleFound && syntaxStack.itemCount() >= 3) {
                    Token x = syntaxStack.top();
                    Token op = syntaxStack.top(1);
                    Token id = syntaxStack.top(2);
                    if (!x.isOperator() && id.isIdentifier()
                         && FunctionRepo::instance()->find(id.text())
                         && (op.asOperator() == Token::Plus || op.asOperator() == Token::Minus))
                    {
                        ruleFound = true;
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.push(x);
                        if (op.asOperator() == Token::Minus)
                            m_codes.append(Opcode(Opcode::Neg));
#ifdef EVALUATOR_DEBUG
                        dbg << "    Rule for unary operator in simplified function syntax; function " << id.text() << "\n";
#endif
                    }
                }

                // Rule for function arguments, if token is ; or ): id (arg1 ; arg2 -> id (arg.
                if (!ruleFound && syntaxStack.itemCount() >= 5
                     && (token.asOperator() == Token::RightPar
                         || token.asOperator() == Token::Semicolon))
                {
                    Token arg2 = syntaxStack.top();
                    Token sep = syntaxStack.top(1);
                    Token arg1 = syntaxStack.top(2);
                    Token par = syntaxStack.top(3);
                    Token id = syntaxStack.top(4);
                    if (!arg2.isOperator() && sep.asOperator() == Token::Semicolon
                         && !arg1.isOperator()
                         && par.asOperator() == Token::LeftPar && id.isIdentifier())
                    {
                        ruleFound = true;
                        argHandled = true;
                        syntaxStack.pop();
                        syntaxStack.pop();
                        ++argCount;
#ifdef EVALUATOR_DEBUG
                        dbg << "    Rule for function argument " << argCount << " \n";
#endif
                    }
                }

                // Rule for function call with parentheses, but without argument e.g. "2*PI()".
                if (!ruleFound && syntaxStack.itemCount() >= 3) {
                    Token par2 = syntaxStack.top();
                    Token par1 = syntaxStack.top(1);
                    Token id = syntaxStack.top(2);
                    if (par2.asOperator() == Token::RightPar
                         && par1.asOperator() == Token::LeftPar && id.isIdentifier())
                    {
                        ruleFound = true;
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.push(Token(Token::stxNumber));
                        m_codes.append(Opcode(Opcode::Function, 0));
#ifdef EVALUATOR_DEBUG
                        dbg << "    Rule for function call with parentheses, but without argument" << "\n";
#endif
                    }
                }

                // Rule for binary operator:  A (op) B -> A.
                // Conditions: precedence of op >= precedence of token.
                // Action: push (op) to result e.g. "A * B" becomes "A" if token is operator "+".
                // Exception: for caret (power operator), if op is another caret
                // then the rule doesn't apply, e.g. "2^3^2" is evaluated as "2^(3^2)".
                if (!ruleFound && syntaxStack.itemCount() >= 3) {
                    Token b = syntaxStack.top();
                    Token op = syntaxStack.top(1);
                    Token a = syntaxStack.top(2);
                    if (!a.isOperator() && !b.isOperator() && op.isOperator()
                         && ((token.isOperator() && opPrecedence(op.asOperator()) >= opPrecedence(token.asOperator())
                              && token.asOperator() != Token::LeftPar
                              && token.asOperator() != Token::Caret)    // token is normal operator
                             || (!token.isOperator()                    // token may represent implicit multiplication
                                 && opPrecedence(op.asOperator()) >= opPrecedence(Token::Asterisk))))
                    {
                        ruleFound = true;
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.pop();
                        if(b.isIdentifier()) //prefer identifiers to numbers on the stack (for implicit mult)
                            syntaxStack.push(b);
                        else
                            syntaxStack.push(a);
                        switch (op.asOperator()) {
                            // Simple binary operations.
                            case Token::Plus:      m_codes.append(Opcode::Add); break;
                            case Token::Minus:     m_codes.append(Opcode::Sub); break;
                            case Token::Asterisk:  m_codes.append(Opcode::Mul); break;
                            case Token::Slash:     m_codes.append(Opcode::Div); break;
                            case Token::Caret:     m_codes.append(Opcode::Pow); break;
                            case Token::Modulo:    m_codes.append(Opcode::Modulo); break;
                            case Token::Backslash: m_codes.append(Opcode::IntDiv); break;
                            case Token::LeftShift: m_codes.append(Opcode::LSh); break;
                            case Token::RightShift: m_codes.append(Opcode::RSh); break;
                            case Token::Ampersand: m_codes.append(Opcode::BAnd); break;
                            case Token::Pipe:      m_codes.append(Opcode::BOr); break;
                            case Token::RightArrow:
                                m_codes.append(Opcode(Opcode::Conv, op.text().mid(2))); break;
                            default: break;
                        };
#ifdef EVALUATOR_DEBUG
                        dbg << "    Rule for binary operator" << "\n";
#endif
                    }
                }

#ifdef ALLOW_IMPLICIT_MULT
                /* Rule #1 for implicit multiplication:  NUMBER IDENTIFIER     -> IDENTIFIER
                 *                                       IDENTIFIER IDENTIFIER -> IDENTIFIER
                 * Action: Treat as A * B.
                 * Note that the second operand must not be a number. The result is always an identifier.
                 */
                if(!ruleFound && syntaxStack.itemCount() >= 2) {
                    Token b = syntaxStack.top();
                    Token a = syntaxStack.top(1);

                    if((a.isNumber() || a.isIdentifier()) && (b.isIdentifier())
                            && token.asOperator() != Token::LeftPar
                            && ((token.isOperator()
                                 && opPrecedence(Token::Asterisk) >= opPrecedence(token.asOperator())) // token is normal operator
                                || !token.isOperator())) // token represents implicit multiplication
                    {
                        ruleFound = true;
                        syntaxStack.pop();
                        syntaxStack.pop();
                        if(b.isIdentifier()) //prefer identifiers to numbers on the stack
                            syntaxStack.push(b);
                        else
                            syntaxStack.push(a);
                        m_codes.append(Opcode::Mul);
#ifdef EVALUATOR_DEBUG
                        dbg << "    Rule #1 for implicit multiplication" << "\n";
#endif
                    }

                }
#endif

                // Rule for unary operator:  (op1) (op2) X -> (op1) X.
                // Conditions: op2 is unary, token is not '('.
                if (!ruleFound && token.asOperator() != Token::LeftPar
                        && syntaxStack.itemCount() >= 3)
                {
                    Token x = syntaxStack.top();
                    Token op2 = syntaxStack.top(1);
                    Token op1 = syntaxStack.top(2);
                    if (!x.isOperator() && op1.isOperator()
                         && (op2.asOperator() == Token::Plus || op2.asOperator() == Token::Minus)
                         && (!token.isOperator()
                             || opPrecedence(token.asOperator()) <= opPrecedence(Token::Asterisk)))
                    {
                        ruleFound = true;
                        if (op2.asOperator() == Token::Minus)
                            m_codes.append(Opcode(Opcode::Neg));

                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.push(x);
#ifdef EVALUATOR_DEBUG
                        dbg << "    Rule for unary operator" << op2.text() << "\n";
#endif
                    }
                }

                // auxiliary rule for unary prefix operator:  (op) X -> X
                // conditions: op is unary, op is first in syntax stack
                // action: create code for (op). Unary MINUS or PLUS are treated with the precedence of multiplication.
                if (!ruleFound && token.asOperator() != Token::LeftPar
                     //&& token.asOperator() != Token::Exclamation
                     && syntaxStack.itemCount() == 2)
                {
                    Token x = syntaxStack.top();
                    Token op = syntaxStack.top(1);
                    if (!x.isOperator()
                        && (op.asOperator() == Token::Plus || op.asOperator() == Token::Minus)
                        && ((token.isOperator() && opPrecedence(token.asOperator()) <= opPrecedence(Token::Asterisk))
                            || !token.isOperator()))
                    {
                        ruleFound = true;
                        if (op.asOperator() == Token::Minus)
                            m_codes.append(Opcode(Opcode::Neg));
#ifdef EVALUATOR_DEBUG
                        dbg << "    Rule for unary operator (auxiliary)" << "\n";
#endif
                    }
                    if (ruleFound) {
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.push(x);
#ifdef EVALUATOR_DEBUG
                        dbg << "    Rule for unary operator (auxiliary)" << "\n";
#endif
                    }
                }

                if (!ruleFound)
                    break;
            }


            // Can't apply rules anymore, push the token.
            syntaxStack.push(token);

            // For identifier, generate code to load from reference.
            if (tokenType == Token::stxIdentifier) {
                m_identifiers.append(token.text());
                m_codes.append(Opcode(Opcode::Ref, m_identifiers.count() - 1));
#ifdef EVALUATOR_DEBUG
                dbg << "  Push " << token.text() << " to identifier pools" << "\n";
#endif
            }

            // For constants, generate code to load from a constant.
            if (tokenType == Token::stxNumber) {
                m_constants.append(token.asNumber());
                m_codes.append(Opcode(Opcode::Load, m_constants.count() - 1));
#ifdef EVALUATOR_DEBUG
                dbg << "  Push " << token.asNumber() << " to constant pools" << "\n";
#endif
            }
        }
    }

    // syntaxStack must left only one operand and end-of-expression (i.e. InvalidOp).
    m_valid = false;
    if (syntaxStack.itemCount() == 2 && syntaxStack.top().isOperator()
         && syntaxStack.top().asOperator() == Token::InvalidOp
         && !syntaxStack.top(1).isOperator())
    {
        m_valid = true;
    }

#ifdef EVALUATOR_DEBUG
    dbg << "Dump: " << dump() << "\n";
    debugFile.close();
#endif

    // Bad parsing ? clean-up everything.
    if (!m_valid) {
        m_constants.clear();
        m_codes.clear();
        m_identifiers.clear();
    }

}

CNumber Evaluator::evalNoAssign()
{
    CNumber result;

    if (m_dirty) {
        Tokens tokens = scan(m_expression);

        // Invalid expression?
        if (!tokens.valid()) {
            m_error = tr("invalid expression");
            return CNumber(0);
        }

        // Variable assignment?
        m_assignId = QString();
        m_assignFunc = false;
        m_assignArg.clear();
        if (tokens.count() > 2 && tokens.at(0).isIdentifier()
             && tokens.at(1).asOperator() == Token::Equal)
        {
            m_assignId = tokens.at(0).text();
            tokens.erase(tokens.begin());
            tokens.erase(tokens.begin());
        } else if (tokens.count() > 2 && tokens.at(0).isIdentifier()
             && tokens.at(1).asOperator() == Token::LeftPar)
        {
            // Check for function assignment
            // Syntax: ident opLeftPar (ident (opSemiColon ident)*)? opRightPar opEqual
            int t;
            if (tokens.count() > 4 && tokens.at(2).asOperator() == Token::RightPar) {
                // Functions with no argument.
                t = 3;
                if (tokens.at(3).asOperator() == Token::Equal)
                    m_assignFunc = true;
            } else {
                for (t = 2; t + 1 < tokens.count(); t += 2)  {
                    if (!tokens.at(t).isIdentifier())
                        break;

                    m_assignArg.append(tokens.at(t).text());

                    if (tokens.at(t + 1).asOperator() == Token::RightPar) {
                        // Check for the equal operator
                        t += 2;
                        if (t < tokens.count() && tokens.at(t).asOperator() == Token::Equal)
                            m_assignFunc = true;

                        break;
                    } else if (tokens.at(t + 1).asOperator() != Token::Semicolon)
                        break;
                }
            }

            if (m_assignFunc == true) {
                m_assignId = tokens.at(0).text();
                for (; t >= 0; --t)
                    tokens.erase(tokens.begin());
            } else
                m_assignArg.clear();
        }

        compile(tokens);
    }

    result = exec(m_codes, m_constants, m_identifiers);

    return result;
}

CNumber Evaluator::exec(const QVector<Opcode>& opcodes, const QVector<CNumber>& constants,
                        const QStringList& identifiers) {
    QStack<CNumber> stack;
    QHash<int, QString> refs;
    int index;
    CNumber val1, val2;
    QVector<CNumber> args;
    QString fname;
    Function* function;
    const UserFunction* userFunction = NULL;

    for (int pc = 0; pc < opcodes.count(); ++pc) {
        const Opcode& opcode = opcodes.at(pc);
        index = opcode.index;
        switch (opcode.type) {
            // No operation.
            case Opcode::Nop:
                break;

            // Load a constant, push to stack.
            case Opcode::Load:
                val1 = constants.at(index);
                stack.push(val1);
                break;

            // Unary operation.
            case Opcode::Neg:
                if (stack.count() < 1) {
                    m_error = tr("invalid expression");
                    return CMath::nan();
                }
                val1 = stack.pop();
                val1 = checkOperatorResult(-val1);
                stack.push(val1);
                break;

            // Binary operation: take two values from stack, do the operation, push the result to stack.
            case Opcode::Add:
                if (stack.count() < 2) {
                    m_error = tr("invalid expression");
                    return CMath::nan();
                }
                val1 = stack.pop();
                val2 = stack.pop();
                val2 = checkOperatorResult(val2 + val1);
                stack.push(val2);
                break;

            case Opcode::Sub:
                if (stack.count() < 2) {
                    m_error = tr("invalid expression");
                    return CMath::nan();
                }
                val1 = stack.pop();
                val2 = stack.pop();
                val2 = checkOperatorResult(val2 - val1);
                stack.push(val2);
                break;

            case Opcode::Mul:
                if (stack.count() < 2) {
                    m_error = tr("invalid expression");
                    return CMath::nan();
                }
                val1 = stack.pop();
                val2 = stack.pop();
                val2 = checkOperatorResult(val2 * val1);
                stack.push(val2);
                break;

            case Opcode::Div:
                if (stack.count() < 2) {
                    m_error = tr("invalid expression");
                    return CMath::nan();
                }
                val1 = stack.pop();
                val2 = stack.pop();
                val2 = checkOperatorResult(val2 / val1);
                stack.push(val2);
                break;

            case Opcode::Pow:
                if (stack.count() < 2) {
                    m_error = tr("invalid expression");
                    return CMath::nan();
                }
                val1 = stack.pop();
                val2 = stack.pop();
                if(Settings::instance()->complexNumbers)
                    val2 = checkOperatorResult(CMath::raise(val2, val1));
                else
                    val2 = checkOperatorResult(HMath::raise(val2.real, val1.real));
                stack.push(val2);
                break;

            case Opcode::Fact:
                if (stack.count() < 1) {
                    m_error = tr("invalid expression");
                    return CMath::nan();
                }
                val1 = stack.pop();
                val1 = checkOperatorResult(CMath::factorial(val1));
                stack.push(val1);
                break;

            case Opcode::Modulo:
                if (stack.count() < 2) {
                    m_error = tr("invalid expression");
                    return CMath::nan();
                }
                val1 = stack.pop();
                val2 = stack.pop();
                val2 = checkOperatorResult(val2 % val1);
                stack.push(val2);
                break;

            case Opcode::IntDiv:
                if (stack.count() < 2) {
                    m_error = tr("invalid expression");
                    return CMath::nan();
                }
                val1 = stack.pop();
                val2 = stack.pop();
                val2 = checkOperatorResult(val2 / val1);
                stack.push(CMath::integer(val2));
                break;

            case Opcode::LSh:
                if (stack.count() < 2) {
                    m_error = tr("invalid expression");
                    return CMath::nan();
                }
                val1 = stack.pop();
                val2 = stack.pop();
                val2 = val2 << val1;
                stack.push(val2);
                break;

            case Opcode::RSh:
                if (stack.count() < 2) {
                    m_error = tr("invalid expression");
                    return CMath::nan();
                }
                val1 = stack.pop();
                val2 = stack.pop();
                val2 = val2 >> val1;
                stack.push(val2);
                break;

            case Opcode::BAnd:
                if (stack.count() < 2) {
                    m_error = tr("invalid expression");
                    return CMath::nan();
                }
                val1 = stack.pop();
                val2 = stack.pop();
                val2 &= val1;
                stack.push(val2);
                break;

            case Opcode::BOr:
                if (stack.count() < 2) {
                    m_error = tr("invalid expression");
                    return CMath::nan();
                }
                val1 = stack.pop();
                val2 = stack.pop();
                val2 |= val1;
                stack.push(val2);
                break;

            case Opcode::Conv:
                if (stack.count() < 2) {
                    m_error = tr("invalid expression");
                    return HMath::nan();
                }
                val1 = stack.pop();
                val2 = stack.pop();
                if(val1.isZero()) {
                    m_error = tr("unit must not be zero");
                    return HMath::nan();
                }
                /*if(!val1.sameDimension(val2)) {
                    m_error = tr("Conversion failed - dimension mismatch");
                    return HMath::nan();
                }
                val2.setDisplayUnit(val1, opcode.text);*/
                stack.push(val2);
                break;

            // Reference.
            case Opcode::Ref:
                fname = identifiers.at(index);
                if (m_assignArg.contains(fname)) // Argument.
                    stack.push(CMath::nan());
                else if (hasVariable(fname)) // Variable.
                    stack.push(getVariable(fname).value());
                else { // Function.
                    function = FunctionRepo::instance()->find(fname);
                    if (function) {
                        stack.push(CMath::nan());
                        refs.insert(stack.count(), fname);
                    } else if (m_assignFunc) {
                        // Allow arbitrary identifiers when declaring user functions.
                        stack.push(CMath::nan());
                        refs.insert(stack.count(), fname);
                    } else if (hasUserFunction(fname)) {
                        stack.push(CMath::nan());
                        refs.insert(stack.count(), fname);
                    } else {
                        m_error = "<b>" + fname + "</b>: " + tr("unknown function or variable");
                        return CMath::nan();
                    }
                }
                break;

            // Calling function.
            case Opcode::Function:
                // Must do this first to avoid crash when using vars like functions.
                if (refs.isEmpty())
                    break;

                fname = refs.take(stack.count() - index);
                function = FunctionRepo::instance()->find(fname);

                userFunction = NULL;
                if (!function) {
                    // Check if this is a valid user function call.
                    userFunction = getUserFunction(fname);
                }

                if (!function && !userFunction && !m_assignFunc) {
                    m_error = "<b>" + fname + "</b>: " + tr("unknown function or variable");
                    return CMath::nan();
                }

                if (stack.count() < index) {
                    m_error = tr("invalid expression");
                    return CMath::nan();
                }

                args.clear();
                for(; index; --index)
                    args.insert(args.begin(), stack.pop());

                // Remove the NaN we put on the stack (needed to make the user
                // functions declaration work with arbitrary identifiers).
                stack.pop();

                // Show function signature if the user gave no argument (yet).
                if (userFunction) {
                    if (!args.count() && userFunction->arguments().count() != 0) {
                        m_error = QString::fromLatin1("<b>%1</b>(%2)").arg(userFunction->name())
                            .arg(userFunction->arguments().join(";"));
                        return CMath::nan();
                    }
                } else if (function) {
                    if (!args.count()) {
                        m_error = QString::fromLatin1("<b>%1</b>(%2)").arg(fname).arg(function->usage());
                        return CMath::nan();
                    }
                }

                if (m_assignFunc) {
                    // Allow arbitrary identifiers when declaring user functions.
                    stack.push(CMath::nan());
                } else if (userFunction) {
                    stack.push(execUserFunction(userFunction, args));
                    if (!m_error.isEmpty())
                        return CMath::nan();
                } else {
                    stack.push(function->exec(args));
                    if (function->error()) {
                        m_error = stringFromFunctionError(function);
                        return CMath::nan();
                    }
                }
                break;

            default:
                break;
        }
    }

    // More than one value in stack? Unsuccesfull execution...
    if (stack.count() != 1) {
        m_error = tr("invalid expression");
        return CMath::nan();
    }
    return stack.pop();
}

CNumber Evaluator::execUserFunction(const UserFunction* function, QVector<CNumber>& arguments) {
    /* TODO:
     *   OK ignore missing variables or user functions when declaring a user function.
     *   OK prohibit user function recursion by using a flag in UserFunction.
     *   OK when an error happens in a user function, tell the user which one it is.
     *   OK save user functions when the app closes, and restore them at startup.
     *   OK show a list of user functions and allow the user to delete them.
     *   - replace user variables by user functions (with no argument) ?
     */
    if (arguments.count() != function->arguments().count()) {
        m_error = "<b>" + function->name() + "</b>: " + tr("wrong number of arguments");
        return CMath::nan();
    }



    ++m_stack_depth;
    if(m_stack_depth>MAX_STACK_DEPTH) {
        m_error = "<b>" + function->name() + "</b>: " + tr("Maximum recursion depth exceeded");
        --m_stack_depth;
        return CMath::nan();
    }

    QVector<Opcode> newOpcodes;
    QVector<CNumber> newConstants = function->constants; // Copy

    // Replace references to function arguments by constants.
    for (int i = 0; i < function->opcodes.count(); ++i) {
        Opcode opcode = function->opcodes.at(i);

        if (opcode.type == Opcode::Ref) {
            // Check if the identifier is an argument name.
            QString name = function->identifiers.at(opcode.index);
            int argIdx = function->arguments().indexOf(name);
            if (argIdx >= 0) {
                // Replace the reference by a constant value.
                opcode = Opcode(Opcode::Load, newConstants.count());
                newConstants.append(arguments.at(argIdx));
            }
        }

        newOpcodes.append(opcode);
    }

    CNumber result = exec(newOpcodes, newConstants, function->identifiers);
    if (!m_error.isEmpty() && !m_error.contains(tr("Maximum recursion depth exceeded"))) {
        // Tell the user where the error happened, but avoid spamming them when the stack overflowed
        m_error = "<b>" + function->name() + "</b>: " + m_error;
    }
    --m_stack_depth;
    return result;
}

bool Evaluator::isUserFunctionAssign() const
{
    return m_assignFunc;
}

bool Evaluator::isBuiltInVariable(const QString& id) const
{
    // Defining variables with the same name as existing functions is not supported for now.
    if (FunctionRepo::instance()->find(id))
        return true;

    if (!m_session || !m_session->hasVariable(id))
        return false;

    return m_session->getVariable(id).type() == Variable::BuiltIn;
}

CNumber Evaluator::eval()
{
    CNumber result = evalNoAssign(); // This sets m_assignId.

    if (isBuiltInVariable(m_assignId)) {
        m_error = tr("%1 is a reserved name, please choose another").arg(m_assignId);
        return CMath::nan();
    }
    // Handle user variable or function assignment.
    if (!m_assignId.isEmpty()) {
        if (m_assignFunc) {
            if(hasVariable(m_assignId)) {
                m_error = tr("%1 is a variable name, please choose another or delete the variable").arg(m_assignId);
                return CMath::nan();
            }

            // Check that each arguments is unique and not a reserved identifier.
            for (int i = 0; i < m_assignArg.count() - 1; ++i) {
                const QString &argName = m_assignArg.at(i);

                if (m_assignArg.indexOf(argName, i + 1) != -1) {
                    m_error = tr("argument %1 is used more than once").arg(argName);
                    return CMath::nan();
                }

                if (isBuiltInVariable(argName)) {
                    m_error = tr("%1 is a reserved name, please choose another").arg(argName);
                    return CMath::nan();
                }
            }

            if(m_codes.isEmpty())
                return CMath::nan();
            UserFunction userFunction(m_assignId, m_assignArg, m_expression.section("=", 1, 1).trimmed());
            userFunction.constants = m_constants;
            userFunction.identifiers = m_identifiers;
            userFunction.opcodes = m_codes;

            setUserFunction(userFunction);

        } else {
            if(hasUserFunction(m_assignId)) {
                m_error = tr("%1 is a user function name, please choose another or delete the function").arg(m_assignId);
                return CMath::nan();
            }

            setVariable(m_assignId, result);
        }
    }

    return result;
}

CNumber Evaluator::evalUpdateAns()
{
    CNumber result = eval();
    if (m_error.isEmpty() && !m_assignFunc)
        setVariable(QLatin1String("ans"), result, Variable::BuiltIn);
    return result;
}

void Evaluator::setVariable(const QString& id, CNumber value, Variable::Type type)
{
    if(!m_session)
        m_session = new Session;
    m_session->addVariable(Variable(id, value, type));
}

Variable Evaluator::getVariable(const QString& id) const
{
    if (id.isEmpty() || !m_session)
        return Variable(QLatin1String(""), CNumber(0));

    return m_session->getVariable(id);
}

bool Evaluator::hasVariable(const QString& id) const
{
    if (id.isEmpty() || !m_session)
        return false;
    else
        return m_session->hasVariable(id);
}

void Evaluator::unsetVariable(const QString& id, bool force_builtin)
{
    if(!m_session || (m_session->isBuiltInVariable(id) && ! force_builtin))
        return;
    m_session->removeVariable(id);
}

QList<Variable> Evaluator::getVariables() const
{
    return m_session ? m_session->variablesToList() : QList<Variable>();
}

QList<Variable> Evaluator::getUserDefinedVariables() const
{
    QList<Variable> result = getVariables();
    auto iter = result.begin();
    while (iter != result.end()) {
        if ((*iter).type() == Variable::BuiltIn)
            iter = result.erase(iter);
        else
            ++iter;
    }
    return result;
}

QList<Variable> Evaluator::getUserDefinedVariablesPlusAns() const
{
    QList<Variable> result = getUserDefinedVariables();
    Variable ans = getVariable(QLatin1String("ans"));
    if (!ans.identifier().isEmpty())
        result.append(ans);
    return result;
}

void Evaluator::unsetAllUserDefinedVariables()
{
    if(!m_session) return;
    CNumber ansBackup = getVariable(QLatin1String("ans")).value();
    m_session->clearVariables();
    setVariable(QLatin1String("ans"), ansBackup, Variable::BuiltIn);
}

static void replaceSuperscriptPowersWithCaretEquivalent(QString& expr)
{
    expr.replace(QString::fromUtf8("⁰"), QLatin1String("^0"));
    expr.replace(QString::fromUtf8("¹"), QLatin1String("^1"));
    expr.replace(QString::fromUtf8("²"), QLatin1String("^2"));
    expr.replace(QString::fromUtf8("³"), QLatin1String("^3"));
    expr.replace(QString::fromUtf8("⁴"), QLatin1String("^4"));
    expr.replace(QString::fromUtf8("⁵"), QLatin1String("^5"));
    expr.replace(QString::fromUtf8("⁶"), QLatin1String("^6"));
    expr.replace(QString::fromUtf8("⁷"), QLatin1String("^7"));
    expr.replace(QString::fromUtf8("⁸"), QLatin1String("^8"));
    expr.replace(QString::fromUtf8("⁹"), QLatin1String("^9"));
}

QList<UserFunction> Evaluator::getUserFunctions() const
{
        return m_session ? m_session->UserFunctionsToList() : QList<UserFunction>();
}

void Evaluator::setUserFunction(const UserFunction &f)
{
    if(!m_session)
        m_session = new Session;
    m_session->addUserFunction(f);
}

void Evaluator::unsetUserFunction(const QString& fname)
{
    m_session->removeUserFunction(fname);
}

void Evaluator::unsetAllUserFunctions()
{
    m_session->clearUserFunctions();
}

bool Evaluator::hasUserFunction(const QString& fname) const
{
    return (fname.isEmpty() || !m_session) ? false : m_session->hasUserFunction(fname);
}

const UserFunction *Evaluator::getUserFunction(const QString& fname) const
{
    if(hasUserFunction(fname))
        return m_session->getUserFunction(fname);
    else
        return NULL;
}

QString Evaluator::autoFix(const QString& expr)
{
    int par = 0;
    QString result;

    // Strip off all funny characters.
    for(int c = 0; c < expr.length(); ++c)
        if (expr.at(c) >= QChar(32))
            result.append(expr.at(c));

    // No extra whitespaces at the beginning and at the end.
    result = result.trimmed();

    // Strip trailing equal sign (=).
    while(result.endsWith("="))
        result = result.left(result.length() - 1);

    replaceSuperscriptPowersWithCaretEquivalent(result);

    // Automagically close all parenthesis.
    Tokens tokens = Evaluator::scan(result);
    if (tokens.count()) {
        for(int i = 0; i < tokens.count(); ++i)
            if (tokens.at(i).asOperator() == Token::LeftPar)
                ++par;
            else if (tokens.at(i).asOperator() == Token::RightPar)
                --par;

        if (par < 0)
            par = 0;

        // If the scanner stops in the middle, do not bother to apply fix.
        const Token& lastToken = tokens.at(tokens.count() - 1);
        if (lastToken.pos() + lastToken.text().length() >= result.length())
            while (par--)
                result.append(')');
    }

    // Special treatment for simple function e.g. "cos" is regarded as "cos(ans)".
    if (!result.isEmpty()) {
        Tokens tokens = Evaluator::scan(result);

        if (tokens.count() == 1 && tokens.at(0).isIdentifier()
             && FunctionRepo::instance()->find(tokens.at(0).text()))
        {
            result.append("(ans)");
        }
    }

    return result;
}

QString Evaluator::dump()
{
    QString result;
    int c;

    if (m_dirty) {
        Tokens tokens = scan(m_expression);
        compile(tokens);
    }

    result = QString("Expression: [%1]\n").arg(m_expression);

    result.append("  Constants:\n");
    for (c = 0; c < m_constants.count(); ++c) {
        QString vtext;
        CNumber val = m_constants.at(c);
        char* ss = CMath::format(val, 'f');
        result += QString("    #%1 = %2\n").arg(c).arg(ss);
        free(ss);
    }

    result.append("\n");
    result.append("  Identifiers:\n");
    for(c = 0; c < m_identifiers.count(); ++c) {
        QString vtext;
        result += QString("    #%1 = %2\n").arg(c).arg(m_identifiers.at(c));
    }

    result.append("\n");
    result.append("  Code:\n");
    for(int i = 0; i < m_codes.count(); ++i) {
        QString ctext;
        switch (m_codes.at(i).type) {
            case Opcode::Load: ctext = QString("Load #%1").arg(m_codes.at(i).index); break;
            case Opcode::Ref: ctext = QString("Ref #%1").arg(m_codes.at(i).index); break;
            case Opcode::Function: ctext = QString("Function (%1)").arg(m_codes.at(i).index);
                                   break;
            case Opcode::Add: ctext = "Add"; break;
            case Opcode::Sub: ctext = "Sub"; break;
            case Opcode::Mul: ctext = "Mul"; break;
            case Opcode::Div: ctext = "Div"; break;
            case Opcode::Neg: ctext = "Neg"; break;
            case Opcode::Pow: ctext = "Pow"; break;
            case Opcode::Fact: ctext = "Fact"; break;
            case Opcode::LSh: ctext = "LSh"; break;
            case Opcode::RSh: ctext = "RSh"; break;
            case Opcode::BAnd: ctext = "BAnd"; break;
            case Opcode::BOr: ctext = "BOr"; break;
            default: ctext = "Unknown"; break;
        }
        result.append("   ").append(ctext).append("\n");
    }

    return result;
}
