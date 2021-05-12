//---------------------------------------------------------------------------
#ifndef Q_Las_H
#define Q_Las_H
//---------------------------------------------------------------------------
#include <QObject>
#include <QDataStream>
#include <QFile>
#include <QList>
#include <QString>
#include <QStringList>
#include <QTextStream>
//---------------------------------------------------------------------------
#pragma pack(1)
struct TVer
{
    qint16    hi;
    qint16    lo;
};
#pragma pack()
//---------------------------------------------------------------------------
#pragma pack(1)
struct TLas_Data
{
    qint32    dept;
    float     data;
};
#pragma pack()
typedef TLas_Data* PLas_Data;
bool operator <  (const TLas_Data& left, const TLas_Data& right);
bool operator >  (const TLas_Data& left, const TLas_Data& right);
bool operator == (const TLas_Data& left, const TLas_Data& right);
bool operator != (const TLas_Data& left, const TLas_Data& right);
//---------------------------------------------------------------------------
class QLas  : public QObject
{
    Q_OBJECT
private:
    TVer        ver;
    int         crv_count;

    QStringList* ver_strings;
    QStringList* well_strings;
    QStringList* curv_strings;
    QStringList* param_strings;
    QStringList* other_strings;
    QStringList* data_strings;

    int sec_ver(void);
    int sec_well(void);
    int sec_curv(void);
    int sec_param(void);
    int sec_other(void);
    int time2dept(QList<PLas_Data> *dat_list);
    int add_dept_col(void);

public:
    qint32    dept_start;   //начальная глубина, см
    qint32    dept_stop;    //конечная глубина, см
    qint32    dept_step;    //шаг квантования по глубине, см
    QString   null_val;     //отсутствие данных
    QString   COMP;
    QString   WELL;         //№ скважины
    QString   FLD;          //название площади
    QString   LOC;
    QString   CNTY;
    QString   STAT;
    QString   CTRY;
    QString   SRVC;
    QString   DATE;         //дата проведения исследования
    QString   METD;         //название метода исследования

    bool      wrap;

    explicit QLas(QObject *parent = 0);
    ~QLas();

    int create_head(void);
    int add_data(QList<PLas_Data>* dat_list, QString mnem, QString m_unit, QString descripton, int precison);
    int add_ml(QList<TLas_Data> *dat_list);
    int write_las(QString f_name);
};
//---------------------------------------------------------------------------
#endif  //Q_Las_H
