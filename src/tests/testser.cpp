#include "math/hmath.h"
#include "math/cmath.h"

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
void check_ser(const char* file, int line, const char* msg, const T& num, const char* expected, int issue = 0) {
    /* Serialization */
    QJsonObject obj;
    num.serialize(obj);
    QJsonDocument doc(obj);
    QString result = doc.toJson(QJsonDocument::Compact);
    /* Test result and display info */
    ++ser_total_tests;
    if (result != expected) {
        ++ser_failed_tests;
        cerr << "[Line " << line << "]\t" << msg << "\tResult: " << result.toStdString();
        cerr << "\tExpected: " << expected;
        if (issue)
            cerr << "\t[ISSUE " << issue << "]";
        else {
            cerr << "\t[NEW]";
            ++ser_new_failed_tests;
        }
        cerr << endl;
    }
}

void check_deser_cnumber(const char* file, int line, const char* msg, const char* str,  const char* expected, int issue = 0) {
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

void check_deser_hnumber(const char* file, int line, const char* msg, const char* str,  const char* expected, int issue = 0) {
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
    CHECK_SER(HNumber("3"), "{\"format\":0,\"value\":\"3.000000000000000000000000000000000000000000000000000000000000000000000000000000\"}");
    CHECK_SER(CNumber("3"), "{\"format\":0,\"value\":\"3.000000000000000000000000000000000000000000000000000000000000000000000000000000\"}");
    CHECK_SER(CNumber("3+1j"), "{\"format\":0,\"value\":\"3.000000000000000000000000000000000000000000000000000000000000000000000000000000+1.000000000000000000000000000000000000000000000000000000000000000000000000000000j\"}");
    /* HNumber deserialization tests */
    CHECK_DESER_HNUMBER("{\"format\": 0,\"value\": \"1\"}", "1");
    CHECK_DESER_HNUMBER("{\"format\": 0,""\"value\": \"0.1\"}", "0.1");
    /* CNumber deserialization tests */
    CHECK_DESER_CNUMBER("{\"format\": 0,\"value\": \"1\"}", "1");
    CHECK_DESER_CNUMBER("{\"format\": 0,\"value\": \"0.1\"}", "0.1");
    CHECK_DESER_CNUMBER("{\"format\": 0,\"value\": \"0.0+1.0j\"}", "0+1j");
    /* Serizalitation + deserialization tests */
    printf("%s\n", "SER-DESER");
    check_ser_deser_hnumber(HNumber("3"));
    check_ser_deser_cnumber(CNumber("3"));
    check_ser_deser_cnumber(CNumber("3+1j"));
}

