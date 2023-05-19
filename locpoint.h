#ifndef LOCPOINT_H
#define LOCPOINT_H

#include<QString>
#include<math.h>

/*
#define PI 3.14159269932626
#define g_a 6378137
#define g_f 1/298.257222101
*/

const double PI = acos(-1.0);//atan(1)*4;
const long g_a = 6378137;
const double g_f = 1/298.257222101;
const double RAD = 180.0 / PI;
const double g_e2 = (2 - g_f) * g_f;
const  double g_e2p = g_e2 / (1 - g_e2);
const  double g_A = 1+3*g_e2/4 + 45*pow(g_e2,2)/64 + 525*pow(g_e2,3)/768 + 33075*pow(g_e2,4)/24576;
const  double g_B = 3*g_e2/4 + 15*pow(g_e2,2)/16 + 1575*pow(g_e2,3)/1536 + 4725*pow(g_e2,4)/3075;
const  double g_C = 15*pow(g_e2,2)/64 + 315*pow(g_e2,3)/768 + 6615*pow(g_e2,4)/6144;
const  double g_D = 105*pow(g_e2,3)/1536 + 945*pow(g_e2,4)/3072;
const  double g_E = 945*pow(g_e2,4)/24576;

class LocPoint
{
public:
    LocPoint(QString,  double,  double, double h = 0);
    void convert();


    double longtitude;
    double latitude;
    double height;
    double x_val;
    double y_val;
    QString name;
    int num;

    static double direction_calc(double lat1, double lon1, double lat2, double lon2);
    static double LBH_to_polar(LocPoint local, LocPoint target, double &az, double &el, double &azn, double &distance);
};

#endif // LOCPOINT_H
