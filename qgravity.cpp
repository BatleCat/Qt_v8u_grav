//---------------------------------------------------------------------------
// Класс работы с данными акселерометров приборов серии ВАК/АКЦ
// Автор: Петрухин Андрей
// дата: 10/06/2019
//---------------------------------------------------------------------------
#include <math.h>
//---------------------------------------------------------------------------
#include "qgravity.h"
//---------------------------------------------------------------------------
QGravity::QGravity(void)// : QObject(parent)
{
    dept = 0;

    angle_x  = BAD_ANGLE;
    angle_y  = BAD_ANGLE;
    angle_z1 = BAD_ANGLE;
    angle_z2 = BAD_ANGLE;

    accel_x  = BAD_ACCEL;
    accel_y  = BAD_ACCEL;
    accel_z1 = BAD_ACCEL;
    accel_z2 = BAD_ACCEL;

    temp = BAD_TEMP;

}
//---------------------------------------------------------------------------
QGravity::~QGravity(void)
{

}
//---------------------------------------------------------------------------
void QGravity::set_dept(qint32 new_dept)
{
    dept = new_dept;
}
//---------------------------------------------------------------------------
void QGravity::set_angle_x (qint16 new_angle)
{
    angle_x = 0.1 * (float)new_angle;
}
//---------------------------------------------------------------------------
void QGravity::set_angle_y (qint16 new_angle)
{
    angle_y = 0.1 * (float)new_angle;
}
//---------------------------------------------------------------------------
void QGravity::set_angle_z1(qint16 new_angle)
{
    angle_z1 = 0.1 * (float)new_angle;
}
//---------------------------------------------------------------------------
void QGravity::set_angle_z2(qint16 new_angle)
{
    angle_z2 = 0.1 * (float)new_angle;
}
//---------------------------------------------------------------------------
void QGravity::set_accel_x (qint16 new_accel)
{
    accel_x = 0.0004625 * (float)new_accel;
}
//---------------------------------------------------------------------------
void QGravity::set_accel_y (qint16 new_accel)
{
    accel_y = 0.0004625 * (float)new_accel;
}
//---------------------------------------------------------------------------
void QGravity::set_accel_z1(qint16 new_accel)
{
    accel_z1 = 0.0004625 * (float)new_accel;
}
//---------------------------------------------------------------------------
void QGravity::set_accel_z2(qint16 new_accel)
{
    accel_z2 = 0.0004625 * (float)new_accel;
}
//---------------------------------------------------------------------------
void QGravity::set_temp(qint16 new_temp_int, qint16 new_temp_ppt)
{
    temp = (float)new_temp_int + (( (float)new_temp_ppt ) /100.0);
}
//---------------------------------------------------------------------------
bool QGravity::is_valid_angle_x (void)
{
    if (angle_x == BAD_ANGLE) return false;
    else return true;
}
//---------------------------------------------------------------------------
bool QGravity::is_valid_angle_y (void)
{
    if (angle_y == BAD_ANGLE) return false;
    else return true;
}
//---------------------------------------------------------------------------
bool QGravity::is_valid_angle_z1(void)
{
    if (angle_z1 == BAD_ANGLE) return false;
    else return true;
}
//---------------------------------------------------------------------------
bool QGravity::is_valid_angle_z2(void)
{
    if (angle_z2 == BAD_ANGLE) return false;
    else return true;
}
//---------------------------------------------------------------------------
bool QGravity::is_valid_accel_x (void)
{
    if (accel_x == BAD_ACCEL) return false;
    else return true;
}
//---------------------------------------------------------------------------
bool QGravity::is_valid_accel_y (void)
{
    if (accel_y == BAD_ACCEL) return false;
    else return true;
}
//---------------------------------------------------------------------------
bool QGravity::is_valid_accel_z1(void)
{
    if (accel_z1 == BAD_ACCEL) return false;
    else return true;
}
//---------------------------------------------------------------------------
bool QGravity::is_valid_accel_z2(void)
{
    if (accel_z2 == BAD_ACCEL) return false;
    else return true;
}
//---------------------------------------------------------------------------
bool QGravity::is_valid_temp(void)
{
    if (temp == BAD_TEMP) return false;
    else return true;
}
//---------------------------------------------------------------------------
qint32 QGravity::get_dept(void)
{
    return dept;
}
//---------------------------------------------------------------------------
float QGravity::get_angle_x (void)
{
    return angle_x;
}
//---------------------------------------------------------------------------
float QGravity::get_angle_y (void)
{
    return angle_y;
}
//---------------------------------------------------------------------------
float QGravity::get_angle_z1(void)
{
    return angle_z1;
}
//---------------------------------------------------------------------------
float QGravity::get_angle_z2(void)
{
    return angle_z2;
}
//---------------------------------------------------------------------------
float QGravity::get_accel_x (void)
{
    return accel_x;
}
//---------------------------------------------------------------------------
float QGravity::get_accel_y (void)
{
    return accel_y;
}
//---------------------------------------------------------------------------
float QGravity::get_accel_z1(void)
{
    return accel_z1;
}
//---------------------------------------------------------------------------
float QGravity::get_accel_z2(void)
{
    return accel_z2;
}
//---------------------------------------------------------------------------
float QGravity::get_temp(void)
{
    return temp;
}
//---------------------------------------------------------------------------
bool QGravity::is_valid_accels (void)
{
    float accel_z;

    if ( is_valid_accel_z2() )
    {
        accel_z = accel_z2;
    }
    else
    {
        if ( is_valid_accel_z1() ) accel_z = accel_z1;
        else return false;
    }

    if( !is_valid_accel_x() ) return false;
    if( !is_valid_accel_y() ) return false;
    if (accel_z <= 0.0) return false;
    if(accel_x == 0.0) return false;
    if(accel_y == 0.0) return false;

    float g;
    g = (accel_x * accel_x) + (accel_y * accel_y) + (accel_z * accel_z);
    if (g > SHOCK_LEVEL) return false;

    return true;
}
//---------------------------------------------------------------------------
float QGravity::get_angle_xoz (void)
{
    float sign;
    float accel_z;

    if ( is_valid_accel_z2() )
    {
        accel_z = accel_z2;
    }
    else
    {
        accel_z = accel_z1;
    }

    if (accel_y > 0.0)  sign = 1.0;
    else sign = -1.0;

    return (180.0 + sign * (180.0 * acos( (accel_x * accel_z) / sqrt( (accel_x * accel_x + accel_y * accel_y) * (accel_y * accel_y + accel_z * accel_z) ) ) / M_PI) );
}
//---------------------------------------------------------------------------
float QGravity::get_angle_yoz (void)
{
    float sign;
    float accel_z;

    if ( is_valid_accel_z2() )
    {
        accel_z = accel_z2;
    }
    else
    {
        accel_z = accel_z1;
    }

    if (accel_x > 0.0)  sign = 1.0;
    else sign = -1.0;

    return (180.0 - sign * (180.0 * acos( (accel_y * accel_z) / sqrt( (accel_x * accel_x + accel_y * accel_y) * (accel_x * accel_x + accel_z * accel_z) ) ) / M_PI) );
}
//---------------------------------------------------------------------------
bool operator <  (const QGravity& left, const QGravity& right)
{
    return ( left.dept < right.dept);
}
//---------------------------------------------------------------------------
bool operator >  (const QGravity& left, const QGravity& right)
{
//    return (left.get_dept() > right.get_dept());
    return (left.dept > right.dept);
}
//---------------------------------------------------------------------------
bool operator ==  (const QGravity& left, const QGravity& right)
{
    return (left.dept == right.dept);
}
//---------------------------------------------------------------------------
bool operator !=  (const QGravity& left, const QGravity& right)
{
    return (left.dept != right.dept);
}
//---------------------------------------------------------------------------
