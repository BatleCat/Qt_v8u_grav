//-----------------------------------------------------------------------------
#ifndef QT_VAK_8U_FILE_H
#define QT_VAK_8U_FILE_H
//-----------------------------------------------------------------------------
#include <QObject>
#include <QFile>
#include <QDataStream>
#include <QTextStream>
#include <QString>
#include <QList>
//-----------------------------------------------------------------------------
#include "tool_type.h"
//-----------------------------------------------------------------------------
#define VAK_8U_WAVE_NUM_POINTS 512
//-----------------------------------------------------------------------------
#define frame_pos_num      	     16
#define frame_pos_Rx_type  	     32
#define frame_pos_Ku       	     48
#define frame_pos_delay     	 64
#define frame_pos_freq     	     80
#define frame_pos_tool_type 	 96
#define frame_pos_mode_num      112
#define frame_pos_mode_count    128
#define frame_pos_angle_x		144
#define frame_pos_accel_x		160
#define frame_pos_angle_y		176
#define frame_pos_accel_y		192
#define frame_pos_angle_z1		208
#define frame_pos_accel_z1		224
#define frame_pos_angle_z2		240
#define frame_pos_accel_z2		256
#define frame_pos_temp_int		272
#define frame_pos_temp_ppt		288
#define frame_pos_calibr_ampl	304
#define frame_pos_calibr_ofset	320
#define frame_pos_tool_no		336
#define frame_pos_ver_major		352
#define frame_pos_ver_minor		368
#define frame_pos_Td    		384
//-----------------------------------------------------------------------------
//enum TTOOL_TYPE {TOOL_UNKNOW = 0x0, TOOL_VAK_8U = 0x01, TOOL_AKC = 0x02};
//-----------------------------------------------------------------------------
enum T_VAK_8U_error { VAK_8U_success,
                      VAK_8U_error,
                      VAK_8U_unknow_file,
                      VAK_8U_CRC_error,
                      VAK_8U_index_out_of_band,
                      VAK_8U_bad_pointer };
//-----------------------------------------------------------------------------
typedef qint16 TVAK_8U_WAVE[VAK_8U_WAVE_NUM_POINTS];
//-----------------------------------------------------------------------------
typedef TVAK_8U_WAVE* PVAK_8U_WAVE;
//-----------------------------------------------------------------------------
#pragma pack(1)
struct TVAK_8U_FRAME
{
    qint32          dept;
    qint8           ml;
    quint16         frame_number;
    quint16         Rx_type;
    quint16         Ku;
    quint16         delay;
    quint16         freq;
    TVAK_8U_WAVE    ch1;
    TVAK_8U_WAVE    ch2;
    TVAK_8U_WAVE    ch3;
    TVAK_8U_WAVE    ch4;
    TVAK_8U_WAVE    ch5;
    TVAK_8U_WAVE    ch6;
    TVAK_8U_WAVE    ch7;
    TVAK_8U_WAVE    ch8;
};
#pragma pack()
//-----------------------------------------------------------------------------
typedef TVAK_8U_FRAME* PVAK_8U_FRAME;
//-----------------------------------------------------------------------------
QDataStream& operator <<(QDataStream &out, const TVAK_8U_FRAME &vak_8u_frame);
QDataStream& operator >>(QDataStream &in, TVAK_8U_FRAME &vak_8u_frame);
//-----------------------------------------------------------------------------
class CVAK8U
{
private:
    int          Count;
    QString      File_Type;
    QString      Ver;
    QString      Well_Number;
    QString      Fild_Name;
    QString      Operator_Name;
    QString      Log_Date;
    QString      Start_Dept;
    TTOOL_TYPE   Tool_Type;
    int          Model;
    int          Number_of_Zondes;
    int          Shift_Point_VK1;
    int          Shift_Point_VK2;
    int          Shift_Point_VK3;
    int          Shift_Point_VK4;
    int          Shift_Point_VK5;
    int          Shift_Point_VK6;
    int          Shift_Point_VK7;
    int          Shift_Point_VK8;

    QList<PVAK_8U_FRAME>  data_list;

    void   load_head(const QString &file_name);
    void   load_well_sec(QTextStream &head);
    void   load_tool_sec(QTextStream &head);
    void   load_data(const QString &file_name);

public:
    CVAK8U(void);
    ~CVAK8U(void);
    void           load(const QString &file_name);
    void           clear(void);
    void           read_frame(const int index, TVAK_8U_FRAME &item);
    void           read_ch1(const int index, TVAK_8U_WAVE &wave);
    void           read_ch2(const int index, TVAK_8U_WAVE &wave);
    void           read_ch3(const int index, TVAK_8U_WAVE &wave);
    void           read_ch4(const int index, TVAK_8U_WAVE &wave);
    void           read_ch5(const int index, TVAK_8U_WAVE &wave);
    void           read_ch6(const int index, TVAK_8U_WAVE &wave);
    void           read_ch7(const int index, TVAK_8U_WAVE &wave);
    void           read_ch8(const int index, TVAK_8U_WAVE &wave);
    int            read_dept(const int index);
    bool           read_ml(const int index);
    unsigned short read_frame_number(const int index);
    unsigned short read_rx_type(const int index);
    unsigned short read_ku(const int index);
    unsigned short read_delay(const int index);
    unsigned short read_freq (const int index);
    unsigned short read_tool_type(const int index);
    unsigned short read_mode_num (const int index);
    unsigned short read_mode_count(const int index);
    short          read_angle_x (const int index);
    short          read_accel_x (const int index);
    short          read_angle_y (const int index);
    short          read_accel_y (const int index);
    short          read_angle_z1(const int index);
    short          read_accel_z1(const int index);
    short          read_angle_z2(const int index);
    short          read_accel_z2(const int index);
    short          read_temp_int(const int index);
    unsigned short read_temp_ppt(const int index);
    unsigned short read_calibr_ampl(const int index);
    unsigned short read_calibr_ofset(const int index);
    unsigned short read_tool_no(const int index);
    unsigned short read_ver_major(const int index);
    unsigned short read_ver_minor(const int index);
    unsigned short read_Td(const int index);

    int          count() const {return Count;}
    QString      file_type() const {return File_Type;}
    QString      ver() const {return Ver;}
    QString      well_number() const {return Well_Number;}
    QString      fild_name() const {return Fild_Name;}
    QString      operator_name() const {return Operator_Name;}
    QString      log_date() const {return Log_Date;}
    QString      start_dept() const {return Start_Dept;}
    TTOOL_TYPE   tool_type() const {return Tool_Type;}
    int          model() const {return Model;}
    int          number_of_zondes() const {return Number_of_Zondes;}
    int          shift_point_vk1() const  {return Shift_Point_VK1;}
    int          shift_point_vk2() const  {return Shift_Point_VK2;}
    int          shift_point_vk3() const  {return Shift_Point_VK3;}
    int          shift_point_vk4() const  {return Shift_Point_VK4;}
    int          shift_point_vk5() const  {return Shift_Point_VK5;}
    int          shift_point_vk6() const  {return Shift_Point_VK6;}
    int          shift_point_vk7() const  {return Shift_Point_VK7;}
    int          shift_point_vk8() const  {return Shift_Point_VK8;}
};
//-----------------------------------------------------------------------------
typedef CVAK8U* PVAK8U;
//-----------------------------------------------------------------------------
unsigned short get_frame_label (const TVAK_8U_WAVE &pocket);
unsigned short get_frame_number(const TVAK_8U_WAVE &pocket);
unsigned short get_rx_type(const TVAK_8U_WAVE &pocket);
unsigned short get_ku(const TVAK_8U_WAVE &pocket);
unsigned short get_delay(const TVAK_8U_WAVE &pocket);
unsigned short get_freq (const TVAK_8U_WAVE &pocket);
unsigned short get_CRC1 (const TVAK_8U_WAVE &pocket);
unsigned short calc_CRC1(const TVAK_8U_WAVE &pocket);
unsigned short get_CRC2 (const TVAK_8U_WAVE &pocket);
unsigned short calc_CRC2(const TVAK_8U_WAVE &pocket);
unsigned short get_CRC3 (const TVAK_8U_WAVE &pocket);
unsigned short calc_CRC3(const TVAK_8U_WAVE &pocket);
unsigned short get_CRC4 (const TVAK_8U_WAVE &pocket);
unsigned short calc_CRC4(const TVAK_8U_WAVE &pocket);
unsigned short get_CRC5 (const TVAK_8U_WAVE &pocket);
unsigned short calc_CRC5(const TVAK_8U_WAVE &pocket);
unsigned short get_CRC6 (const TVAK_8U_WAVE &pocket);
unsigned short calc_CRC6(const TVAK_8U_WAVE &pocket);
unsigned short get_CRC7 (const TVAK_8U_WAVE &pocket);
unsigned short calc_CRC7(const TVAK_8U_WAVE &pocket);
unsigned short get_CRC8 (const TVAK_8U_WAVE &pocket);
unsigned short calc_CRC8(const TVAK_8U_WAVE &pocket);
bool           is_valid_CRC (const TVAK_8U_WAVE &pocket);
unsigned short get_tool_type(const TVAK_8U_WAVE &pocket);
unsigned short get_mode_num (const TVAK_8U_WAVE &pocket);
unsigned short get_mode_count(const TVAK_8U_WAVE &pocket);

short          get_angle_x (const TVAK_8U_WAVE &pocket);
short          get_accel_x (const TVAK_8U_WAVE &pocket);
short          get_angle_y (const TVAK_8U_WAVE &pocket);
short          get_accel_y (const TVAK_8U_WAVE &pocket);
short          get_angle_z1(const TVAK_8U_WAVE &pocket);
short          get_accel_z1(const TVAK_8U_WAVE &pocket);
short          get_angle_z2(const TVAK_8U_WAVE &pocket);
short          get_accel_z2(const TVAK_8U_WAVE &pocket);
short          get_temp_int(const TVAK_8U_WAVE &pocket);
unsigned short get_temp_ppt(const TVAK_8U_WAVE &pocket);
unsigned short get_calibr_ampl(const TVAK_8U_WAVE &pocket);
unsigned short get_calibr_ofset(const TVAK_8U_WAVE &pocket);
unsigned short get_tool_no(const TVAK_8U_WAVE &pocket);
unsigned short get_ver_major(const TVAK_8U_WAVE &pocket);
unsigned short get_ver_minor(const TVAK_8U_WAVE &pocket);
unsigned short get_Td(const TVAK_8U_WAVE &pocket);

void write_v8u_head(const QString f_name, const QDate d, const QString fild, const QString well,
                    const QString name, const int dept, const QString tool,
                    const int wp1, const int wp2, const int wp3, const int wp4,
                    const int wp5, const int wp6, const int wp7, const int wp8);
void write_v8u_data(const QString f_name, const TVAK_8U_FRAME &data);
//-----------------------------------------------------------------------------
#endif	// QT_VAK_8U_FILE_
//-----------------------------------------------------------------------------
