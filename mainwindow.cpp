//---------------------------------------------------------------------------
#include "mainwindow.h"
#include "ui_mainwindow.h"
//---------------------------------------------------------------------------
#include <QCoreApplication>
#include <QDesktopWidget>
#include <QString>
#include <QList>
#include <QFileDialog>
#include <QFile>
#include <QSettings>
#include <QRegularExpression>
#include <math.h>
//---------------------------------------------------------------------------
#include "qlas.h"
#include "qt_vak_8u_file.h"
#include "qgravity.h"
//---------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    app_settings(new QSettings("TNG-Group", "qt_vak8u_grav"))
{
    ui->setupUi(this);

    QCoreApplication::setOrganizationName(QString::fromUtf8("TNG-Group"));
    QCoreApplication::setApplicationName(QString::fromUtf8("qt_vak8u_grav"));//Extract gravity info from VAK series tool"));
    QCoreApplication::setApplicationVersion(APP_VERSION);//QString::fromUtf8("0.0.0.1"));

    setWindowIcon(QIcon(":/images/TNG.ico"));
    setWindowTitle(QString::fromUtf8("Extract gravity info from VAK series tool"));

    load_setings();

    connect(ui->action_Exit,         SIGNAL( triggered()         ), this,            SLOT( close()        ) );
    connect(ui->action_FileOpen,     SIGNAL( triggered()         ), this,            SLOT( fileOpen()     ) );
    connect(this,                    SIGNAL( send_info(QString)  ), ui->textBrowser, SLOT(append(QString) ) );

}
//---------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    save_setings();
    delete ui;
}
//---------------------------------------------------------------------------
void MainWindow::load_setings(void)
{
    app_settings->beginGroup( QString::fromUtf8("/Settings") );

    filePath = app_settings->value( QString::fromUtf8("/WorkFolderName"), QString::fromUtf8("c:/") ).toString();

    app_settings->endGroup();
}
//---------------------------------------------------------------------------
void MainWindow::save_setings(void)
{
    if (filePath.isEmpty()) return;

    app_settings->beginGroup(QString::fromUtf8("/Settings"));

    app_settings->setValue(QString::fromUtf8("/WorkFolderName"), filePath);

    app_settings->endGroup();
}
//---------------------------------------------------------------------------
void MainWindow::fileOpen(void)
{
    QString     fName;
//    QString     filePath = QString::fromUtf8("c:/");
//    QString     filePath = QString::fromUtf8("d:/Andrey/2019/АКЦ-8С-5f/Скважина");

    fName = QFileDialog::getOpenFileName(this,
                                         QString::fromUtf8("Открыть файлы приборов серии ВАК/АКЦ"),
                                         filePath,
                                         QString::fromUtf8("Файлы приборов серии ВАК/АКЦ (*.10 *.20 *.30 *.40 *.50 *.60 *.70 *.80 *.90 *.100)"));
    if (!fName.isEmpty())
    {
        int                 pos;
        QString             tempFileName;
        QString             nextFileName;
        QFile               file;

        TLas_Data           ml;
        QList<TLas_Data>    ml_list;

        PLas_Data           data;
        QList<PLas_Data>    *data_list;

        QList<PLas_Data>    *list1;
        QList<PLas_Data>    *list2;
        QList<PLas_Data>    *list3;
        QList<PLas_Data>    *list4;
        QList<PLas_Data>    *list5;
        QList<PLas_Data>    *list6;
        QList<PLas_Data>    *list7;
        QList<PLas_Data>    *list8;

        CVAK8U              vak;

        QGravity            grav;
        QList<QGravity>     grav_list;

        tempFileName = fName;
        emit send_info(QString::fromUtf8("Выбран файл: %1").arg(fName));

        pos = tempFileName.lastIndexOf(QString::fromUtf8("/"));
        tempFileName.resize(pos);
        filePath = tempFileName;
        emit send_info(QString::fromUtf8("Рабочая папка: %1").arg(filePath));

        tempFileName = fName;
        pos = tempFileName.lastIndexOf(QRegularExpression("\\.[1-9]"));
        tempFileName.resize(pos);

        int i;
        for(i = 10; i < 200; i+=10)
        {
            nextFileName = QString::fromUtf8("%1.%2").arg(tempFileName).arg(i);
            file.setFileName(nextFileName);
            if (file.open(QIODevice::ReadOnly))
            {
                emit send_info(QString::fromUtf8("Найден файл %1").arg(nextFileName));
                file.close();

                vak.load(nextFileName);
                int j;
                for(j = 0; j < vak.count(); j++)
                {
                    qint32 dept = vak.read_dept(j);

                    {
                        ml.dept = dept;
                        if (vak.read_ml(j)) ml.data = 1.0;
                        else ml.data = 0.0;
                        ml_list.append(ml);
                    }

                    {
                        grav.set_dept(dept);

                        grav.set_angle_x (vak.read_angle_x (j));
                        grav.set_angle_y (vak.read_angle_y (j));
                        grav.set_angle_z1(vak.read_angle_z1(j));
                        grav.set_angle_z2(vak.read_angle_z2(j));

                        grav.set_accel_x (vak.read_accel_x (j));
                        grav.set_accel_y (vak.read_accel_y (j));
                        grav.set_accel_z1(vak.read_accel_z1(j));
                        grav.set_accel_z2(vak.read_accel_z2(j));

                        grav.set_temp(vak.read_temp_int(j), vak.read_temp_ppt(j));

                        grav_list.append(grav);
                    }
                }
                vak.clear();
            }
            else
            {
                break;
            }
        }

        std::sort( ml_list.begin(),   ml_list.end()   );
        std::sort( grav_list.begin(), grav_list.end() );

        nextFileName = QString::fromUtf8("%1.%2").arg(tempFileName).arg(10);
        file.setFileName(nextFileName);

        //---------------------------------------------------------------------
        // Заполняю шапку Las-файла
        //---------------------------------------------------------------------
        if (file.open(QIODevice::ReadOnly))
        {
            vak.load(nextFileName);

            QLas                las;

            las.COMP =  QString::fromUtf8("");//("ТНГ-Групп");
            las.WELL = vak.well_number();
            las.FLD  = vak.fild_name();
            las.LOC  = QString::fromUtf8("");
            las.CNTY = QString::fromUtf8("");
            las.STAT = QString::fromUtf8("");
            las.CTRY = QString::fromUtf8("");//("Россия");
            las.SRVC = QString::fromUtf8("");
            las.DATE = vak.log_date();
            switch ( vak.tool_type() )
            {
            case VAK_8:
                las.METD = QString::fromUtf8("ВАК-8");
                break;
            case VAK_8U:
                las.METD = QString::fromUtf8("ВАК-8У");
                break;
            case AKC:
                las.METD = QString::fromUtf8("АКЦ-8С");
                break;
            case VAK_8_M_09:
                las.METD = QString::fromUtf8("");
                break;
            case AKC_SD:
                las.METD = QString::fromUtf8("АКЦ-8С-SD");
                break;
            case AKC_MF:
                las.METD = QString::fromUtf8("АКЦ-8С-MF");
                break;
            case AKC_MF_SD:
                las.METD = QString::fromUtf8("АКЦ-8С-MF-SD");
                break;
            case VAK32:
                las.METD = QString::fromUtf8("ВАК-32");
                break;
            case AKC8S_FAST_SD:
                las.METD = QString::fromUtf8("АКЦ-8С-MF-FAST-SD");
                break;
            case AKC4S_FAST_SD:
                las.METD = QString::fromUtf8("АКЦ-4С-MF-FAST-SD");
                break;
            case VAK32_SHDSL:
                las.METD = QString::fromUtf8("ВАК-32-SHDSL");
                break;
            case UNKNOWN:
                las.METD = QString::fromUtf8("");
                break;
            default:
                las.METD = QString::fromUtf8("");
                break;
            }

            las.dept_step = 10;
            las.dept_start = ml_list.first().dept;
            las.dept_stop  = ml_list.last().dept;

            vak.clear();

            las.add_ml(&ml_list);

            while (ml_list.count() != 0)
            {
                ml_list.removeFirst();
            }

            ml_list.clear();

            if (grav_list.count() != 0)
            {
                //-----------------------------------------------------------
                //кривая угла Х
                //-----------------------------------------------------------
                data_list = new QList<PLas_Data>;
                for (i = 0; i < grav_list.count(); i++)
                {
                    grav = grav_list.at(i);
                    if (grav.is_valid_angle_x())
                    {
                        data = new TLas_Data;
                        data->dept = grav.get_dept();
                        data->data = grav.get_angle_x();
                        data_list->append(data);
                    }
                }
                las.add_data(data_list,
                             QString::fromUtf8("ANX"),
                             QString::fromUtf8("ГРАД"),
                             QString::fromUtf8("Угол отклонения прибора по оси Х"),
                             2);
                while (data_list->count() != 0)
                {
                    data = data_list->first();
                    delete data;
                    data_list->removeFirst();
                }
                delete data_list;

                //-----------------------------------------------------------
                //кривая ускорения X
                //-----------------------------------------------------------
                data_list = new QList<PLas_Data>;
                for (i = 0; i < grav_list.count(); i++)
                {
                    grav = grav_list.at(i);
                    if (grav.is_valid_accel_x())
                    {
                        data = new TLas_Data;
                        data->dept = grav.get_dept();
                        data->data = grav.get_accel_x();
                        data_list->append(data);
                    }
                }
                las.add_data(data_list,
                             QString::fromUtf8("ACX"),
                             QString::fromUtf8("g"),
                             QString::fromUtf8("Ускорение прибора по оси Х"),
                             3);
                while (data_list->count() != 0)
                {
                    data = data_list->first();
                    delete data;
                    data_list->removeFirst();
                }
                delete data_list;

                //-----------------------------------------------------------
                //кривая угла Z1
                //-----------------------------------------------------------
                data_list = new QList<PLas_Data>;
                for (i = 0; i < grav_list.count(); i++)
                {
                    grav = grav_list.at(i);
                    if (grav.is_valid_angle_z1())
                    {
                        data = new TLas_Data;
                        data->dept = grav.get_dept();
                        data->data = grav.get_angle_z1();
                        data_list->append(data);
                    }
                }
                las.add_data(data_list,
                             QString::fromUtf8("ANZ1"),
                             QString::fromUtf8("ГРАД"),
                             QString::fromUtf8("Угол отклонения прибора по оси Z"),
                             2);
                while (data_list->count() != 0)
                {
                    data = data_list->first();
                    delete data;
                    data_list->removeFirst();
                }
                delete data_list;

                //-----------------------------------------------------------
                //кривая ускорения Z1
                //-----------------------------------------------------------
                data_list = new QList<PLas_Data>;
                for (i = 0; i < grav_list.count(); i++)
                {
                    grav = grav_list.at(i);
                    if (grav.is_valid_accel_z1())
                    {
                        data = new TLas_Data;
                        data->dept = grav.get_dept();
                        data->data = grav.get_accel_z1();
                        data_list->append(data);
                    }
                }
                las.add_data(data_list,
                             QString::fromUtf8("ACZ1"),
                             QString::fromUtf8("g"),
                             QString::fromUtf8("Ускорение прибора по оси Z"),
                             3);
                while (data_list->count() != 0)
                {
                    data = data_list->first();
                    delete data;
                    data_list->removeFirst();
                }
                delete data_list;

                //-----------------------------------------------------------
                //кривая угла Y
                //-----------------------------------------------------------
                data_list = new QList<PLas_Data>;
                for (i = 0; i < grav_list.count(); i++)
                {
                    grav = grav_list.at(i);
                    if (grav.is_valid_angle_y())
                    {
                        data = new TLas_Data;
                        data->dept = grav.get_dept();
                        data->data = grav.get_angle_y();
                        data_list->append(data);
                    }
                }
                las.add_data(data_list,
                             QString::fromUtf8("ANY"),
                             QString::fromUtf8("ГРАД"),
                             QString::fromUtf8("Угол отклонения прибора по оси Y"),
                             2);
                while (data_list->count() != 0)
                {
                    data = data_list->first();
                    delete data;
                    data_list->removeFirst();
                }
                delete data_list;

                //-----------------------------------------------------------
                //кривая ускорения Y
                //-----------------------------------------------------------
                data_list = new QList<PLas_Data>;
                for (i = 0; i < grav_list.count(); i++)
                {
                    grav = grav_list.at(i);
                    if (grav.is_valid_accel_y())
                    {
                        data = new TLas_Data;
                        data->dept = grav.get_dept();
                        data->data = grav.get_accel_y();
                        data_list->append(data);
                    }
                }
                las.add_data(data_list,
                             QString::fromUtf8("ACY"),
                             QString::fromUtf8("g"),
                             QString::fromUtf8("Ускорение прибора по оси Y"),
                             3);
                while (data_list->count() != 0)
                {
                    data = data_list->first();
                    delete data;
                    data_list->removeFirst();
                }
                delete data_list;

                //-----------------------------------------------------------
                //кривая угла Z2
                //-----------------------------------------------------------
                data_list = new QList<PLas_Data>;
                for (i = 0; i < grav_list.count(); i++)
                {
                    grav = grav_list.at(i);
                    if (grav.is_valid_angle_z2())
                    {
                        data = new TLas_Data;
                        data->dept = grav.get_dept();
                        data->data = grav.get_angle_z2();
                        data_list->append(data);
                    }
                }
                las.add_data(data_list,
                             QString::fromUtf8("ANZ2"),
                             QString::fromUtf8("ГРАД"),
                             QString::fromUtf8("Угол отклонения прибора по оси Z"),
                             2);
                while (data_list->count() != 0)
                {
                    data = data_list->first();
                    delete data;
                    data_list->removeFirst();
                }
                delete data_list;

                //-----------------------------------------------------------
                //кривая ускорения Z2
                //-----------------------------------------------------------
                data_list = new QList<PLas_Data>;
                for (i = 0; i < grav_list.count(); i++)
                {
                    grav = grav_list.at(i);
                    if (grav.is_valid_accel_z2())
                    {
                        data = new TLas_Data;
                        data->dept = grav.get_dept();
                        data->data = grav.get_accel_z2();
                        data_list->append(data);
                    }
                }
                las.add_data(data_list,
                             QString::fromUtf8("ACZ2"),
                             QString::fromUtf8("g"),
                             QString::fromUtf8("Ускорение прибора по оси Z"),
                             3);
                while (data_list->count() != 0)
                {
                    data = data_list->first();
                    delete data;
                    data_list->removeFirst();
                }
                delete data_list;

                //-----------------------------------------------------------
                //кривая температуры
                //-----------------------------------------------------------
                data_list = new QList<PLas_Data>;
                for (i = 0; i < grav_list.count(); i++)
                {
                    grav = grav_list.at(i);
                    if (grav.is_valid_temp())
                    {
                        data = new TLas_Data;
                        data->dept = grav.get_dept();
                        data->data = grav.get_temp();
                        data_list->append(data);
                    }
                }
                las.add_data(data_list,
                             QString::fromUtf8("TEMP"),
                             QString::fromUtf8("ГРАД"),
                             QString::fromUtf8("Температура в приборе"),
                             3);
                while (data_list->count() != 0)
                {
                    data = data_list->first();
                    delete data;
                    data_list->removeFirst();
                }
                delete data_list;

                //-----------------------------------------------------------
                //кривая проекции угла XO'Z
                //-----------------------------------------------------------
                list1 = new QList<PLas_Data>;
                for (i = 0; i < grav_list.count(); i++)
                {
                    grav = grav_list.at(i);
                    if (grav.is_valid_accels())
                    {
                        data = new TLas_Data;
                        data->dept = grav.get_dept();
                        data->data = grav.get_angle_xoz();
                        list1->append(data);
                    }
                }

                //-----------------------------------------------------------
                //кривая проекции угла YO'Z
                //-----------------------------------------------------------
                list2 = new QList<PLas_Data>;
                for (i = 0; i < grav_list.count(); i++)
                {
                    grav = grav_list.at(i);
                    if (grav.is_valid_accels())
                    {
                        data = new TLas_Data;
                        data->dept = grav.get_dept();
                        data->data = grav.get_angle_yoz();
                        list2->append(data);
                    }
                }

                //-----------------------------------------------------------
                //перевожу углы XO'Z в "непрерывные линейные значения"
                //-----------------------------------------------------------
                float angle_base;
                float angle_shift;
                float angle_prev;
                float angle_cur;

                angle_base  = 0.0;
                angle_shift = 0.0;

//                data = new TLas_Data;

                if ((list1 != NULL) && (list1->count() != 0))
                {
                    data = (PLas_Data)list1->at(0);

                    angle_prev  = data->data;

                    for (i = 0; i < list1->count(); i++)
                    {
                        float a;
                        float b;
                        float c;

                        data = (PLas_Data)list1->at(i);
                        angle_cur = data->data;

                        a = fabs(angle_prev - (angle_cur + angle_shift));
                        b = fabs(angle_prev - (angle_cur + angle_shift + 360.0));
                        c = fabs(angle_prev - (angle_cur + angle_shift - 360.0));

                        if ((b < a) && (b < c)) angle_shift += 360.0;
                        if ((c < a) && (c < b)) angle_shift -= 360.0;

                        if (angle_shift < angle_base) angle_base = angle_shift;

                        data->data = angle_cur + angle_shift;
                        angle_prev = data->data;
                    }
                }

                //-----------------------------------------------------------
                //перевожу углы YO'Z в "непрерывные линейные значения"
                //-----------------------------------------------------------
                if ((list2 != NULL) && (list2->count() != 0))
                {
                    data = (PLas_Data)list2->at(0);

                    angle_shift = 0.0;
                    angle_prev = data->data;

                    for (i = 0; i < list2->count(); i++)
                    {
                        float a;
                        float b;
                        float c;

                        data = (PLas_Data)list2->at(i);
                        angle_cur = data->data;

                        a = fabs(angle_prev - (angle_cur + angle_shift));
                        b = fabs(angle_prev - (angle_cur + angle_shift + 360.0));
                        c = fabs(angle_prev - (angle_cur + angle_shift - 360.0));

                        if ((b < a) && (b < c)) angle_shift += 360.0;
                        if ((c < a) && (c < b)) angle_shift -= 360.0;

                        if (angle_shift < angle_base) angle_base = angle_shift;

                        data->data = angle_cur + angle_shift;
                        angle_prev = data->data;
                    }
                }

                if ((list1 != NULL) && (list1->count() != 0))
                {
                    for (i = 0; i < list1->count(); i++)
                    {
                        data = (PLas_Data)list1->at(i);
                        data->data -= angle_base;
                    }
                }

                if ((list2 != NULL) && (list2->count() != 0))
                {
                    for (i = 0; i < list2->count(); i++)
                    {
                        data = (PLas_Data)list2->at(i);
                        data->data -= angle_base;
                    }
                }

                //-----------------------------------------------------------
                //Вычисляю проекцию угла между осью Сферы-3 и осью прибора OZ на плоскость
                //-----------------------------------------------------------
                list3 = new QList<PLas_Data>;
                if ((list1 != NULL) && (list1->count() != 0))
                {
                    for (i = 0; i < list1->count(); i++)
                    {
                        PLas_Data data3 = new TLas_Data;
                        PLas_Data data1 = (PLas_Data)list1->at(i);

                        data3->data = data1->data + 180.0;
                        data3->dept = data1->dept;

                        list3->append(data3);
                    }
                }

                //-----------------------------------------------------------
                //Вычисляю проекцию угла между осью Сферы-4 и осью прибора OZ на плоскость
                //-----------------------------------------------------------
                list4 = new QList<PLas_Data>;
                if ((list2 != NULL) && (list2->count() != 0))
                {
                    for (i = 0; i < list2->count(); i++)
                    {
                        PLas_Data data4 = new TLas_Data;
                        PLas_Data data2 = (PLas_Data)list2->at(i);

                        data4->data = data2->data + 180.0;
                        data4->dept = data2->dept;

                        list4->append(data4);
                    }
                }

                //-----------------------------------------------------------
                //Вычисляю проекцию угла между осью Сферы-5 и осью прибора OZ на плоскость
                //-----------------------------------------------------------
                list5 = new QList<PLas_Data>;
                if ((list1 != NULL) && (list2 != NULL) && (list1->count() != 0) && (list2->count() != 0))
                {
                    for (i = 0; i < list1->count(); i++)
                    {
                        PLas_Data data5 = new TLas_Data;
                        PLas_Data data1 = (PLas_Data)list1->at(i);
                        PLas_Data data2 = (PLas_Data)list2->at(i);

                        data5->data = (data1->data + data2->data) / 2.0;
                        if (data1->data < data2->data) data5->data += 180.0;
                        data5->dept = data1->dept;

                        list5->append(data5);
                    }
                }

                //-----------------------------------------------------------
                //Вычисляю проекцию угла между осью Сферы-6 и осью прибора OZ на плоскость
                //-----------------------------------------------------------
                list6 = new QList<PLas_Data>;
                if ((list2 != NULL) && (list3 != NULL) && (list2->count() != 0) && (list3->count() != 0))
                {
                    for (i = 0; i < list2->count(); i++)
                    {
                        PLas_Data data6 = new TLas_Data;
                        PLas_Data data2 = (PLas_Data)list2->at(i);
                        PLas_Data data3 = (PLas_Data)list3->at(i);

                        data6->data = (data2->data + data3->data) / 2.0;
                        if (data2->data < data3->data) data6->data += 180.0;
                        data6->dept = data2->dept;

                        list6->append(data6);
                    }
                }

                //-----------------------------------------------------------
                //Вычисляю проекцию угла между осью Сферы-7 и осью прибора OZ на плоскость
                //-----------------------------------------------------------
                list7 = new QList<PLas_Data>;
                if ((list5 != NULL) && (list5->count() != 0))
                {
                    for (i = 0; i < list5->count(); i++)
                    {
                        PLas_Data data7 = new TLas_Data;
                        PLas_Data data5 = (PLas_Data)list5->at(i);

                        data7->data = data5->data + 180.0;
                        data7->dept = data5->dept;

                        list7->append(data7);
                    }
                }

                //-----------------------------------------------------------
                //Вычисляю проекцию угла между осью Сферы-8 и осью прибора OZ на плоскость
                //-----------------------------------------------------------
                list8 = new QList<PLas_Data>;
                if ((list6 != NULL) && (list6->count() != 0))
                {
                    for (i = 0; i < list6->count(); i++)
                    {
                        PLas_Data data8 = new TLas_Data;
                        PLas_Data data6 = (PLas_Data)list6->at(i);

                        data8->data = data6->data + 180.0;
                        data8->dept = data6->dept;

                        list8->append(data8);
                    }
                }

                //-----------------------------------------------------------
                // Сдвигаю углы XO'Z на целое число оборотов к нулю
                //-----------------------------------------------------------
                if (list1->count() != 0)
                {
                    angle_base = 0.0;
                    float min;
                    PLas_Data data_cur;
                    data_cur = (PLas_Data)list1->first();
                    min = data_cur->data;

                    for (i = 0; i < list1->count(); i++)
                    {
                        data_cur = (PLas_Data)list1->at(i);
                        if (min > data_cur->data) min = data_cur->data;
                    }

                    while (min >= 360.0)
                    {
                        min -= 360.0;
                        angle_base += 360.0;
                    }

                    for (i = 0; i < list1->count(); i++)
                    {
                        data_cur = (PLas_Data)list1->at(i);
                        data_cur->data -= angle_base;
                    }
                }

                //-----------------------------------------------------------
                // Сдвигаю углы YO'Z на целое число оборотов к нулю
                //-----------------------------------------------------------
                if (list2->count() != 0)
                {
                    angle_base = 0.0;
                    float min;
                    PLas_Data data_cur;
                    data_cur = (PLas_Data)list2->first();
                    min = data_cur->data;

                    for (i = 0; i < list2->count(); i++)
                    {
                        data_cur = (PLas_Data)list2->at(i);
                        if (min > data_cur->data) min = data_cur->data;
                    }

                    while (min >= 360.0)
                    {
                        min -= 360.0;
                        angle_base += 360.0;
                    }

                    for (i = 0; i < list2->count(); i++)
                    {
                        data_cur = (PLas_Data)list2->at(i);
                        data_cur->data -= angle_base;
                    }
                }

                //-----------------------------------------------------------
                // Сдвигаю проекцию угла между осью Сферы-3 и осью прибора OZ на плоскость на целое число оборотов к нулю
                //-----------------------------------------------------------
                if (list3->count() != 0)
                {
                    angle_base = 0.0;
                    float min;
                    PLas_Data data_cur;
                    data_cur = (PLas_Data)list3->first();
                    min = data_cur->data;

                    for (i = 0; i < list3->count(); i++)
                    {
                        data_cur = (PLas_Data)list3->at(i);
                        if (min > data_cur->data) min = data_cur->data;
                    }

                    while (min >= 360.0)
                    {
                        min -= 360.0;
                        angle_base += 360.0;
                    }

                    for (i = 0; i < list3->count(); i++)
                    {
                        data_cur = (PLas_Data)list3->at(i);
                        data_cur->data -= angle_base;
                    }
                }

                //-----------------------------------------------------------
                // Сдвигаю проекцию угла между осью Сферы-4 и осью прибора OZ на плоскость на целое число оборотов к нулю
                //-----------------------------------------------------------
                if (list4->count() != 0)
                {
                    angle_base = 0.0;
                    float min;
                    PLas_Data data_cur;
                    data_cur = (PLas_Data)list4->first();
                    min = data_cur->data;

                    for (i = 0; i < list4->count(); i++)
                    {
                        data_cur = (PLas_Data)list4->at(i);
                        if (min > data_cur->data) min = data_cur->data;
                    }

                    while (min >= 360.0)
                    {
                        min -= 360.0;
                        angle_base += 360.0;
                    }

                    for (i = 0; i < list4->count(); i++)
                    {
                        data_cur = (PLas_Data)list4->at(i);
                        data_cur->data -= angle_base;
                    }
                }

                //-----------------------------------------------------------
                // Сдвигаю проекцию угла между осью Сферы-5 и осью прибора OZ на плоскость на целое число оборотов к нулю
                //-----------------------------------------------------------
                if (list5->count() != 0)
                {
                    angle_base = 0.0;
                    float min;
                    PLas_Data data_cur;
                    data_cur = (PLas_Data)list5->first();
                    min = data_cur->data;

                    for (i = 0; i < list5->count(); i++)
                    {
                        data_cur = (PLas_Data)list5->at(i);
                        if (min > data_cur->data) min = data_cur->data;
                    }

                    while (min >= 360.0)
                    {
                        min -= 360.0;
                        angle_base += 360.0;
                    }

                    for (i = 0; i < list5->count(); i++)
                    {
                        data_cur = (PLas_Data)list5->at(i);
                        data_cur->data -= angle_base;
                    }
                }

                //-----------------------------------------------------------
                // Сдвигаю проекцию угла между осью Сферы-6 и осью прибора OZ на плоскость на целое число оборотов к нулю
                //-----------------------------------------------------------
                if (list6->count() != 0)
                {
                    angle_base = 0.0;
                    float min;
                    PLas_Data data_cur;
                    data_cur = (PLas_Data)list6->first();
                    min = data_cur->data;

                    for (i = 0; i < list6->count(); i++)
                    {
                        data_cur = (PLas_Data)list6->at(i);
                        if (min > data_cur->data) min = data_cur->data;
                    }

                    while (min >= 360.0)
                    {
                        min -= 360.0;
                        angle_base += 360.0;
                    }

                    for (i = 0; i < list6->count(); i++)
                    {
                        data_cur = (PLas_Data)list6->at(i);
                        data_cur->data -= angle_base;
                    }
                }

                //-----------------------------------------------------------
                // Сдвигаю проекцию угла между осью Сферы-7 и осью прибора OZ на плоскость на целое число оборотов к нулю
                //-----------------------------------------------------------
                if (list7->count() != 0)
                {
                    angle_base = 0.0;
                    float min;
                    PLas_Data data_cur;
                    data_cur = (PLas_Data)list7->first();
                    min = data_cur->data;

                    for (i = 0; i < list7->count(); i++)
                    {
                        data_cur = (PLas_Data)list7->at(i);
                        if (min > data_cur->data) min = data_cur->data;
                    }

                    while (min >= 360.0)
                    {
                        min -= 360.0;
                        angle_base += 360.0;
                    }

                    for (i = 0; i < list7->count(); i++)
                    {
                        data_cur = (PLas_Data)list7->at(i);
                        data_cur->data -= angle_base;
                    }
                }

                //-----------------------------------------------------------
                // Сдвигаю проекцию угла между осью Сферы-8 и осью прибора OZ на плоскость на целое число оборотов к нулю
                //-----------------------------------------------------------
                if (list8->count() != 0)
                {
                    angle_base = 0.0;
                    float min;
                    PLas_Data data_cur;
                    data_cur = (PLas_Data)list8->first();
                    min = data_cur->data;

                    for (i = 0; i < list8->count(); i++)
                    {
                        data_cur = (PLas_Data)list8->at(i);
                        if (min > data_cur->data) min = data_cur->data;
                    }

                    while (min >= 360.0)
                    {
                        min -= 360.0;
                        angle_base += 360.0;
                    }

                    for (i = 0; i < list8->count(); i++)
                    {
                        data_cur = (PLas_Data)list8->at(i);
                        data_cur->data -= angle_base;
                    }
                }

                //-----------------------------------------------------------
                // Добавляю полученные проекции углов в las-файл
                //-----------------------------------------------------------
                las.add_data(list1,
                              QString::fromUtf8("S1"),
                              QString::fromUtf8("ГРАД"),
                              QString::fromUtf8("Проекция угла между осью Сферы-1 и осью прибора OZ на плоскость"),
                              0);
                while (list1->count() != 0)
                {
                    data = list1->first();
                    delete data;
                    list1->removeFirst();
                }
                delete list1;

                las.add_data(list2,
                              QString::fromUtf8("S2"),
                              QString::fromUtf8("ГРАД"),
                              QString::fromUtf8("Проекция угла между осью Сферы-2 и осью прибора OZ на плоскость"),
                              0);
                while (list2->count() != 0)
                {
                    data = list2->first();
                    delete data;
                    list2->removeFirst();
                }
                delete list2;

                las.add_data(list3,
                              QString::fromUtf8("S3"),
                              QString::fromUtf8("ГРАД"),
                              QString::fromUtf8("Проекция угла между осью Сферы-3 и осью прибора OZ на плоскость"),
                              0);
                while (list3->count() != 0)
                {
                    data = list3->first();
                    delete data;
                    list3->removeFirst();
                }
                delete list3;

                las.add_data(list4,
                              QString::fromUtf8("S4"),
                              QString::fromUtf8("ГРАД"),
                              QString::fromUtf8("Проекция угла между осью Сферы-4 и осью прибора OZ на плоскость"),
                              0);
                while (list4->count() != 0)
                {
                    data = list4->first();
                    delete data;
                    list4->removeFirst();
                }
                delete list4;

                las.add_data(list5,
                              QString::fromUtf8("S5"),
                              QString::fromUtf8("ГРАД"),
                              QString::fromUtf8("Проекция угла между осью Сферы-5 и осью прибора OZ на плоскость"),
                              0);
                while (list5->count() != 0)
                {
                    data = list5->first();
                    delete data;
                    list5->removeFirst();
                }
                delete list5;

                las.add_data(list6,
                              QString::fromUtf8("S6"),
                              QString::fromUtf8("ГРАД"),
                              QString::fromUtf8("Проекция угла между осью Сферы-6 и осью прибора OZ на плоскость"),
                              0);
                while (list6->count() != 0)
                {
                    data = list6->first();
                    delete data;
                    list6->removeFirst();
                }
                delete list6;

                las.add_data(list7,
                              QString::fromUtf8("S7"),
                              QString::fromUtf8("ГРАД"),
                              QString::fromUtf8("Проекция угла между осью Сферы-7 и осью прибора OZ на плоскость"),
                              0);
                while (list7->count() != 0)
                {
                    data = list7->first();
                    delete data;
                    list7->removeFirst();
                }
                delete list7;

                las.add_data(list8,
                              QString::fromUtf8("S8"),
                              QString::fromUtf8("ГРАД"),
                              QString::fromUtf8("Проекция угла между осью Сферы-8 и осью прибора OZ на плоскость"),
                              0);
                while (list8->count() != 0)
                {
                    data = list8->first();
                    delete data;
                    list8->removeFirst();
                }
                delete list8;
            }

            nextFileName = QString::fromUtf8("%1.%2").arg(tempFileName).arg(QString::fromUtf8("las"));

            las.create_head();
            las.write_las(nextFileName);

            emit send_info(QString::fromUtf8("Результат записан в файл: %1").arg(nextFileName));
        }

        while (grav_list.count() != 0)
        {
            grav_list.removeFirst();
        }
        grav_list.clear();
        emit send_info(QString::fromUtf8("Обработка завершена."));
    }
}
//-----------------------------------------------------------------------------
