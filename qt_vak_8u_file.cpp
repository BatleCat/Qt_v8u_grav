//-----------------------------------------------------------------------------
//#include <stdio.h>
#include <QString>
#include <QStringList>
#include <QDate>
#include <QFile>
#include <QTextStream>
#include <QtDebug>
#include "vak_8.h"
#include "qt_vak_8u_file.h"
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
QDataStream& operator <<(QDataStream &out, const TVAK_8U_FRAME &vak_8u_frame)
{
    out.writeRawData((char*)(&vak_8u_frame), sizeof(TVAK_8U_FRAME));
    return out;
}
//-----------------------------------------------------------------------------
QDataStream& operator >>(QDataStream &in, TVAK_8U_FRAME &vak_8u_frame)
{
    in.readRawData((char*)(&vak_8u_frame), sizeof(TVAK_8U_FRAME));
    return in;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CVAK8U::CVAK8U(void)
{
    Count = 0;

    File_Type       = QString::fromUtf8("");
    Ver             = QString::fromUtf8("");
    Well_Number     = QString::fromUtf8("");
    Fild_Name       = QString::fromUtf8("");
    Operator_Name   = QString::fromUtf8("");
    Log_Date        = QString::fromUtf8("");
    Start_Dept      = QString::fromUtf8("");

    Tool_Type = UNKNOWN;

    Model = 0;

    Number_of_Zondes = 0;

    Shift_Point_VK1 = 0;
    Shift_Point_VK2 = 0;
    Shift_Point_VK3 = 0;
    Shift_Point_VK4 = 0;
    Shift_Point_VK5 = 0;
    Shift_Point_VK6 = 0;
    Shift_Point_VK7 = 0;
    Shift_Point_VK8 = 0;
}
//-----------------------------------------------------------------------------
CVAK8U::~CVAK8U(void)
{
    clear();
}
//-----------------------------------------------------------------------------
void CVAK8U::load(const QString &file_name)
{
    load_head(file_name);
    load_data(file_name);
}
//-----------------------------------------------------------------------------
void CVAK8U::load_head(const QString &file_name)
{
    QFile       f_data(file_name);
    QString     str;
    QTextStream head;

    File_Type = "";
    Ver = "";

    if (!f_data.open(QIODevice::ReadOnly))
    {
        qDebug() << f_data.errorString();
        throw VAK_8U_error;
    }

    head.setDevice(&f_data);
    head.setCodec("windows-1251");

    str = head.readLine();
    if (str.isEmpty()) throw VAK_8U_error;

    if (str != QString::fromUtf8("~head")) throw VAK_8U_unknow_file;

    str = head.readLine();
    QStringList list = str.split(" ", QString::SkipEmptyParts);
    if (list.at(0) != QString::fromUtf8("Формат")) throw VAK_8U_unknow_file;
    if (list.at(1) != QString::fromUtf8("GIS")   ) throw VAK_8U_unknow_file;
    File_Type = list.at(1);
    list.clear();

    str = head.readLine();
    list = str.split(" ", QString::SkipEmptyParts);
    if (list.at(0) != QString::fromUtf8("Версия")) throw VAK_8U_unknow_file;
    if (list.at(1) != QString::fromUtf8("1.0")   ) throw VAK_8U_unknow_file;
    else Ver = "1.0";
    list.clear();

    do
    {
        str = head.readLine();
        if (str == QString::fromUtf8("~well"))
        {
//            qDebug() << f_data.pos();
            load_well_sec(head);
            continue;
        }
        if (str == QString::fromUtf8("~tool"))
        {
            load_tool_sec(head);
            continue;
        }
    } while (str != QString::fromUtf8("~data"));

    f_data.close();
}
//-----------------------------------------------------------------------------
void CVAK8U::load_well_sec(QTextStream &head)
{
    qint64      pos;
    QString     str = "";

    Fild_Name       = "";
    Well_Number     = "";
    Log_Date        = "";
    Operator_Name   = "";
    Start_Dept      = "";


    do
    {
        pos = head.pos();

        str = head.readLine();
        if (str.isEmpty()) throw VAK_8U_error;      // Error

        QStringList list = str.split(" ", QString::SkipEmptyParts);

        if (list.count() > 1)
        {
            if (list.at(0) == QString::fromUtf8("Площадь"))
            {
                list.removeFirst();
                Fild_Name = list.takeFirst();
                while (!list.isEmpty())
                {
                    Fild_Name += QString::fromUtf8(" ");
                    Fild_Name += list.takeFirst();
                }
                continue;
            }
            if (list.at(0) == QString::fromUtf8("Скважина"))
            {
                list.removeFirst();
                Well_Number = list.takeFirst();
                while (!list.isEmpty())
                {
                    Well_Number += QString::fromUtf8(" ");
                    Well_Number += list.takeFirst();
                }
                continue;
            }
            if (list.at(0) == QString::fromUtf8("Дата"))
            {
                list.removeFirst();
                Log_Date = list.takeFirst();
                while (!list.isEmpty())
                {
                    Log_Date += QString::fromUtf8(" ");
                    Log_Date += list.takeFirst();
                }
                continue;
            }
            if (list.at(0) == QString::fromUtf8("Оператор"))
            {
                list.removeFirst();
                Operator_Name = list.takeFirst();
                while (!list.isEmpty())
                {
                    Operator_Name += QString::fromUtf8(" ");
                    Operator_Name += list.takeFirst();
                }
                continue;
            }
            if (list.at(0) == QString::fromUtf8("Глубина"))
            {
                list.removeFirst();
                Start_Dept = list.takeFirst();
                while (!list.isEmpty())
                {
                    Start_Dept += QString::fromUtf8(" ");
                    Start_Dept += list.takeFirst();
                }
                continue;
            }
        }
    } while (str.at(0) != '~');

    head.seek(pos);

    return;
}
//-----------------------------------------------------------------------------
void CVAK8U::load_tool_sec(QTextStream &head)
{
    qint64      pos;
    QString     str;

    Tool_Type = UNKNOWN;
    Model = 0;
    Number_of_Zondes = 0;
    Shift_Point_VK1 = 0;
    Shift_Point_VK2 = 0;
    Shift_Point_VK3 = 0;
    Shift_Point_VK4 = 0;
    Shift_Point_VK5 = 0;
    Shift_Point_VK6 = 0;
    Shift_Point_VK7 = 0;
    Shift_Point_VK8 = 0;

    do
    {
        pos = head.pos();

        str = head.readLine();
        if (str.isEmpty()) throw VAK_8U_error;

//        QStringList list = str.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        QStringList list = str.split(" ", QString::SkipEmptyParts);

        if (list.count() > 1)
        {
            if (list.at(0) == QString::fromUtf8("Прибор"))
            {
                if (list.at(1) == QString::fromUtf8("АКЦ"))
                {
                    Tool_Type = AKC;
                    continue;
                }
                if (list.at(1) == QString::fromUtf8("ВАК-8У"))
                    Tool_Type = VAK_8U;
                continue;
            }

            if (list.at(0) == QString::fromUtf8("Модель"))
            {
                Model = list.at(1).toInt();
                continue;
            }

            if (list.at(0) == QString::fromUtf8("Зонды"))
            {
                Number_of_Zondes = list.at(1).toInt();
                continue;
            }

            if ((list.at(0) == QString::fromUtf8("Точки")) && (list.at(0) == QString::fromUtf8("записи")))
            {
                int i;
                for (i = 0; i < 8; i++)
                {
                    pos = head.pos();
                    str = head.readLine();
                    if (str.at(0) == '~') break;

                    QStringList list1 = str.split(" ", QString::SkipEmptyParts);

                    if (list1.at(0) == QString::fromUtf8("ВК1")) Shift_Point_VK1 = list1.at(1).toInt();
                    if (list1.at(0) == QString::fromUtf8("ВК2")) Shift_Point_VK2 = list1.at(1).toInt();
                    if (list1.at(0) == QString::fromUtf8("ВК3")) Shift_Point_VK3 = list1.at(1).toInt();
                    if (list1.at(0) == QString::fromUtf8("ВК4")) Shift_Point_VK4 = list1.at(1).toInt();
                    if (list1.at(0) == QString::fromUtf8("ВК5")) Shift_Point_VK5 = list1.at(1).toInt();
                    if (list1.at(0) == QString::fromUtf8("ВК6")) Shift_Point_VK6 = list1.at(1).toInt();
                    if (list1.at(0) == QString::fromUtf8("ВК7")) Shift_Point_VK7 = list1.at(1).toInt();
                    if (list1.at(0) == QString::fromUtf8("ВК8")) Shift_Point_VK8 = list1.at(1).toInt();
                }
            }
        }
    } while (str.at(0) != '~');

    head.seek(pos);

    return;
}
//-----------------------------------------------------------------------------
void CVAK8U::load_data(const QString &file_name)
{
    QFile           f_data(file_name);
    qint64          pos;
    QTextStream     head;
    QDataStream     data;
    QString         s;
    PVAK_8U_FRAME   pData;

    if (!f_data.open(QIODevice::ReadOnly))
    {
        qDebug() << f_data.errorString();
        return;
    }

    head.setDevice(&f_data);
    head.setCodec("windows-1251");
    do
    {
        s = head.readLine();
    } while (s != QString::fromUtf8("~data"));

//    qDebug() << f_data.pos() << " " << head.pos();
    pos = head.pos();
    head.flush();

    f_data.seek(pos);
    data.setDevice(&f_data);
    Count = 0;
    while (!f_data.atEnd())
    {
        pData = new TVAK_8U_FRAME;
        data >> (*pData);
        data_list.append(pData);
        Count++;
    }
    f_data.close();
}
//-----------------------------------------------------------------------------
void CVAK8U::clear(void)
{
    while (!data_list.isEmpty())
        delete data_list.takeFirst();
}
//-----------------------------------------------------------------------------
void CVAK8U::read_frame(const int index, TVAK_8U_FRAME &item)
{
//    if (item == NULL) throw VAK_8U_bad_pointer;
//    if (index < 0)      throw VAK_8U_index_out_of_band;
//    if (index >= count) throw VAK_8U_index_out_of_band;

    item.dept = read_dept(index);
    read_ch1(index, item.ch1);
    read_ch2(index, item.ch2);
    read_ch3(index, item.ch3);
    read_ch4(index, item.ch4);
    read_ch5(index, item.ch5);
    read_ch6(index, item.ch6);
    read_ch7(index, item.ch7);
    read_ch8(index, item.ch8);
/*
    for (int i = 0; i < VAK_8U_WAVE_NUM_POINTS; i++)
    {
        item.ch1[i] = data_list[index]->ch1[i];
    }
*/
}
//-----------------------------------------------------------------------------
void CVAK8U::read_ch1(const int index, TVAK_8U_WAVE &wave)
{
    PVAK_8U_FRAME frame;

    if (index < 0)      throw VAK_8U_index_out_of_band;
    if (index >= Count) throw VAK_8U_index_out_of_band;

    frame = (PVAK_8U_FRAME) data_list[index];

    for (int i = 0; i < VAK_8U_WAVE_NUM_POINTS; i++)
    {
        qint16 x = frame->ch1[i];
        if (x & 0x2000) x = x | 0xC000;
        else x = x & 0x1FFF;
        wave[i] = x;
    }

//    if (!is_valid_CRC(frame->ch1)) throw VAK_8U_CRC_error;
}

//-----------------------------------------------------------------------------
void CVAK8U::read_ch2(const int index, TVAK_8U_WAVE &wave)
{
    PVAK_8U_FRAME frame;

    if (index < 0)      throw VAK_8U_index_out_of_band;
    if (index >= Count) throw VAK_8U_index_out_of_band;

    frame = (PVAK_8U_FRAME) data_list[index];

    for (int i = 0; i < VAK_8U_WAVE_NUM_POINTS; i++)
    {
        qint16 x = frame->ch2[i];
        if (x & 0x2000) x = x | 0xC000;
        else x = x & 0x1FFF;
        wave[i] = x;
    }

//    if (!is_valid_CRC(frame->ch2)) throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
void CVAK8U::read_ch3(const int index, TVAK_8U_WAVE &wave)
{
    PVAK_8U_FRAME frame;

    if (index < 0)      throw VAK_8U_index_out_of_band;
    if (index >= Count) throw VAK_8U_index_out_of_band;

    frame = (PVAK_8U_FRAME) data_list[index];

    for (int i = 0; i < VAK_8U_WAVE_NUM_POINTS; i++)
    {
        qint16 x = frame->ch3[i];
        if (x & 0x2000) x = x | 0xC000;
        else x = x & 0x1FFF;
        wave[i] = x;
    }

//    if (!is_valid_CRC(frame->ch3)) throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
void CVAK8U::read_ch4(const int index, TVAK_8U_WAVE &wave)
{
    PVAK_8U_FRAME frame;

    if (index < 0)      throw VAK_8U_index_out_of_band;
    if (index >= Count) throw VAK_8U_index_out_of_band;

    frame = (PVAK_8U_FRAME) data_list[index];

    for (int i = 0; i < VAK_8U_WAVE_NUM_POINTS; i++)
    {
        qint16 x = frame->ch4[i];
        if (x & 0x2000) x = x | 0xC000;
        else x = x & 0x1FFF;
        wave[i] = x;
    }

//    if (!is_valid_CRC(frame->ch4)) throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
void CVAK8U::read_ch5(const int index, TVAK_8U_WAVE &wave)
{
    PVAK_8U_FRAME frame;

    if (index < 0)      throw VAK_8U_index_out_of_band;
    if (index >= Count) throw VAK_8U_index_out_of_band;

    frame = (PVAK_8U_FRAME) data_list[index];

    for (int i = 0; i < VAK_8U_WAVE_NUM_POINTS; i++)
    {
        qint16 x = frame->ch5[i];
        if (x & 0x2000) x = x | 0xC000;
        else x = x & 0x1FFF;
        wave[i] = x;
    }

//    if (!is_valid_CRC(frame->ch5)) throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
void CVAK8U::read_ch6(const int index, TVAK_8U_WAVE &wave)
{
    PVAK_8U_FRAME frame;

    if (index < 0)      throw VAK_8U_index_out_of_band;
    if (index >= Count) throw VAK_8U_index_out_of_band;

    frame = (PVAK_8U_FRAME) data_list[index];
    for (int i = 0; i < VAK_8U_WAVE_NUM_POINTS; i++)
    {
        qint16 x = frame->ch6[i];
        if (x & 0x2000) x = x | 0xC000;
        else x = x & 0x1FFF;
        wave[i] = x;
    }

//    if (!is_valid_CRC(frame->ch6)) throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
void CVAK8U::read_ch7(const int index, TVAK_8U_WAVE &wave)
{
    PVAK_8U_FRAME frame;

    if (index < 0)      throw VAK_8U_index_out_of_band;
    if (index >= Count) throw VAK_8U_index_out_of_band;

    frame = (PVAK_8U_FRAME) data_list[index];

    for (int i = 0; i < VAK_8U_WAVE_NUM_POINTS; i++)
    {
        qint16 x = frame->ch7[i];
        if (x & 0x2000) x = x | 0xC000;
        else x = x & 0x1FFF;
        wave[i] = x;
    }

//    if (!is_valid_CRC(frame->ch7)) throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
void CVAK8U::read_ch8(const int index, TVAK_8U_WAVE &wave)
{
  PVAK_8U_FRAME frame;

  if (index < 0)      throw VAK_8U_index_out_of_band;
  if (index >= Count) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  for (int i = 0; i < VAK_8U_WAVE_NUM_POINTS; i++)
  {
      qint16 x = frame->ch8[i];
      if (x & 0x2000) x = x | 0xC000;
      else x = x & 0x1FFF;
      wave[i] = x;
  }

//  if (!is_valid_CRC(frame->ch8)) throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
int CVAK8U::read_dept(const int index)
{
  PVAK_8U_FRAME frame;

  if (index >= data_list.size()) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  return frame->dept;
}
//-----------------------------------------------------------------------------
bool CVAK8U::read_ml(const int index)
{
  PVAK_8U_FRAME frame;

  if (index >= data_list.size()) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  if (frame->ml == 0) return false;
  else return true;
}
//-----------------------------------------------------------------------------
unsigned short CVAK8U::read_frame_number(const int index)
{
  PVAK_8U_FRAME frame;

  if (index >= data_list.size()) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  if (get_CRC1(frame->ch1) == calc_CRC1(frame->ch1)) return get_frame_number(frame->ch1);
  else if (get_CRC1(frame->ch2) == calc_CRC1(frame->ch2)) return get_frame_number(frame->ch2);
       else if (get_CRC1(frame->ch3) == calc_CRC1(frame->ch3)) return get_frame_number(frame->ch3);
            else if (get_CRC1(frame->ch4) == calc_CRC1(frame->ch4)) return get_frame_number(frame->ch4);
                 else if (get_CRC1(frame->ch5) == calc_CRC1(frame->ch5)) return get_frame_number(frame->ch5);
                      else if (get_CRC1(frame->ch6) == calc_CRC1(frame->ch6)) return get_frame_number(frame->ch6);
                           else if (get_CRC1(frame->ch7) == calc_CRC1(frame->ch7)) return get_frame_number(frame->ch7);
                                else if (get_CRC1(frame->ch8) == calc_CRC1(frame->ch8)) return get_frame_number(frame->ch8);
                                     else throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
unsigned short CVAK8U::read_rx_type(const int index)
{
  PVAK_8U_FRAME frame;

  if (index >= data_list.size()) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  if (get_CRC1(frame->ch1) == calc_CRC1(frame->ch1)) return get_rx_type(frame->ch1);
  else if (get_CRC1(frame->ch2) == calc_CRC1(frame->ch2)) return get_rx_type(frame->ch2);
       else if (get_CRC1(frame->ch3) == calc_CRC1(frame->ch3)) return get_rx_type(frame->ch3);
            else if (get_CRC1(frame->ch4) == calc_CRC1(frame->ch4)) return get_rx_type(frame->ch4);
                 else if (get_CRC1(frame->ch5) == calc_CRC1(frame->ch5)) return get_rx_type(frame->ch5);
                      else if (get_CRC1(frame->ch6) == calc_CRC1(frame->ch6)) return get_rx_type(frame->ch6);
                           else if (get_CRC1(frame->ch7) == calc_CRC1(frame->ch7)) return get_rx_type(frame->ch7);
                                else if (get_CRC1(frame->ch8) == calc_CRC1(frame->ch8)) return get_rx_type(frame->ch8);
                                     else throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
unsigned short CVAK8U::read_ku(const int index)
{
  PVAK_8U_FRAME frame;

  if (index >= data_list.size()) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  if (get_CRC1(frame->ch1) == calc_CRC1(frame->ch1)) return get_ku(frame->ch1);
  else if (get_CRC1(frame->ch2) == calc_CRC1(frame->ch2)) return get_ku(frame->ch2);
       else if (get_CRC1(frame->ch3) == calc_CRC1(frame->ch3)) return get_ku(frame->ch3);
            else if (get_CRC1(frame->ch4) == calc_CRC1(frame->ch4)) return get_ku(frame->ch4);
                 else if (get_CRC1(frame->ch5) == calc_CRC1(frame->ch5)) return get_ku(frame->ch5);
                      else if (get_CRC1(frame->ch6) == calc_CRC1(frame->ch6)) return get_ku(frame->ch6);
                           else if (get_CRC1(frame->ch7) == calc_CRC1(frame->ch7)) return get_ku(frame->ch7);
                                else if (get_CRC1(frame->ch8) == calc_CRC1(frame->ch8)) return get_ku(frame->ch8);
                                     else throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
unsigned short CVAK8U::read_delay(const int index)
{
  PVAK_8U_FRAME frame;

  if (index >= data_list.size()) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  if (get_CRC2(frame->ch1) == calc_CRC2(frame->ch1)) return get_delay(frame->ch1);
  else if (get_CRC2(frame->ch2) == calc_CRC2(frame->ch2)) return get_delay(frame->ch2);
       else if (get_CRC2(frame->ch3) == calc_CRC2(frame->ch3)) return get_delay(frame->ch3);
            else if (get_CRC2(frame->ch4) == calc_CRC2(frame->ch4)) return get_delay(frame->ch4);
                 else if (get_CRC2(frame->ch5) == calc_CRC2(frame->ch5)) return get_delay(frame->ch5);
                      else if (get_CRC2(frame->ch6) == calc_CRC2(frame->ch6)) return get_delay(frame->ch6);
                           else if (get_CRC2(frame->ch7) == calc_CRC2(frame->ch7)) return get_delay(frame->ch7);
                                else if (get_CRC2(frame->ch8) == calc_CRC2(frame->ch8)) return get_delay(frame->ch8);
                                     else throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
unsigned short CVAK8U::read_freq(const int index)
{
  PVAK_8U_FRAME frame;

  if (index >= data_list.size()) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  if (get_CRC2(frame->ch1) == calc_CRC2(frame->ch1)) return get_freq(frame->ch1);
  else if (get_CRC2(frame->ch2) == calc_CRC2(frame->ch2)) return get_freq(frame->ch2);
       else if (get_CRC2(frame->ch3) == calc_CRC2(frame->ch3)) return get_freq(frame->ch3);
            else if (get_CRC2(frame->ch4) == calc_CRC2(frame->ch4)) return get_freq(frame->ch4);
                 else if (get_CRC2(frame->ch5) == calc_CRC2(frame->ch5)) return get_freq(frame->ch5);
                      else if (get_CRC2(frame->ch6) == calc_CRC2(frame->ch6)) return get_freq(frame->ch6);
                           else if (get_CRC2(frame->ch7) == calc_CRC2(frame->ch7)) return get_freq(frame->ch7);
                                else if (get_CRC2(frame->ch8) == calc_CRC2(frame->ch8)) return get_freq(frame->ch8);
                                     else throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
unsigned short CVAK8U::read_tool_type(const int index)
{
  PVAK_8U_FRAME frame;

  if (index >= data_list.size()) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  if (get_CRC2(frame->ch1) == calc_CRC2(frame->ch1)) return get_tool_type(frame->ch1);
  else if (get_CRC2(frame->ch2) == calc_CRC2(frame->ch2)) return get_tool_type(frame->ch2);
       else if (get_CRC2(frame->ch3) == calc_CRC2(frame->ch3)) return get_tool_type(frame->ch3);
            else if (get_CRC2(frame->ch4) == calc_CRC2(frame->ch4)) return get_tool_type(frame->ch4);
                 else if (get_CRC2(frame->ch5) == calc_CRC2(frame->ch5)) return get_tool_type(frame->ch5);
                      else if (get_CRC2(frame->ch6) == calc_CRC2(frame->ch6)) return get_tool_type(frame->ch6);
                           else if (get_CRC2(frame->ch7) == calc_CRC2(frame->ch7)) return get_tool_type(frame->ch7);
                                else if (get_CRC2(frame->ch8) == calc_CRC2(frame->ch8)) return get_tool_type(frame->ch8);
                                     else throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
unsigned short CVAK8U::read_mode_num(const int index)
{
  PVAK_8U_FRAME frame;

  if (index >= data_list.size()) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  if (get_CRC2(frame->ch1) == calc_CRC2(frame->ch1)) return get_mode_num(frame->ch1);
  else if (get_CRC2(frame->ch2) == calc_CRC2(frame->ch2)) return get_mode_num(frame->ch2);
       else if (get_CRC2(frame->ch3) == calc_CRC2(frame->ch3)) return get_mode_num(frame->ch3);
            else if (get_CRC2(frame->ch4) == calc_CRC2(frame->ch4)) return get_mode_num(frame->ch4);
                 else if (get_CRC2(frame->ch5) == calc_CRC2(frame->ch5)) return get_mode_num(frame->ch5);
                      else if (get_CRC2(frame->ch6) == calc_CRC2(frame->ch6)) return get_mode_num(frame->ch6);
                           else if (get_CRC2(frame->ch7) == calc_CRC2(frame->ch7)) return get_mode_num(frame->ch7);
                                else if (get_CRC2(frame->ch8) == calc_CRC2(frame->ch8)) return get_mode_num(frame->ch8);
                                     else throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
unsigned short CVAK8U::read_mode_count(const int index)
{
  PVAK_8U_FRAME frame;

  if (index >= data_list.size()) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  if (get_CRC3(frame->ch1) == calc_CRC3(frame->ch1)) return get_mode_count(frame->ch1);
  else if (get_CRC3(frame->ch2) == calc_CRC3(frame->ch2)) return get_mode_count(frame->ch2);
       else if (get_CRC3(frame->ch3) == calc_CRC3(frame->ch3)) return get_mode_count(frame->ch3);
            else if (get_CRC3(frame->ch4) == calc_CRC3(frame->ch4)) return get_mode_count(frame->ch4);
                 else if (get_CRC3(frame->ch5) == calc_CRC3(frame->ch5)) return get_mode_count(frame->ch5);
                      else if (get_CRC3(frame->ch6) == calc_CRC3(frame->ch6)) return get_mode_count(frame->ch6);
                           else if (get_CRC3(frame->ch7) == calc_CRC3(frame->ch7)) return get_mode_count(frame->ch7);
                                else if (get_CRC3(frame->ch8) == calc_CRC3(frame->ch8)) return get_mode_count(frame->ch8);
                                     else throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
short CVAK8U::read_angle_x(const int index)
{
  PVAK_8U_FRAME frame;

  if (index >= data_list.size()) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  if (get_CRC3(frame->ch1) == calc_CRC3(frame->ch1)) return get_angle_x(frame->ch1);
  else if (get_CRC3(frame->ch2) == calc_CRC3(frame->ch2)) return get_angle_x(frame->ch2);
       else if (get_CRC3(frame->ch3) == calc_CRC3(frame->ch3)) return get_angle_x(frame->ch3);
            else if (get_CRC3(frame->ch4) == calc_CRC3(frame->ch4)) return get_angle_x(frame->ch4);
                 else if (get_CRC3(frame->ch5) == calc_CRC3(frame->ch5)) return get_angle_x(frame->ch5);
                      else if (get_CRC3(frame->ch6) == calc_CRC3(frame->ch6)) return get_angle_x(frame->ch6);
                           else if (get_CRC3(frame->ch7) == calc_CRC3(frame->ch7)) return get_angle_x(frame->ch7);
                                else if (get_CRC3(frame->ch8) == calc_CRC3(frame->ch8)) return get_angle_x(frame->ch8);
                                     else throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
short CVAK8U::read_accel_x(const int index)
{
  PVAK_8U_FRAME frame;

  if (index >= data_list.size()) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  if (get_CRC3(frame->ch1) == calc_CRC3(frame->ch1)) return get_accel_x(frame->ch1);
  else if (get_CRC3(frame->ch2) == calc_CRC3(frame->ch2)) return get_accel_x(frame->ch2);
       else if (get_CRC3(frame->ch3) == calc_CRC3(frame->ch3)) return get_accel_x(frame->ch3);
            else if (get_CRC3(frame->ch4) == calc_CRC3(frame->ch4)) return get_accel_x(frame->ch4);
                 else if (get_CRC3(frame->ch5) == calc_CRC3(frame->ch5)) return get_accel_x(frame->ch5);
                      else if (get_CRC3(frame->ch6) == calc_CRC3(frame->ch6)) return get_accel_x(frame->ch6);
                           else if (get_CRC3(frame->ch7) == calc_CRC3(frame->ch7)) return get_accel_x(frame->ch7);
                                else if (get_CRC3(frame->ch8) == calc_CRC3(frame->ch8)) return get_accel_x(frame->ch8);
                                     else throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
short CVAK8U::read_angle_y(const int index)
{
  PVAK_8U_FRAME frame;

  if (index >= data_list.size()) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  if (get_CRC3(frame->ch1) == calc_CRC3(frame->ch1)) return get_angle_y(frame->ch1);
  else if (get_CRC3(frame->ch2) == calc_CRC3(frame->ch2)) return get_angle_y(frame->ch2);
       else if (get_CRC3(frame->ch3) == calc_CRC3(frame->ch3)) return get_angle_y(frame->ch3);
            else if (get_CRC3(frame->ch4) == calc_CRC3(frame->ch4)) return get_angle_y(frame->ch4);
                 else if (get_CRC3(frame->ch5) == calc_CRC3(frame->ch5)) return get_angle_y(frame->ch5);
                      else if (get_CRC3(frame->ch6) == calc_CRC3(frame->ch6)) return get_angle_y(frame->ch6);
                           else if (get_CRC3(frame->ch7) == calc_CRC3(frame->ch7)) return get_angle_y(frame->ch7);
                                else if (get_CRC3(frame->ch8) == calc_CRC3(frame->ch8)) return get_angle_y(frame->ch8);
                                     else throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
short CVAK8U::read_accel_y(const int index)
{
  PVAK_8U_FRAME frame;

  if (index >= data_list.size()) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  if (get_CRC4(frame->ch1) == calc_CRC4(frame->ch1)) return get_accel_y(frame->ch1);
  else if (get_CRC4(frame->ch2) == calc_CRC4(frame->ch2)) return get_accel_y(frame->ch2);
       else if (get_CRC4(frame->ch3) == calc_CRC4(frame->ch3)) return get_accel_y(frame->ch3);
            else if (get_CRC4(frame->ch4) == calc_CRC4(frame->ch4)) return get_accel_y(frame->ch4);
                 else if (get_CRC4(frame->ch5) == calc_CRC4(frame->ch5)) return get_accel_y(frame->ch5);
                      else if (get_CRC4(frame->ch6) == calc_CRC4(frame->ch6)) return get_accel_y(frame->ch6);
                           else if (get_CRC4(frame->ch7) == calc_CRC4(frame->ch7)) return get_accel_y(frame->ch7);
                                else if (get_CRC4(frame->ch8) == calc_CRC4(frame->ch8)) return get_accel_y(frame->ch8);
                                     else throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
short CVAK8U::read_angle_z1(const int index)
{
  PVAK_8U_FRAME frame;

  if (index >= data_list.size()) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  if (get_CRC4(frame->ch1) == calc_CRC4(frame->ch1)) return get_angle_z1(frame->ch1);
  else if (get_CRC4(frame->ch2) == calc_CRC4(frame->ch2)) return get_angle_z1(frame->ch2);
       else if (get_CRC4(frame->ch3) == calc_CRC4(frame->ch3)) return get_angle_z1(frame->ch3);
            else if (get_CRC4(frame->ch4) == calc_CRC4(frame->ch4)) return get_angle_z1(frame->ch4);
                 else if (get_CRC4(frame->ch5) == calc_CRC4(frame->ch5)) return get_angle_z1(frame->ch5);
                      else if (get_CRC4(frame->ch6) == calc_CRC4(frame->ch6)) return get_angle_z1(frame->ch6);
                           else if (get_CRC4(frame->ch7) == calc_CRC4(frame->ch7)) return get_angle_z1(frame->ch7);
                                else if (get_CRC4(frame->ch8) == calc_CRC4(frame->ch8)) return get_angle_z1(frame->ch8);
                                     else throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
short CVAK8U::read_accel_z1(const int index)
{
  PVAK_8U_FRAME frame;

  if (index >= data_list.size()) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  if (get_CRC4(frame->ch1) == calc_CRC4(frame->ch1)) return get_accel_z1(frame->ch1);
  else if (get_CRC4(frame->ch2) == calc_CRC4(frame->ch2)) return get_accel_z1(frame->ch2);
       else if (get_CRC4(frame->ch3) == calc_CRC4(frame->ch3)) return get_accel_z1(frame->ch3);
            else if (get_CRC4(frame->ch4) == calc_CRC4(frame->ch4)) return get_accel_z1(frame->ch4);
                 else if (get_CRC4(frame->ch5) == calc_CRC4(frame->ch5)) return get_accel_z1(frame->ch5);
                      else if (get_CRC4(frame->ch6) == calc_CRC4(frame->ch6)) return get_accel_z1(frame->ch6);
                           else if (get_CRC4(frame->ch7) == calc_CRC4(frame->ch7)) return get_accel_z1(frame->ch7);
                                else if (get_CRC4(frame->ch8) == calc_CRC4(frame->ch8)) return get_accel_z1(frame->ch8);
                                     else throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
short CVAK8U::read_angle_z2(const int index)
{
  PVAK_8U_FRAME frame;

  if (index >= data_list.size()) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  if (get_CRC4(frame->ch1) == calc_CRC4(frame->ch1)) return get_angle_z2(frame->ch1);
  else if (get_CRC4(frame->ch2) == calc_CRC4(frame->ch2)) return get_angle_z2(frame->ch2);
       else if (get_CRC4(frame->ch3) == calc_CRC4(frame->ch3)) return get_angle_z2(frame->ch3);
            else if (get_CRC4(frame->ch4) == calc_CRC4(frame->ch4)) return get_angle_z2(frame->ch4);
                 else if (get_CRC4(frame->ch5) == calc_CRC4(frame->ch5)) return get_angle_z2(frame->ch5);
                      else if (get_CRC4(frame->ch6) == calc_CRC4(frame->ch6)) return get_angle_z2(frame->ch6);
                           else if (get_CRC4(frame->ch7) == calc_CRC4(frame->ch7)) return get_angle_z2(frame->ch7);
                                else if (get_CRC4(frame->ch8) == calc_CRC4(frame->ch8)) return get_angle_z2(frame->ch8);
                                     else throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
short CVAK8U::read_accel_z2(const int index)
{
  PVAK_8U_FRAME frame;

  if (index >= data_list.size()) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  if (get_CRC5(frame->ch1) == calc_CRC5(frame->ch1)) return get_accel_z2(frame->ch1);
  else if (get_CRC5(frame->ch2) == calc_CRC5(frame->ch2)) return get_accel_z2(frame->ch2);
       else if (get_CRC5(frame->ch3) == calc_CRC5(frame->ch3)) return get_accel_z2(frame->ch3);
            else if (get_CRC5(frame->ch4) == calc_CRC5(frame->ch4)) return get_accel_z2(frame->ch4);
                 else if (get_CRC5(frame->ch5) == calc_CRC5(frame->ch5)) return get_accel_z2(frame->ch5);
                      else if (get_CRC5(frame->ch6) == calc_CRC5(frame->ch6)) return get_accel_z2(frame->ch6);
                           else if (get_CRC5(frame->ch7) == calc_CRC5(frame->ch7)) return get_accel_z2(frame->ch7);
                                else if (get_CRC5(frame->ch8) == calc_CRC5(frame->ch8)) return get_accel_z2(frame->ch8);
                                     else throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
short CVAK8U::read_temp_int(const int index)
{
  PVAK_8U_FRAME frame;

  if (index >= data_list.size()) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  if (get_CRC5(frame->ch1) == calc_CRC5(frame->ch1)) return get_temp_int(frame->ch1);
  else if (get_CRC5(frame->ch2) == calc_CRC5(frame->ch2)) return get_temp_int(frame->ch2);
       else if (get_CRC5(frame->ch3) == calc_CRC5(frame->ch3)) return get_temp_int(frame->ch3);
            else if (get_CRC5(frame->ch4) == calc_CRC5(frame->ch4)) return get_temp_int(frame->ch4);
                 else if (get_CRC5(frame->ch5) == calc_CRC5(frame->ch5)) return get_temp_int(frame->ch5);
                      else if (get_CRC5(frame->ch6) == calc_CRC5(frame->ch6)) return get_temp_int(frame->ch6);
                           else if (get_CRC5(frame->ch7) == calc_CRC5(frame->ch7)) return get_temp_int(frame->ch7);
                                else if (get_CRC5(frame->ch8) == calc_CRC5(frame->ch8)) return get_temp_int(frame->ch8);
                                     else throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
unsigned short CVAK8U::read_temp_ppt(const int index)
{
  PVAK_8U_FRAME frame;

  if (index >= data_list.size()) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  if (get_CRC5(frame->ch1) == calc_CRC5(frame->ch1)) return get_temp_ppt(frame->ch1);
  else if (get_CRC5(frame->ch2) == calc_CRC5(frame->ch2)) return get_temp_ppt(frame->ch2);
       else if (get_CRC5(frame->ch3) == calc_CRC5(frame->ch3)) return get_temp_ppt(frame->ch3);
            else if (get_CRC5(frame->ch4) == calc_CRC5(frame->ch4)) return get_temp_ppt(frame->ch4);
                 else if (get_CRC5(frame->ch5) == calc_CRC5(frame->ch5)) return get_temp_ppt(frame->ch5);
                      else if (get_CRC5(frame->ch6) == calc_CRC5(frame->ch6)) return get_temp_ppt(frame->ch6);
                           else if (get_CRC5(frame->ch7) == calc_CRC5(frame->ch7)) return get_temp_ppt(frame->ch7);
                                else if (get_CRC5(frame->ch8) == calc_CRC5(frame->ch8)) return get_temp_ppt(frame->ch8);
                                     else throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
unsigned short CVAK8U::read_calibr_ampl(const int index)
{
  PVAK_8U_FRAME frame;

  if (index >= data_list.size()) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  if (get_CRC5(frame->ch1) == calc_CRC5(frame->ch1)) return get_calibr_ampl(frame->ch1);
  else if (get_CRC5(frame->ch2) == calc_CRC5(frame->ch2)) return get_calibr_ampl(frame->ch2);
       else if (get_CRC5(frame->ch3) == calc_CRC5(frame->ch3)) return get_calibr_ampl(frame->ch3);
            else if (get_CRC5(frame->ch4) == calc_CRC5(frame->ch4)) return get_calibr_ampl(frame->ch4);
                 else if (get_CRC5(frame->ch5) == calc_CRC5(frame->ch5)) return get_calibr_ampl(frame->ch5);
                      else if (get_CRC5(frame->ch6) == calc_CRC5(frame->ch6)) return get_calibr_ampl(frame->ch6);
                           else if (get_CRC5(frame->ch7) == calc_CRC5(frame->ch7)) return get_calibr_ampl(frame->ch7);
                                else if (get_CRC5(frame->ch8) == calc_CRC5(frame->ch8)) return get_calibr_ampl(frame->ch8);
                                     else throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
unsigned short CVAK8U::read_calibr_ofset(const int index)
{
  PVAK_8U_FRAME frame;

  if (index >= data_list.size()) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  if (get_CRC6(frame->ch1) == calc_CRC6(frame->ch1)) return get_calibr_ofset(frame->ch1);
  else if (get_CRC6(frame->ch2) == calc_CRC6(frame->ch2)) return get_calibr_ofset(frame->ch2);
       else if (get_CRC6(frame->ch3) == calc_CRC6(frame->ch3)) return get_calibr_ofset(frame->ch3);
            else if (get_CRC6(frame->ch4) == calc_CRC6(frame->ch4)) return get_calibr_ofset(frame->ch4);
                 else if (get_CRC6(frame->ch5) == calc_CRC6(frame->ch5)) return get_calibr_ofset(frame->ch5);
                      else if (get_CRC6(frame->ch6) == calc_CRC6(frame->ch6)) return get_calibr_ofset(frame->ch6);
                           else if (get_CRC6(frame->ch7) == calc_CRC6(frame->ch7)) return get_calibr_ofset(frame->ch7);
                                else if (get_CRC6(frame->ch8) == calc_CRC6(frame->ch8)) return get_calibr_ofset(frame->ch8);
                                     else throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
unsigned short CVAK8U::read_tool_no(const int index)
{
  PVAK_8U_FRAME frame;

  if (index >= data_list.size()) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  if (get_CRC6(frame->ch1) == calc_CRC6(frame->ch1)) return get_tool_no(frame->ch1);
  else if (get_CRC6(frame->ch2) == calc_CRC6(frame->ch2)) return get_tool_no(frame->ch2);
       else if (get_CRC6(frame->ch3) == calc_CRC6(frame->ch3)) return get_tool_no(frame->ch3);
            else if (get_CRC6(frame->ch4) == calc_CRC6(frame->ch4)) return get_tool_no(frame->ch4);
                 else if (get_CRC6(frame->ch5) == calc_CRC6(frame->ch5)) return get_tool_no(frame->ch5);
                      else if (get_CRC6(frame->ch6) == calc_CRC6(frame->ch6)) return get_tool_no(frame->ch6);
                           else if (get_CRC6(frame->ch7) == calc_CRC6(frame->ch7)) return get_tool_no(frame->ch7);
                                else if (get_CRC6(frame->ch8) == calc_CRC6(frame->ch8)) return get_tool_no(frame->ch8);
                                     else throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
unsigned short CVAK8U::read_ver_major(const int index)
{
  PVAK_8U_FRAME frame;

  if (index >= data_list.size()) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  if (get_CRC6(frame->ch1) == calc_CRC6(frame->ch1)) return get_ver_major(frame->ch1);
  else if (get_CRC6(frame->ch2) == calc_CRC6(frame->ch2)) return get_ver_major(frame->ch2);
       else if (get_CRC6(frame->ch3) == calc_CRC6(frame->ch3)) return get_ver_major(frame->ch3);
            else if (get_CRC6(frame->ch4) == calc_CRC6(frame->ch4)) return get_ver_major(frame->ch4);
                 else if (get_CRC6(frame->ch5) == calc_CRC6(frame->ch5)) return get_ver_major(frame->ch5);
                      else if (get_CRC6(frame->ch6) == calc_CRC6(frame->ch6)) return get_ver_major(frame->ch6);
                           else if (get_CRC6(frame->ch7) == calc_CRC6(frame->ch7)) return get_ver_major(frame->ch7);
                                else if (get_CRC6(frame->ch8) == calc_CRC6(frame->ch8)) return get_ver_major(frame->ch8);
                                     else throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
unsigned short CVAK8U::read_ver_minor(const int index)
{
  PVAK_8U_FRAME frame;

  if (index >= data_list.size()) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  if (get_CRC6(frame->ch1) == calc_CRC6(frame->ch1)) return get_ver_minor(frame->ch1);
  else if (get_CRC6(frame->ch2) == calc_CRC6(frame->ch2)) return get_ver_minor(frame->ch2);
       else if (get_CRC6(frame->ch3) == calc_CRC6(frame->ch3)) return get_ver_minor(frame->ch3);
            else if (get_CRC6(frame->ch4) == calc_CRC6(frame->ch4)) return get_ver_minor(frame->ch4);
                 else if (get_CRC6(frame->ch5) == calc_CRC6(frame->ch5)) return get_ver_minor(frame->ch5);
                      else if (get_CRC6(frame->ch6) == calc_CRC6(frame->ch6)) return get_ver_minor(frame->ch6);
                           else if (get_CRC6(frame->ch7) == calc_CRC6(frame->ch7)) return get_ver_minor(frame->ch7);
                                else if (get_CRC6(frame->ch8) == calc_CRC6(frame->ch8)) return get_ver_minor(frame->ch8);
                                     else throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
unsigned short CVAK8U::read_Td(const int index)
{
  PVAK_8U_FRAME frame;

  if (index >= data_list.size()) throw VAK_8U_index_out_of_band;

  frame = (PVAK_8U_FRAME) data_list[index];

  if (get_CRC7(frame->ch1) == calc_CRC7(frame->ch1)) return get_Td(frame->ch1);
  else if (get_CRC7(frame->ch2) == calc_CRC7(frame->ch2)) return get_Td(frame->ch2);
       else if (get_CRC7(frame->ch3) == calc_CRC7(frame->ch3)) return get_Td(frame->ch3);
            else if (get_CRC7(frame->ch4) == calc_CRC7(frame->ch4)) return get_Td(frame->ch4);
                 else if (get_CRC7(frame->ch5) == calc_CRC7(frame->ch5)) return get_Td(frame->ch5);
                      else if (get_CRC7(frame->ch6) == calc_CRC7(frame->ch6)) return get_Td(frame->ch6);
                           else if (get_CRC7(frame->ch7) == calc_CRC7(frame->ch7)) return get_Td(frame->ch7);
                                else if (get_CRC7(frame->ch8) == calc_CRC7(frame->ch8)) return get_Td(frame->ch8);
                                     else throw VAK_8U_CRC_error;
}
//-----------------------------------------------------------------------------
unsigned short get_frame_label(const TVAK_8U_WAVE &pocket)
{
  int            i;
  unsigned short x;
  unsigned short frame_label;

  frame_label = 0;
  for(i = 0; i < 16; i++)
  {
    x = pocket[i] & 0x4000;
    if (x == 0x4000) frame_label = (frame_label | (1 << (15 - i)));
  }
  return frame_label;
}
//-----------------------------------------------------------------------------
unsigned short get_frame_number(const TVAK_8U_WAVE &pocket)
{
  int            i;
  unsigned short x;
  unsigned short frame_number;

  frame_number = 0;
  for(i = 0; i < 16; i++)
  {
    x = pocket[i + frame_pos_num] & 0x4000;
    if (x == 0x4000) frame_number = (frame_number | (1 << (15 - i)));
  }
  return frame_number;
}
//-----------------------------------------------------------------------------
unsigned short get_rx_type(const TVAK_8U_WAVE &pocket)
{
  int            i;
  unsigned short x;
  unsigned short Rx_type;

  Rx_type = 0;
  for(i = 0; i < 16; i++)
  {
    x = pocket[i + frame_pos_Rx_type] & 0x4000;
    if (x == 0x4000) Rx_type = (Rx_type | (1 << (15 - i)));
  }
  return Rx_type;
}
//-----------------------------------------------------------------------------
unsigned short get_ku(const TVAK_8U_WAVE &pocket)
{
  int            i;
  unsigned short x;
  unsigned short Ku;

  Ku = 0;
  for(i = 0; i < 16; i++)
  {
    x = pocket[i + frame_pos_Ku] & 0x4000;
    if (x == 0x4000) Ku = (Ku | (1 << (15 - i)));
  }
  return Ku;
}
//-----------------------------------------------------------------------------
unsigned short get_delay(const TVAK_8U_WAVE &pocket)
{
  int            i;
  unsigned short x;
  unsigned short delay;

  delay = 0;
  for(i = 0; i < 16; i++)
  {
    x = pocket[i + frame_pos_delay] & 0x4000;
    if (x == 0x4000) delay = (delay | (1 << (15 - i)));
  }
  return delay;
}
//-----------------------------------------------------------------------------
unsigned short get_freq(const TVAK_8U_WAVE &pocket)
{
  int            i;
  unsigned short x;
  unsigned short freq;

  freq = 0;
  for(i = 0; i < 16; i++)
  {
    x = pocket[i + frame_pos_freq] & 0x4000;
    if (x == 0x4000) freq = (freq | (1 << (15 - i)));
  }
  return freq;
}
//-----------------------------------------------------------------------------
unsigned short get_CRC1(const TVAK_8U_WAVE &pocket)
{
  const int crc_pos = 48;

  int            i;
  unsigned short crc_rx;

  crc_rx = 0;
  for(i = 0; i < 16; i++)
  {
    if ((pocket[crc_pos + i] & 0x8000) == 0x8000) crc_rx = (crc_rx | (1 << (15 - i)));
  }
  return crc_rx;
}
//-----------------------------------------------------------------------------
unsigned short get_CRC2(const TVAK_8U_WAVE &pocket)
{
  const int crc_pos = 48 + 64;

  int            i;
  unsigned short crc_rx;

  crc_rx = 0;
  for(i = 0; i < 16; i++)
  {
    if ((pocket[crc_pos + i] & 0x8000) == 0x8000) crc_rx = (crc_rx | (1 << (15 - i)));
  }
  return crc_rx;
}
//-----------------------------------------------------------------------------
unsigned short get_CRC3(const TVAK_8U_WAVE &pocket)
{
  const int crc_pos = 48 + 2 * 64;

  int            i;
  unsigned short crc_rx;

  crc_rx = 0;
  for(i = 0; i < 16; i++)
  {
    if ((pocket[crc_pos + i] & 0x8000) == 0x8000) crc_rx = (crc_rx | (1 << (15 - i)));
  }
  return crc_rx;
}
//-----------------------------------------------------------------------------
unsigned short get_CRC4(const TVAK_8U_WAVE &pocket)
{
  const int crc_pos = 48 + 3 * 64;

  int            i;
  unsigned short crc_rx;

  crc_rx = 0;
  for(i = 0; i < 16; i++)
  {
    if ((pocket[crc_pos + i] & 0x8000) == 0x8000) crc_rx = (crc_rx | (1 << (15 - i)));
  }
  return crc_rx;
}
//-----------------------------------------------------------------------------
unsigned short get_CRC5(const TVAK_8U_WAVE &pocket)
{
  const int crc_pos = 48 + 4 * 64;

  int            i;
  unsigned short crc_rx;

  crc_rx = 0;
  for(i = 0; i < 16; i++)
  {
    if ((pocket[crc_pos + i] & 0x8000) == 0x8000) crc_rx = (crc_rx | (1 << (15 - i)));
  }
  return crc_rx;
}
//-----------------------------------------------------------------------------
unsigned short get_CRC6(const TVAK_8U_WAVE &pocket)
{
  const int crc_pos = 48 + 5 * 64;

  int            i;
  unsigned short crc_rx;

  crc_rx = 0;
  for(i = 0; i < 16; i++)
  {
    if ((pocket[crc_pos + i] & 0x8000) == 0x8000) crc_rx = (crc_rx | (1 << (15 - i)));
  }
  return crc_rx;
}
//-----------------------------------------------------------------------------
unsigned short get_CRC7(const TVAK_8U_WAVE &pocket)
{
  const int crc_pos = 48 + 6 * 64;

  int            i;
  unsigned short crc_rx;

  crc_rx = 0;
  for(i = 0; i < 16; i++)
  {
    if ((pocket[crc_pos + i] & 0x8000) == 0x8000) crc_rx = (crc_rx | (1 << (15 - i)));
  }
  return crc_rx;
}
//-----------------------------------------------------------------------------
unsigned short get_CRC8(const TVAK_8U_WAVE &pocket)
{
  const int crc_pos = 48 + 7 * 64;

  int            i;
  unsigned short crc_rx;

  crc_rx = 0;
  for(i = 0; i < 16; i++)
  {
    if ((pocket[crc_pos + i] & 0x8000) == 0x8000) crc_rx = (crc_rx | (1 << (15 - i)));
  }
  return crc_rx;
}
//-----------------------------------------------------------------------------
unsigned short calc_CRC1(const TVAK_8U_WAVE &pocket)
{
  const int start = 0;
  const int stop  = start + 64;

  int            i;
  unsigned short crc_calc;

  crc_calc = 0;
  for(i = start; i < stop; i++)
  {
    crc_calc = crc_calc ^ (pocket[i] & 0x7FFF);
  }
  return crc_calc;
}
//-----------------------------------------------------------------------------
unsigned short calc_CRC2(const TVAK_8U_WAVE &pocket)
{
  const int start = 64;
  const int stop  = start + 64;

  int            i;
  unsigned short crc_calc;

  crc_calc = 0;
  for(i = start; i < stop; i++)
  {
    crc_calc = crc_calc ^ (pocket[i] & 0x7FFF);
  }
  return crc_calc;
}
//-----------------------------------------------------------------------------
unsigned short calc_CRC3(const TVAK_8U_WAVE &pocket)
{
  const int start = 2 * 64;
  const int stop  = start + 64;

  int            i;
  unsigned short crc_calc;

  crc_calc = 0;
  for(i = start; i < stop; i++)
  {
    crc_calc = crc_calc ^ (pocket[i] & 0x7FFF);
  }
  return crc_calc;
}
//-----------------------------------------------------------------------------
unsigned short calc_CRC4(const TVAK_8U_WAVE &pocket)
{
  const int start = 3 * 64;
  const int stop  = start + 64;

  int            i;
  unsigned short crc_calc;

  crc_calc = 0;
  for(i = start; i < stop; i++)
  {
    crc_calc = crc_calc ^ (pocket[i] & 0x7FFF);
  }
  return crc_calc;
}
//-----------------------------------------------------------------------------
unsigned short calc_CRC5(const TVAK_8U_WAVE &pocket)
{
  const int start = 4 * 64;
  const int stop  = start + 64;

  int            i;
  unsigned short crc_calc;

  crc_calc = 0;
  for(i = start; i < stop; i++)
  {
    crc_calc = crc_calc ^ (pocket[i] & 0x7FFF);
  }
  return crc_calc;
}
//-----------------------------------------------------------------------------
unsigned short calc_CRC6(const TVAK_8U_WAVE &pocket)
{
  const int start = 5 * 64;
  const int stop  = start + 64;

  int            i;
  unsigned short crc_calc;

  crc_calc = 0;
  for(i = start; i < stop; i++)
  {
    crc_calc = crc_calc ^ (pocket[i] & 0x7FFF);
  }
  return crc_calc;
}
//-----------------------------------------------------------------------------
unsigned short calc_CRC7(const TVAK_8U_WAVE &pocket)
{
  const int start = 6 * 64;
  const int stop  = start + 64;

  int            i;
  unsigned short crc_calc;

  crc_calc = 0;
  for(i = start; i < stop; i++)
  {
    crc_calc = crc_calc ^ (pocket[i] & 0x7FFF);
  }
  return crc_calc;
}
//-----------------------------------------------------------------------------
unsigned short calc_CRC8(const TVAK_8U_WAVE &pocket)
{
  const int start = 7 * 64;
  const int stop  = start + 64;

  int            i;
  unsigned short crc_calc;

  crc_calc = 0;
  for(i = start; i < stop; i++)
  {
    crc_calc = crc_calc ^ (pocket[i] & 0x7FFF);
  }
  return crc_calc;
}
//-----------------------------------------------------------------------------
bool is_valid_CRC(const TVAK_8U_WAVE &pocket)
{
  if ((get_CRC1(pocket) == calc_CRC1(pocket)) &&
      (get_CRC2(pocket) == calc_CRC2(pocket)) &&
      (get_CRC3(pocket) == calc_CRC3(pocket)) &&
      (get_CRC4(pocket) == calc_CRC4(pocket)) &&
      (get_CRC5(pocket) == calc_CRC5(pocket)) &&
      (get_CRC6(pocket) == calc_CRC6(pocket)) &&
      (get_CRC7(pocket) == calc_CRC7(pocket)) &&
      (get_CRC8(pocket) == calc_CRC8(pocket)) ) return true;
  return false;
}
//-----------------------------------------------------------------------------
unsigned short get_tool_type(const TVAK_8U_WAVE &pocket)
{
  int            i;
  unsigned short x;
  unsigned short tool_type;

  tool_type = 0;
  for(i = 0; i < 16; i++)
  {
    x = pocket[i + frame_pos_tool_type] & 0x4000;
    if (x == 0x4000) tool_type = (tool_type | (1 << (15 - i)));
  }
  return tool_type;
}
//---------------------------------------------------------------------------
unsigned short get_mode_num(const TVAK_8U_WAVE &pocket)
{
  int            i;
  unsigned short x;
  unsigned short mode_num;

  mode_num = 0;
  for(i = 0; i < 16; i++)
  {
    x = pocket[i + frame_pos_mode_num] & 0x4000;
    if (x == 0x4000) mode_num = (mode_num | (1 << (15 - i)));
  }
  return mode_num;
}
//---------------------------------------------------------------------------
unsigned short get_mode_count(const TVAK_8U_WAVE &pocket)
{
  int            i;
  unsigned short x;
  unsigned short mode_count;

  mode_count = 0;
  for(i = 0; i < 16; i++)
  {
    x = pocket[i + frame_pos_mode_count] & 0x4000;
    if (x == 0x4000) mode_count = (mode_count | (1 << (15 - i)));
  }
  return mode_count;
}
//---------------------------------------------------------------------------
short get_angle_x(const TVAK_8U_WAVE &pocket)
{
  int            i;
  unsigned short x;
  short angle_x;

  angle_x = 0;
  for(i = 0; i < 16; i++)
  {
    x = pocket[i + frame_pos_angle_x] & 0x4000;
    if (x == 0x4000) angle_x = (angle_x | (1 << (15 - i)));
  }
  return angle_x;
}
//---------------------------------------------------------------------------
short get_accel_x(const TVAK_8U_WAVE &pocket)
{
  int            i;
  unsigned short x;
  short accel_x;

  accel_x = 0;
  for(i = 0; i < 16; i++)
  {
    x = pocket[i + frame_pos_accel_x] & 0x4000;
    if (x == 0x4000) accel_x = (accel_x | (1 << (15 - i)));
  }
  return accel_x;
}
//---------------------------------------------------------------------------
short get_angle_y(const TVAK_8U_WAVE &pocket)
{
  int            i;
  unsigned short x;
  short angle_y;

  angle_y = 0;
  for(i = 0; i < 16; i++)
  {
    x = pocket[i + frame_pos_angle_y] & 0x4000;
    if (x == 0x4000) angle_y = (angle_y | (1 << (15 - i)));
  }
  return angle_y;
}
//---------------------------------------------------------------------------
short get_accel_y(const TVAK_8U_WAVE &pocket)
{
  int            i;
  unsigned short x;
  short accel_y;

  accel_y = 0;
  for(i = 0; i < 16; i++)
  {
    x = pocket[i + frame_pos_accel_y] & 0x4000;
    if (x == 0x4000) accel_y = (accel_y | (1 << (15 - i)));
  }
  return accel_y;
}
//---------------------------------------------------------------------------
short get_angle_z1(const TVAK_8U_WAVE &pocket)
{
  int            i;
  unsigned short x;
  short angle_z1;

  angle_z1 = 0;
  for(i = 0; i < 16; i++)
  {
    x = pocket[i + frame_pos_angle_z1] & 0x4000;
    if (x == 0x4000) angle_z1 = (angle_z1 | (1 << (15 - i)));
  }
  return angle_z1;
}
//---------------------------------------------------------------------------
short get_accel_z1(const TVAK_8U_WAVE &pocket)
{
  int            i;
  unsigned short x;
  short accel_z1;

  accel_z1 = 0;
  for(i = 0; i < 16; i++)
  {
    x = pocket[i + frame_pos_accel_z1] & 0x4000;
    if (x == 0x4000) accel_z1 = (accel_z1 | (1 << (15 - i)));
  }
  return accel_z1;
}
//---------------------------------------------------------------------------
short get_angle_z2(const TVAK_8U_WAVE &pocket)
{
  int            i;
  unsigned short x;
  short angle_z2;

  angle_z2 = 0;
  for(i = 0; i < 16; i++)
  {
    x = pocket[i + frame_pos_angle_z2] & 0x4000;
    if (x == 0x4000) angle_z2 = (angle_z2 | (1 << (15 - i)));
  }
  return angle_z2;
}
//---------------------------------------------------------------------------
short get_accel_z2(const TVAK_8U_WAVE &pocket)
{
  int            i;
  unsigned short x;
  short accel_z2;

  accel_z2 = 0;
  for(i = 0; i < 16; i++)
  {
    x = pocket[i + frame_pos_accel_z2] & 0x4000;
    if (x == 0x4000) accel_z2 = (accel_z2 | (1 << (15 - i)));
  }
  return accel_z2;
}
//---------------------------------------------------------------------------
short get_temp_int(const TVAK_8U_WAVE &pocket)
{
  int            i;
  unsigned short x;
  short temp_int;

  temp_int = 0;
  for(i = 0; i < 16; i++)
  {
    x = pocket[i + frame_pos_temp_int] & 0x4000;
    if (x == 0x4000) temp_int = (temp_int | (1 << (15 - i)));
  }
  return temp_int;
}
//---------------------------------------------------------------------------
unsigned short get_temp_ppt(const TVAK_8U_WAVE &pocket)
{
  int            i;
  unsigned short x;
  unsigned short temp_ppt;

  temp_ppt = 0;
  for(i = 0; i < 16; i++)
  {
    x = pocket[i + frame_pos_temp_ppt] & 0x4000;
    if (x == 0x4000) temp_ppt = (temp_ppt | (1 << (15 - i)));
  }
  return temp_ppt;
}
//---------------------------------------------------------------------------
unsigned short get_calibr_ampl(const TVAK_8U_WAVE &pocket)
{
    int            i;
    unsigned short x;
    unsigned short calibr_ampl;

    calibr_ampl = 0;
    for(i = 0; i < 16; i++)
    {
      x = pocket[i + frame_pos_calibr_ampl] & 0x4000;
      if (x == 0x4000) calibr_ampl = (calibr_ampl | (1 << (15 - i)));
    }
    return calibr_ampl;
}
//---------------------------------------------------------------------------
unsigned short get_calibr_ofset(const TVAK_8U_WAVE &pocket)
{
    int            i;
    unsigned short x;
    unsigned short calibr_ofset;

    calibr_ofset = 0;
    for(i = 0; i < 16; i++)
    {
      x = pocket[i + frame_pos_calibr_ofset] & 0x4000;
      if (x == 0x4000) calibr_ofset = (calibr_ofset | (1 << (15 - i)));
    }
    return calibr_ofset;
}
//---------------------------------------------------------------------------
unsigned short get_tool_no(const TVAK_8U_WAVE &pocket)
{
    int            i;
    unsigned short x;
    unsigned short tool_no;

    tool_no = 0;
    for(i = 0; i < 16; i++)
    {
      x = pocket[i + frame_pos_tool_no] & 0x4000;
      if (x == 0x4000) tool_no = (tool_no | (1 << (15 - i)));
    }
    return tool_no;
}
//---------------------------------------------------------------------------
unsigned short get_ver_major(const TVAK_8U_WAVE &pocket)
{
    int            i;
    unsigned short x;
    unsigned short ver_major;

    ver_major = 0;
    for(i = 0; i < 16; i++)
    {
      x = pocket[i + frame_pos_ver_major] & 0x4000;
      if (x == 0x4000) ver_major = (ver_major | (1 << (15 - i)));
    }
    return ver_major;
}
//---------------------------------------------------------------------------
unsigned short get_ver_minor(const TVAK_8U_WAVE &pocket)
{
    int            i;
    unsigned short x;
    unsigned short ver_minor;

    ver_minor = 0;
    for(i = 0; i < 16; i++)
    {
      x = pocket[i + frame_pos_ver_minor] & 0x4000;
      if (x == 0x4000) ver_minor = (ver_minor | (1 << (15 - i)));
    }
    return ver_minor;
}
//---------------------------------------------------------------------------
unsigned short get_Td(const TVAK_8U_WAVE &pocket)
{
    int            i;
    unsigned short x;
    unsigned short Td;

    Td = 0;
    for(i = 0; i < 16; i++)
    {
      x = pocket[i + frame_pos_Td] & 0x4000;
      if (x == 0x4000) Td = (Td | (1 << (15 - i)));
    }
    return Td;
}
//---------------------------------------------------------------------------
void write_v8u_head(const QString f_name, const QDate d, const QString fild, const QString well,
                    const QString name, const int dept, const QString tool,
                    const int wp1, const int wp2, const int wp3, const int wp4,
                    const int wp5, const int wp6, const int wp7, const int wp8)
{
  //---Запись шапки файла -----------------------
    QFile v8u_file(f_name);

    if (!v8u_file.open(QIODevice::WriteOnly))
    {
//      throw
        qDebug() << v8u_file.errorString();
        return;
    }
    QTextStream head(&v8u_file);
    head.setCodec("windows-1251");
//    head.setCodec("CP866");

    head << QString::fromUtf8("~head\r\n");
    head << QString::fromUtf8("  Формат GIS\r\n");
    head << QString::fromUtf8("  Версия 1.0\r\n");

    head << QString::fromUtf8("~well\r\n");
    head << QString::fromUtf8("  Площадь  %1\r\n").arg(fild);
    head << QString::fromUtf8("  Скважина №%1\r\n").arg(well);
    head << QString::fromUtf8("  Дата     %1.%2.%3\r\n").arg(d.day()).arg(d.month()).arg(d.year());
    head << QString::fromUtf8("  Оператор %1\r\n").arg(name);
    head << QString::fromUtf8("  Глубина  %1.%2 м.\r\n").arg(dept / 100).arg(dept % 100);

    head << QString::fromUtf8("~tool\r\n");
    head << QString::fromUtf8("  Прибор %1\r\n").arg(tool);
    head << QString::fromUtf8("  Модель 1\r\n");
    head << QString::fromUtf8("  Зонды  8\r\n");
    head << QString::fromUtf8("  Точки записи\r\n");
    head << QString::fromUtf8("    ВК1 %1\r\n").arg(wp1);
    head << QString::fromUtf8("    ВК2 %1\r\n").arg(wp2);
    head << QString::fromUtf8("    ВК3 %1\r\n").arg(wp3);
    head << QString::fromUtf8("    ВК4 %1\r\n").arg(wp4);
    head << QString::fromUtf8("    ВК5 %1\r\n").arg(wp5);
    head << QString::fromUtf8("    ВК6 %1\r\n").arg(wp6);
    head << QString::fromUtf8("    ВК7 %1\r\n").arg(wp7);
    head << QString::fromUtf8("    ВК8 %1\r\n").arg(wp8);

    head << QString::fromUtf8("~data\r\n");

    v8u_file.flush();
    v8u_file.close();
}
//---------------------------------------------------------------------------
void write_v8u_data(const QString f_name, const TVAK_8U_FRAME &data)
{
    QFile file(f_name);
    if (!file.open(QIODevice::Append))
    {
//        throw
        return;
    }

    QDataStream stream(&file);
    stream << data;

    file.flush();
    file.close();
}
//---------------------------------------------------------------------------
