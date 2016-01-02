#include "math/hmath.h"
#include "math/cmath.h"
#include "math/units.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <QJsonDocument>

using namespace std;

#define CHECK_SER(x,y) check_ser(__FILE__, __LINE__, #x, x, y)
#define CHECK_DESER_HNUMBER(x,y) check_deser_hnumber(__FILE__, __LINE__, #x, x, y)
#define CHECK_DESER_CNUMBER(x,y) check_deser_cnumber(__FILE__, __LINE__, #x, x, y)

static int ser_total_tests = 0;
static int ser_failed_tests = 0;
static int ser_new_failed_tests = 0;

template <class T>
void check_ser(const char*, int line, const char* msg, const T& num, const char* expected, int issue = 0) {
    /* Serialization */
    QJsonObject obj;
    num.serialize(obj);
    QJsonDocument doc(obj);
    QString result = doc.toJson(QJsonDocument::Compact);
    /* Test result and display info */
    ++ser_total_tests;
    if (result != expected) {
        ++ser_failed_tests;
        cerr << "[Line " << line << "]" << msg << "\nResult: " << result.toStdString();
        cerr << "\nExpected: " << expected;
        if (issue)
            cerr << "\t[ISSUE " << issue << "]";
        else {
            cerr << "\t[NEW]";
            ++ser_new_failed_tests;
        }
        cerr << endl;
    }
}

void check_deser_cnumber(const char*, int line, const char* msg, const char* str,  const char* expected, int issue = 0) {
    /* Deserialization */
    QJsonDocument doc = QJsonDocument::fromJson(str);
    CNumber num(doc.object());
    /* Test result and display info */
    char* result = CMath::format(num, 'f');
    if (strcmp(result, expected)) {
        ++ser_failed_tests;
        cerr << "[Line " << line << "]\t" << msg << "\tResult: " << result;
        cerr << "\tExpected: " << expected;
	if (issue)
            cerr << "\t[ISSUE " << issue << "]";
        else {
            cerr << "\t[NEW]";
            ++ser_new_failed_tests;
        }
        cerr << endl;
    }
    /* Cleanup */
    free(result);
}

void check_deser_hnumber(const char*, int line, const char* msg, const char* str,  const char* expected, int issue = 0) {
    /* Deserialization */
    QJsonDocument doc = QJsonDocument::fromJson(str);
    HNumber num(doc.object());
    /* Test result and display info */
    char* result = HMath::format(num, 'f');
    if (strcmp(result, expected)) {
        ++ser_failed_tests;
	cerr << "[Line " << line << "]\t" << msg << "\tResult: " << result;
        cerr << "\tExpected: " << expected;
	if (issue)
            cerr << "\t[ISSUE " << issue << "]";
        else {
            cerr << "\t[NEW]";
            ++ser_new_failed_tests;
        }
        cerr << endl;
    }
    /* Cleanup */
    free(result);
}

void check_ser_deser_hnumber(const HNumber& src) {
    /* Serialization + deserialization */
    QJsonObject obj;
    src.serialize(obj);
    HNumber dest = HNumber::deSerialize(obj);
    /* Checking result */
    char* result = HMath::format(dest, 'g', 50);
    printf("%s\n", result);
    free(result);
}

void check_ser_deser_cnumber(const CNumber& src) {
    /* Serialization + deserialization */
    QJsonObject obj;
    src.serialize(obj);
    CNumber dest = CNumber::deSerialize(obj);
    /* Checking result */
    char* result = CMath::format(dest, 'g', 50);
    printf("%s\n", result);
    free(result);
}

int main(int, char**)
{
    /* Serialization tests */
    CHECK_SER(HNumber("3"), "{\"format\":\"NULL\",\"value\":\"3.000000000000000000000000000000000000000000000000000000000000000000000000000000\"}");

    CNumber a("3");
    a.setFormat('b');
    a *= Units::meter();
    a.setDisplayUnit(CNumber("0.3")*Units::meter(), "foot");
    CHECK_SER(a,"{\"dimension\":{\"length\":\"1\"},\"format\":\"NULL\",\"unit\":\"3.000000000000000000000000000000000000000000000000000000000000000000000000000000e-1\",\"unit_name\":\"foot\",\"value\":\"3.000000000000000000000000000000000000000000000000000000000000000000000000000000\"}");

    CHECK_SER(CNumber("3"), "{\"format\":\"NULL\",\"value\":\"3.000000000000000000000000000000000000000000000000000000000000000000000000000000\"}");
    CHECK_SER(CNumber("3+1j"), "{\"format\":\"NULL\",\"value\":\"3.000000000000000000000000000000000000000000000000000000000000000000000000000000+1.000000000000000000000000000000000000000000000000000000000000000000000000000000j\"}");
    /* HNumber deserialization tests */
    CHECK_DESER_HNUMBER("{\"format\": \"g\",\"value\": \"1\"}", "1");
    CHECK_DESER_HNUMBER("{\"format\": \"g\",""\"value\": \"0.1\"}", "0.1");
    /* CNumber deserialization tests */
    CHECK_DESER_CNUMBER("{\"format\": \"g\",\"value\": \"1\"}", "1");
    CHECK_DESER_CNUMBER("{\"format\": \"g\",\"value\": \"0.1\"}", "0.1");
    CHECK_DESER_CNUMBER("{\"format\": \"g\",\"value\": \"0.0+1.0j\"}", "1j");
    /* Serizalitation + deserialization tests */
    printf("%s\n", "SER-DESER");
    check_ser_deser_hnumber(HNumber("3"));
    check_ser_deser_cnumber(CNumber("3"));
    check_ser_deser_cnumber(CNumber("3+1j"));
}

