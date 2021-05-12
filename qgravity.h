//---------------------------------------------------------------------------
// Класс работы с данными акселерометров приборов серии ВАК/АКЦ
// Автор: Петрухин Андрей
// дата: 10/06/2019
//---------------------------------------------------------------------------
#ifndef QGRAVITY_H
#define QGRAVITY_H
//---------------------------------------------------------------------------
#include <QObject>
//---------------------------------------------------------------------------
#define BAD_ANGLE ((float)384.0)      // (      0.1 * 0x0F00);
#define BAD_ACCEL ((float)1.776)      // (0.0004625 * 0x0F00);
#define BAD_TEMP  ((float)-272.98)    // 1912 соответсвует -272.98 градуса
#define SHOCK_LEVEL ((float)2.0)        // Порог ускорения удара

//---------------------------------------------------------------------------
class QGravity
{
public:
    QGravity(void);
    ~QGravity(void);

    qint32  dept;

private:
//    const float BAD_ANGLE = 384.0;      // (      0.1 * 0x0F00);
//    const float BAD_ACCEL = 1.776;      // (0.0004625 * 0x0F00);
//    const float BAD_TEMP  = -272.98;    // 1912 соответсвует -272.98 градуса
//    const float SHOCK_LEVEL = 2.0;        // Порог ускорения удара

    float   angle_x;
    float   angle_y;
    float   angle_z1;
    float   angle_z2;

    float   accel_x;
    float   accel_y;
    float   accel_z1;
    float   accel_z2;

    float   temp;

public:
    void set_dept(qint32 new_dept);

    void set_angle_x (qint16 new_angle);
    void set_angle_y (qint16 new_angle);
    void set_angle_z1(qint16 new_angle);
    void set_angle_z2(qint16 new_angle);

    void set_accel_x (qint16 new_accel);
    void set_accel_y (qint16 new_accel);
    void set_accel_z1(qint16 new_accel);
    void set_accel_z2(qint16 new_accel);

    void set_temp(qint16 new_temp_int, qint16 new_temp_ppt);

    bool is_valid_angle_x (void);
    bool is_valid_angle_y (void);
    bool is_valid_angle_z1(void);
    bool is_valid_angle_z2(void);

    bool is_valid_accel_x (void);
    bool is_valid_accel_y (void);
    bool is_valid_accel_z1(void);
    bool is_valid_accel_z2(void);

    bool is_valid_temp(void);

    qint32 get_dept(void);

    float get_angle_x (void);
    float get_angle_y (void);
    float get_angle_z1(void);
    float get_angle_z2(void);

    float get_accel_x (void);
    float get_accel_y (void);
    float get_accel_z1(void);
    float get_accel_z2(void);

    float get_temp(void);

    bool is_valid_accels (void);
    float get_angle_xoz (void);
    float get_angle_yoz (void);

};

typedef QGravity* PGravity;

bool operator <  (const QGravity& left, const QGravity& right);
bool operator >  (const QGravity& left, const QGravity& right);
bool operator == (const QGravity& left, const QGravity& right);
bool operator != (const QGravity& left, const QGravity& right);

//---------------------------------------------------------------------------
#endif // QGRAVITY_H
//---------------------------------------------------------------------------
