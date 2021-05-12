//---------------------------------------------------------------------------
#include <math.h>
#include <QFile>
#include <QList>
#include <QTextStream>
#include <QDebug>
//---------------------------------------------------------------------------
#include "qlas.h"
//---------------------------------------------------------------------------
#define left_al 39
#define al1 6
//#define al2 10
//---------------------------------------------------------------------------
bool operator <  (const TLas_Data& left, const TLas_Data& right)
{
    return (left.dept < right.dept);
}
//---------------------------------------------------------------------------
bool operator >  (const TLas_Data& left, const TLas_Data& right)
{
    return (left.dept > right.dept);
}
//---------------------------------------------------------------------------
bool operator == (const TLas_Data& left, const TLas_Data& right)
{
    return (left.dept == right.dept);
}
//---------------------------------------------------------------------------
bool operator != (const TLas_Data& left, const TLas_Data& right)
{
    return (left.dept != right.dept);
}
//---------------------------------------------------------------------------
QLas::QLas(QObject *parent):
    QObject(parent),
    crv_count(0),
    ver_strings  (new(QStringList)),
    well_strings (new(QStringList)),
    curv_strings (new(QStringList)),
    param_strings(new(QStringList)),
    other_strings(new(QStringList)),
    data_strings (new(QStringList)),
    dept_start( 0),
    dept_stop ( 0),
    dept_step (10),
    null_val(QString::fromUtf8("-999.00")),
    COMP(QString::fromUtf8("")),
    WELL(QString::fromUtf8("")),
    FLD (QString::fromUtf8("")),
    LOC (QString::fromUtf8("")),
    CNTY(QString::fromUtf8("")),
    STAT(QString::fromUtf8("")),
    CTRY(QString::fromUtf8("")),
    SRVC(QString::fromUtf8("")),
    DATE(QString::fromUtf8("")),
    METD(QString::fromUtf8("")),
    wrap(false)

{
    qRegisterMetaType<TVer>("TVer");
    qRegisterMetaType<TLas_Data>("TLas_Data");

    ver.hi =1;
    ver.lo = 20;

}
//---------------------------------------------------------------------------
QLas::~QLas()
{
    delete ver_strings;
    delete well_strings;
    delete curv_strings;
    delete param_strings;
    delete other_strings;
    delete data_strings;
}
//---------------------------------------------------------------------------
int QLas::create_head(void)
{
    sec_ver();
    sec_well();
    if (crv_count == 0) sec_curv();
    sec_param();
    sec_other();
    return 0;
}
//---------------------------------------------------------------------------
int QLas::add_data(QList<PLas_Data> *dat_list, QString mnem, QString m_unit, QString descripton, int precison)
{
    QString   s;
    QString   s1;
    int       i;
    int       k1;
    int       k2;
    PLas_Data pLas1;
    int       l1;
    int       al2;

    if (crv_count == 0)
    {
        sec_curv();
        add_dept_col();
    }

    //------------------------------------------------------------------
    // секция ~Curve information
    //------------------------------------------------------------------
    k1 = al1;
    k2 = left_al - al1 - 1 - m_unit.length();
    s = QString("%1%2%3%4%5%6")
            .arg(mnem, k1, QChar(' '))
            .arg(QChar('.'))
            .arg(m_unit)
            .arg(QChar(' '), k2, QChar(' '))
            .arg(QString::fromUtf8(": "))
            .arg(descripton);
    curv_strings->append(s);

    al2 = 6;

    if ((dat_list != NULL) && (dat_list->count() != 0))
    {
        time2dept(dat_list);

        for (i = 0; i < dat_list->count(); i++)
        {
            pLas1 = dat_list->at(i);

            s1 = QString("%1").arg((int)pLas1->data);

            {
                int new_len = s1.length();
                if (precison > 0)
                {
                    new_len += precison;
                    new_len++;
                }

                if (new_len > al2) al2 = new_len;
            }
        }
    }
    else
    {
        int new_len = null_val.length();
        if (precison > 0)
        {
            new_len += precison;
            new_len--;
        }

        if (new_len > al2) al2 = new_len;
    }

    al2 += 2;

    //------------------------------------------------------------------
    // секция ~A
    //------------------------------------------------------------------
    k1 = al2;

    s = QString("%1%2")
            .arg(data_strings->at(0))
            .arg(mnem, k1, QChar(' '));

    data_strings->removeAt(0);
    data_strings->insert(0, s);

    l1 = dept_start;

    for (i = 1; i < data_strings->count(); i++)
    {
        s1 = null_val;
        if ((dat_list != NULL) && (dat_list->count() != 0))
        {
            for (k1 = 0; k1 < dat_list->count(); k1++)
            {
                pLas1 = dat_list->at(k1);
                if (pLas1->dept == l1)
                {
                    s1 = QString("%1").arg(pLas1->data, 0, 'f', precison, QChar(' '));

                    delete pLas1;
                    dat_list->removeAt(k1);
                    break;
                }
            }
        }

        k1 = al2;

        s = QString("%1%2")
                .arg(data_strings->at(i))
                .arg(s1, k1, QChar(' '));


        data_strings->removeAt(i);
        data_strings->insert(i, s);

        l1 += dept_step;
    }

    crv_count++;

    return 0;
}
//---------------------------------------------------------------------------
int QLas::add_dept_col(void)
{
    QString     s;
    int         k;
    int         f1;
    int         f2;

    //------------------------------------------------------------------
    //  DEPT.M                               :Глубина
    //------------------------------------------------------------------
    k = left_al - 8;//s.length();

    s = QString("%1%2%3")
            .arg(QString::fromUtf8("  DEPT.M"))
            .arg(QChar(' '), k, QChar(' '))
            .arg(QString::fromUtf8(": Глубина"));
    curv_strings->append(s);

    //------------------------------------------------------------------
    //~A DEPTH.M
    //------------------------------------------------------------------
    s = QString::fromUtf8("~A DEPT.M ");
    data_strings->append(s);

    dept_step = abs(dept_step);
    if (dept_start > dept_stop) dept_step = -dept_step;

    dept_start = dept_step * (dept_start / dept_step);

    dept_stop = dept_step * (dept_stop / dept_step);

    f1 = dept_start;
    f2 = dept_stop + dept_step;

    while(f1 != f2)
    {
        s = QString("%1").arg((f1 / 100.0), 10, 'f', 2, QChar(' '));
        data_strings->append(s);
        f1 += dept_step;
    }

    crv_count++;

    return 0;
}
//---------------------------------------------------------------------------
int QLas::add_ml(QList<TLas_Data> *dat_list)
{
    QString     s;
    QString     s1;
    int         i;
    int         k1;
    int         k2;
    TLas_Data   las1;
    int         l1;
    int         al2 = 5;

    if (crv_count == 0)
    {
        sec_curv();
        add_dept_col();
    }

    //------------------------------------------------------------------
    /*
    ~Curve information
    # MNEM.UNIT                   API CODE   CURVE DESCRIPTION
    # ====.================================:====================
        ML.                                : Магнитна метка
  */
    //------------------------------------------------------------------
    k1 = al1;
    k2 = left_al - al1 - 1;
    s = QString("%1%2%3%4")
            .arg(QString::fromUtf8("ML"), k1, QChar(' '))
            .arg(QChar('.'))
            .arg(QChar(' '), k2, QChar(' '))
            .arg(QString::fromUtf8(": Магнитная метка"));
    curv_strings->append(s);

    //------------------------------------------------------------------
    //~A DEPTH.M
    //------------------------------------------------------------------
    s = data_strings->at(0);
    s1 = QString::fromUtf8("ML");
    k1 = al2;// - s1.length();
    s.append(QString("%1").arg(s1, k1, QChar(' ')));
    data_strings->removeAt(0);
    data_strings->insert(0, s);

    i = 0;
    while (i < dat_list->count())
    {
        las1 = dat_list->at(i);
        if (las1.data != 1.0)
        {
            dat_list->removeAt(i);
        }
        else
        {
            i++;
            las1.dept = dept_step * (las1.dept / dept_step);
        }
    }

    l1 = dept_start;

    for(i = 1; i < data_strings->count(); i++)
    {
        s1 = "0";
        for(k1 = 0; k1 < dat_list->count(); k1++)
        {
            las1 = dat_list->at(k1);
            if (las1.dept == l1)
            {
                s1 = "1";
                dat_list->removeAt(k1);
                break;
            }
        }

        k1 = al2;// - s1.Length();

        s = QString("%1%2")
                .arg(data_strings->at(i))
                .arg(s1, k1, QChar(' '));

        data_strings->removeAt(i);
        data_strings->insert(i, s);

        l1 += dept_step;
    }

    crv_count++;

    return 0;
}
//---------------------------------------------------------------------------
int QLas::write_las(QString f_name)
{
    ver_strings->append(*well_strings);
    ver_strings->append(*curv_strings);
    ver_strings->append(*param_strings);
    ver_strings->append(*other_strings);
    ver_strings->append(*data_strings);

    QFile f_las(f_name);
    if (!f_las.open(QIODevice::WriteOnly))
    {
        qDebug() << f_las.errorString();
        //throw GIS_file_error;
        return 1;
    }

    QTextStream las(&f_las);
    las.setCodec("CP866");

    int i;
    for(i = 0; i < ver_strings->size(); i++)
    {
        las << ver_strings->at(i) << QChar('\r') << QChar('\n');
    }

    return 0;
}
//---------------------------------------------------------------------------
int QLas::sec_ver(void)
{
    QString     s;
    QString     s1;
    QString     s2;
    QString     s3;
    int         k1;
    int         k2;

    //------------------------------------------------------------------
    //~Version information
    //------------------------------------------------------------------
    s = QString::fromUtf8("~Version information");
    ver_strings->append(s);

    //------------------------------------------------------------------
    //  VERS.                            1.20: CWLS LAS  -  VERSION 1.20
    //------------------------------------------------------------------
    s1 = QString::fromUtf8("VERS");
    s2 = QString("%1%2%3")
            .arg(ver.hi)
            .arg(QChar('.'))
            .arg(ver.lo);
    k1 = al1;
    k2 = left_al - al1 - 1;
    s = QString("%1%2%3%4%5")
            .arg(s1, k1, QChar(' '))
            .arg(QChar('.'))
            .arg(s2, k2, QChar(' '))
            .arg(QString::fromUtf8(": CWLS LAS  -  VERSION "))
            .arg(s2);

    ver_strings->append(s);

    //------------------------------------------------------------------
    //  WRAP.                              NO: One line per depth step
    //------------------------------------------------------------------
    s1 = QString::fromUtf8("WRAP");
    if (wrap)
    {
        // !!! Не реализован !!!
        s2 = QString::fromUtf8("YES");
        s3 = QString::fromUtf8(": Multiple lines per depth step");
    }
    else
    {
        s2 = QString::fromUtf8("NO");
        s3 = QString::fromUtf8(": One line per depth step");
    }

    k1 = al1;
    k2 = left_al - al1 - 1;
    s = QString("%1%2%3%4")
            .arg(s1, k1, QChar(' '))
            .arg(QChar('.'))
            .arg(s2, k2, QChar(' '))
            .arg(s3);
    ver_strings->append(s);

    return 0;
}
//---------------------------------------------------------------------------
int QLas::sec_well(void)
{
    QString     s;
    QString     s1;
    QString     s2;
    int         k1;
    int         k2;

    //------------------------------------------------------------------
    //~Well information
    //------------------------------------------------------------------
    s = QString::fromUtf8("~Well information");
    well_strings->append(s);

    //------------------------------------------------------------------
    //# MNEM.UNIT                   DATA TYPE  INFORMATION
    //------------------------------------------------------------------
    s = QString::fromUtf8("# MNEM.UNIT                   DATA TYPE  INFORMATION");
    well_strings->append(s);

    //------------------------------------------------------------------
    //# ====.================================: ===================
    //------------------------------------------------------------------
    s = QString::fromUtf8("# ====.================================: ===================");
    well_strings->append(s);

    //------------------------------------------------------------------
    //  STRT.M                        1350.20: Начальная глубина
    //------------------------------------------------------------------
    s1 = QString::fromUtf8("STRT");
    s2 = QString::fromUtf8(".M");
    k1 = al1;
    k2 = left_al - al1 - s2.length();

    s = QString("%1%2%3%4")
            .arg(s1, k1, QChar(' '))
            .arg(s2)
            .arg(QString("%1").arg((dept_start / 100.0), k2, 'f', 2, QChar(' ')))
            .arg(QString::fromUtf8(": Начальная глубина"));
    well_strings->append(s);

    //------------------------------------------------------------------
    //  STOP.M                        1280.50: Конечная  глубина
    //------------------------------------------------------------------
    s1 = QString::fromUtf8("STOP");
    s2 = QString::fromUtf8(".M");
    k1 = al1;
    k2 = left_al - al1 - s2.length();

    s = QString("%1%2%3%4")
            .arg(s1, k1, QChar(' '))
            .arg(s2)
            .arg(QString("%1").arg((dept_stop / 100.0), k2, 'f', 2, QChar(' ')))
            .arg(QString::fromUtf8(": Конечная глубина"));
    well_strings->append(s);

    //------------------------------------------------------------------
    //  STEP.M                          -0.10: Шаг квантования по глубине
    //------------------------------------------------------------------
    s1 = QString::fromUtf8("STEP");
    s2 = QString::fromUtf8(".M");
    k1 = al1;
    k2 = left_al - al1 - s2.length();

    s = QString("%1%2%3%4")
            .arg(s1, k1, QChar(' '))
            .arg(s2)
            .arg(QString("%1").arg((dept_step / 100.0), k2, 'f', 2, QChar(' ')))
            .arg(QString::fromUtf8(": Шаг квантования по глубине"));
    well_strings->append(s);

    //------------------------------------------------------------------
    //  NULL.                         -999.00: Отсутствие данных
    //------------------------------------------------------------------
    s1 = QString::fromUtf8("NULL");
    s2 = QString::fromUtf8(".");
    k1 = al1;
    k2 = left_al - al1 - s2.length();

    s = QString("%1%2%3%4")
            .arg(s1, k1, QChar(' '))
            .arg(s2)
            .arg(null_val, k2, QChar(' '))
            .arg(QString::fromUtf8(": Отсутствие данных"));
    well_strings->append(s);

    //------------------------------------------------------------------
    //  COMP.                         COMPANY: ТатНефтеГеофизика
    //------------------------------------------------------------------
    s1 = QString::fromUtf8("COMP");
    s2 = QString::fromUtf8(".");
    k1 = al1;
    k2 = left_al - al1 - s2.length();

    s = QString("%1%2%3%4%5")
            .arg(s1, k1, QChar(' '))
            .arg(s2)
            .arg(QString::fromUtf8("COMPANY"), k2, QChar(' '))
            .arg(QString::fromUtf8(": "))
            .arg(COMP);
    well_strings->append(s);

    //------------------------------------------------------------------
    //  WELL.                            WELL: 15624
    //------------------------------------------------------------------
    s1 = QString::fromUtf8("WELL");
    s2 = QString::fromUtf8(".");
    k1 = al1;
    k2 = left_al - al1 - s2.length();

    s = QString("%1%2%3%4%5")
            .arg(s1, k1, QChar(' '))
            .arg(s2)
            .arg(QString::fromUtf8("WELL"), k2, QChar(' '))
            .arg(QString::fromUtf8(": "))
            .arg(WELL);
    well_strings->append(s);

    //------------------------------------------------------------------
    //  FLD .                           FIELD: Almet
    //------------------------------------------------------------------
    s1 = QString::fromUtf8("FLD ");
    s2 = QString::fromUtf8(".");
    k1 = al1;
    k2 = left_al - al1 - s2.length();

    s = QString("%1%2%3%4%5")
            .arg(s1, k1, QChar(' '))
            .arg(s2)
            .arg(QString::fromUtf8("FIELD"), k2, QChar(' '))
            .arg(QString::fromUtf8(": "))
            .arg(FLD);
    well_strings->append(s);

    //------------------------------------------------------------------
    //  LOC .                        LOCATION:
    //------------------------------------------------------------------
    s1 = QString::fromUtf8("LOC ");
    s2 = QString::fromUtf8(".");
    k1 = al1;
    k2 = left_al - al1 - s2.length();

    s = QString("%1%2%3%4%5")
            .arg(s1, k1, QChar(' '))
            .arg(s2)
            .arg(QString::fromUtf8("LOCATION"), k2, QChar(' '))
            .arg(QString::fromUtf8(": "))
            .arg(LOC);
    well_strings->append(s);

    //------------------------------------------------------------------
    //  CNTY.                          COUNTY:
    //------------------------------------------------------------------
    s1 = QString::fromUtf8("CNTY");
    s2 = QString::fromUtf8(".");
    k1 = al1;
    k2 = left_al - al1 - s2.length();

    s = QString("%1%2%3%4%5")
            .arg(s1, k1, QChar(' '))
            .arg(s2)
            .arg(QString::fromUtf8("COUNTY"), k2, QChar(' '))
            .arg(QString::fromUtf8(": "))
            .arg(CNTY);
    well_strings->append(s);

    //------------------------------------------------------------------
    //  STAT.                           STATE:
    //------------------------------------------------------------------
    s1 = QString::fromUtf8("STAT");
    s2 = QString::fromUtf8(".");
    k1 = al1;
    k2 = left_al - al1 - s2.length();

    s = QString("%1%2%3%4%5")
            .arg(s1, k1, QChar(' '))
            .arg(s2)
            .arg(QString::fromUtf8("STATE"), k2, QChar(' '))
            .arg(QString::fromUtf8(": "))
            .arg(STAT);
    well_strings->append(s);

    //------------------------------------------------------------------
    //  CTRY.                         COUNTRY:
    //------------------------------------------------------------------
    s1 = QString::fromUtf8("CTRY");
    s2 = QString::fromUtf8(".");
    k1 = al1;
    k2 = left_al - al1 - s2.length();

    s = QString("%1%2%3%4%5")
            .arg(s1, k1, QChar(' '))
            .arg(s2)
            .arg(QString::fromUtf8("COUNTRY"), k2, QChar(' '))
            .arg(QString::fromUtf8(": "))
            .arg(CTRY);
    well_strings->append(s);

    //------------------------------------------------------------------
    //  SRVC.                 SERVICE COMPANY:
    //------------------------------------------------------------------
    s1 = QString::fromUtf8("SRVC");
    s2 = QString::fromUtf8(".");
    k1 = al1;
    k2 = left_al - al1 - s2.length();

    s = QString("%1%2%3%4%5")
            .arg(s1, k1, QChar(' '))
            .arg(s2)
            .arg(QString::fromUtf8("SERVICE COMPANY"), k2, QChar(' '))
            .arg(QString::fromUtf8(": "))
            .arg(SRVC);
    well_strings->append(s);

    //------------------------------------------------------------------
    //  DATE.                        LOG DATE:
    //------------------------------------------------------------------
    s1 = QString::fromUtf8("DATE");
    s2 = QString::fromUtf8(".");
    k1 = al1;
    k2 = left_al - al1 - s2.length();

    s = QString("%1%2%3%4%5")
            .arg(s1, k1, QChar(' '))
            .arg(s2)
            .arg(QString::fromUtf8("LOG DATE"), k2, QChar(' '))
            .arg(QString::fromUtf8(": "))
            .arg(DATE);
    well_strings->append(s);

    //------------------------------------------------------------------
    //  METD.                          METHOD: ВАК-8
    //------------------------------------------------------------------
    s1 = QString::fromUtf8("METD");
    s2 = QString::fromUtf8(".");
    k1 = al1;
    k2 = left_al - al1 - s2.length();

    s = QString("%1%2%3%4%5")
            .arg(s1, k1, QChar(' '))
            .arg(s2)
            .arg(QString::fromUtf8("METHOD"), k2, QChar(' '))
            .arg(QString::fromUtf8(": "))
            .arg(METD);
    well_strings->append(s);

    return 0;
}
//---------------------------------------------------------------------------
int QLas::sec_curv(void)
{
    QString s;

    //------------------------------------------------------------------
    //~Curve information
    //------------------------------------------------------------------
    s = QString::fromUtf8("~Curve information");
    curv_strings->append(s);

    //------------------------------------------------------------------
    //# MNEM.UNIT                    API CODE  CURVE DESCRIPTION
    //------------------------------------------------------------------
    s = QString::fromUtf8("# MNEM.UNIT                    API CODE  CURVE DESCRIPTION");
    curv_strings->append(s);

    //------------------------------------------------------------------
    //# ====.================================:====================
    //------------------------------------------------------------------
    s = QString::fromUtf8("# ====.================================:====================");
    curv_strings->append(s);

    return 0;
}
//---------------------------------------------------------------------------
int QLas::sec_param(void)
{
    QString s;

    //------------------------------------------------------------------
    //~Parameter information block
    //------------------------------------------------------------------
    s = QString::fromUtf8("~Parameter information block");
    param_strings->append(s);

    //------------------------------------------------------------------
    //# MNEM.UNIT                       VALUE  DESCRIPTION
    //------------------------------------------------------------------
    s = QString::fromUtf8("# MNEM.UNIT                       VALUE  DESCRIPTION");
    param_strings->append(s);

    //------------------------------------------------------------------
    //# ====.================================:====================
    //------------------------------------------------------------------
    s = QString::fromUtf8("# ====.================================:====================");
    param_strings->append(s);

    return 0;
}
//---------------------------------------------------------------------------
int QLas::sec_other(void)
{
    QString s;

    //------------------------------------------------------------------
    //~Other information
    //------------------------------------------------------------------
    s = QString::fromUtf8("~Other information");
    other_strings->append(s);

    //------------------------------------------------------------------
    //# ------------------- REMARKS AREA ------------------------
    //------------------------------------------------------------------
    s = QString::fromUtf8("# ------------------- REMARKS AREA ------------------------");
    other_strings->append(s);

    //------------------------------------------------------------------
    //# ==========================================================
    //------------------------------------------------------------------
    s = QString::fromUtf8("# ==========================================================");
    other_strings->append(s);

    return 0;
}
//---------------------------------------------------------------------------
int QLas::time2dept(QList<PLas_Data> *dat_list)
{
    PLas_Data           pLas1;
    PLas_Data           pLas2;
    PLas_Data           pLas3;
    int                 d1;
    int                 d2;
    int                 i;
    QList<PLas_Data>*   list;

    list = new QList<PLas_Data>;

    pLas1 = dat_list->first();
    d1 = dept_step * (pLas1->dept / dept_step);
    d2 = d1 + dept_step;

    for(i = 1; i < dat_list->count(); i++)
    {
        pLas2 = dat_list->at(i);

        while ( abs(d1 - pLas1->dept) > abs(d2 - pLas1->dept) )
        {
            d1 = d2;
            d2 = d1 + dept_step;
        }

        if ( abs(d1 - pLas2->dept) > abs(d1 - pLas1->dept) )
        {
            pLas3 = new TLas_Data;
            pLas3->dept = d1;
            pLas3->data = pLas1->data;
            list->append(pLas3);
            d1 = d2;
            d2 = d1 + dept_step;
        }
        pLas1 = pLas2;
    }

    while (dat_list->count() != 0)
    {
        pLas1 = (PLas_Data) dat_list->first();
        delete pLas1;
        dat_list->removeFirst();
    }

    for(i = 0; i < list->count(); i++) dat_list->append(list->at(i));

    while (list->count() != 0) list->removeFirst(); // Утечка памяти?

    delete list;

    return 0;
}
//---------------------------------------------------------------------------
