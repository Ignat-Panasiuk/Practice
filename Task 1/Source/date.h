#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <cwctype>

class date
{
private:
    int day;
    int month;
    long long year;

public:

    int getday();
    int getmonth();
    int getyear();

    static int format_mode;

    date(): day(1), month(1), year(1) {}
    date(int d, int m, long long y): day(d), month(m), year(y) {}

    bool isleap();
    int checkdate();
    void getdate(std::wstring str);
    date getdate_txt(std::wstring str);


    long long date_to_days(date& d1);
    date days_to_date(long long days);


    static date today();


    long long operator-(date& other);
    date operator+(long long d);


    friend std::wistream& operator>>(std::wistream& in, date& d);
    friend std::wostream& operator<<(std::wostream& out, const date& d);
};