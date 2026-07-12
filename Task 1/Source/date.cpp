#include "date.h"

date date::today()
{
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    return date(now->tm_mday, now->tm_mon + 1, now->tm_year + 1900);
}

int date::format_mode = 1;

int date::getday ()
{
    return day;
}

int date::getmonth ()
{
    return month;
}

int date::getyear ()
{
    return year;
}

bool date::isleap()
{
    if (year % 400 == 0 || (year % 100 != 0 && year % 4 == 0))
    {
        return true;
    }
    return false;
}

int date::checkdate()
{
    int days[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (year != 0)
    {
        if (month >= 1 && month <= 12)
        {
            if (day < 1 || (!isleap() && day > days[month - 1]) || (isleap() && ((month == 2 && day > 29) || (day > days[month - 1] && month != 2))))
            {
                if (isleap() && month == 2)
                {
                    std::cout << "Wrong day! For 2 month in leap year it should be from 1 to 29";
                }
                else
                {
                    std::cout << "Wrong day! For " << month << " month it should be from 1 to " << days[month - 1] << '\n';
                }
                return 4;
            }
        }
        else
        {
            std::cout << "Wrong month! It should be from 1 to 12\n";
            return 3;
        }
    }
    else
    {
        std::cout << "Wrong year! It can't be zero\n";
        return 2;
    }
    return 1;
}

const std::wstring day_txt[] = {
    L"первое", L"второе", L"третье", L"четвертое", L"пятое", L"шестое", L"седьмое", L"восьмое", L"девятое", // 1-9
    L"десятое", L"одиннадцатое", L"двенадцатое", L"тринадцатое", L"четырнадцатое", L"пятнадцатое",
    L"шестнадцатое", L"семнадцатое", L"восемнадцатое", L"девятнадцатое", // 10-19
    L"двадцать", L"двадцатое", // 20
    L"тридцать", L"тридцатое"   // 30
};


const int day_values[] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    20, 20,
    30, 30
};


const std::wstring month_txt[] = {
    L"января", L"январь",
    L"февраля", L"февраль",
    L"марта", L"март",
    L"апреля", L"апрель",
    L"мая", L"май",
    L"июня", L"июнь",
    L"июля", L"июль",
    L"августа", L"август",
    L"сентября", L"сентябрь",
    L"октября", L"октябрь",
    L"ноября", L"ноябрь",
    L"декабря", L"декабрь"
};


const int month_values[] = {
    1, 1,
    2, 2,
    3, 3,
    4, 4,
    5, 5,
    6, 6,
    7, 7,
    8, 8,
    9, 9,
    10, 10,
    11, 11,
    12, 12
};

date date::getdate_txt(std::wstring str)
{
    date temp;
    std::wstringstream ss(str);
    std::wstring words[15];
    char i = 0;
    char k = 0;
    while (i < 15 && ss >> words[i])
    {
        k = 0;
        while (k < words[i].length())
        {
            words[i][k] = std::towlower(words[i][k]);
            k = k + 1;
        }
        i = i + 1;
    }
    k = 0;

    char counter = 0;
    char m = 0;
    bool induk = 0;
    temp.day = 0;
    while (k < i && counter < 2)
    {
        std::wstring pot_day = words[k];
        m = 0;
        induk = 0;
        while (m < 23 && !induk)
        {
            if (pot_day == day_txt[m])
            {
                if (day_values[m] >= 10 && day_values[m] <= 19)
                {
                    counter = 2;
                }
                else
                {
                    counter = counter + 1;
                }
                temp.day = temp.day + day_values[m];
                induk = 1;
            }
            m = m + 1;
        }
        k = k + 1;
    }

    k = 0;
    temp.month = 0;
    induk = 0;
    while (k < i && !induk)
    {
        std::wstring pot_month = words[k];
        m = 0;
        while (m < 24 && !induk)
        {
            if (pot_month == month_txt[m])
            {
                temp.month = temp.month + month_values[m];
                induk = 1;
            }
            m = m + 1;
        }
        k = k + 1;
    }

    k = 0;
    temp.year = 0;
    bool found_year = 0;
    while (k < i && !found_year)
    {
        induk = 1;
        m = 0;
        std::wstring pot_year = words[k];
        if (pot_year.length() > 1 && pot_year[0] == L'-')
        {
            m = 1;
        }
        while (m < pot_year.length() && induk)
        {
            if (pot_year[m] < L'0' || pot_year[m] > L'9')
            {
                induk = 0;
            }
            m = m + 1;
        }
        if (induk)
        {
            found_year = 1;
            temp.year = std::stoi(pot_year);
        }
        k = k + 1;
    }

    if (temp.year > 0)
    {
        k = 0;
        bool induk1 = 0;
        bool induk2 = 0;
        bool induk3 = 0;
        while (k < i && !(induk1 == 1 && induk2 == 1 && induk3 == 1))
        {
            std::wstring year_type = words[k];
            if (year_type == L"до")
            {
                induk1 = 1;
            }
            else if (year_type == L"нашей")
            {
                induk2 = 1;
            }
            else if (year_type == L"эры")
            {
                induk3 = 1;
            }
            k = k + 1;
        }
        if (induk1 == 1 && induk2 == 1 && induk3 == 1)
        {
            temp.year = -temp.year;
        }
    }

    return temp;
}

void date::getdate(std::wstring str)
{
    wchar_t det;
    date temp;
    temp.day = 0;
    temp.month = 0;
    temp.year = 0;
    std::wstringstream ss(str);
    bool parsed = false;
    if (date::format_mode == 1)
    {
        ss >> temp.day >> det >> temp.month >> det >> temp.year;
        if (!ss.fail())
        {
            parsed = true;
        }
    }
    else if (date::format_mode == 2)
    {
        temp = getdate_txt(str);
        if (temp.day != 0 || temp.month != 0 || temp.year != 0)
        {
            parsed = true;
        }
    }
    else if (date::format_mode == 3)
    {
        ss >> temp.year >> det >> temp.month >> det >> temp.day;
        if (!ss.fail())
        {
            parsed = true;
        }
    }
    if (parsed)
    {
        *this = temp;
    }
    else
    {
        this->day = 0;
        this->month = 0;
        this->year = 0;
    }
}

long long date::date_to_days(date& d1)
{
    long long days1;
    int days[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    long long temp1 = d1.year;
    if (temp1 < 0)
    {
        temp1++;
    }
    days1 = (temp1 - 1) * 365LL + (temp1 - 1) / 4 - (temp1 - 1) / 100 + (temp1 - 1) / 400 + d1.day;
    if (d1.isleap() && d1.month > 2)
    {
        days1 = days1 + 1;
    }
    int i = 0;
    while (i < d1.month - 1)
    {
        days1 = days1 + days[i];
        i = i + 1;
    }
    return days1;
}

date date::days_to_date(long long days)
{
    long long d = days;
    this->year = 1;
    if (days > 0)
    {
        while ((isleap() && d > 366) || (!isleap() && d > 365))
        {
            if (isleap())
            {
                d = d - 366;
            }
            else
            {
                d = d - 365;
            }
            this->year = this->year + 1;
        }
    }
    else
    {
        this-> year = 0;
        while (d <= 0)
        {
            this->year = this->year - 1;
            if (isleap())
            {
                d = d + 366;
            }
            else
            {
                d = d + 365;
            }
        }
    }
    int months[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (isleap())
    {
        months[1] = 29;
    }
    int m = 0;
    while (d > months[m])
    {
        d = d - months[m];
        m = m + 1;
    }
    this->month = m + 1;
    this->day = d;


    return *this;
}

long long date::operator-(date& other)
{
    return date_to_days(*this) - date_to_days(other);
}

date date::operator+(long long d)
{
    long long temp;
    temp = date_to_days(*this) + d;
    date res;
    return res.days_to_date(temp);
}

std::wistream& operator>>(std::wistream& in, date& d)
{
    std::wstring buffer;
    std::getline(in, buffer);
    if (buffer.length() > 0)
    {
        d.getdate(buffer);
    }
    return in;
}

std::wostream& operator<<(std::wostream& out, const date& d)
{
    if (date::format_mode == 1)
    {
        out << d.day << L"." << d.month << L"." << d.year;
    }
    else if (date::format_mode == 3)
    {
        out << d.year << L"." << d.month << L"." << d.day;
    }
    else if (date::format_mode == 2)
    {

        if (d.day <= 19)
        {
            out << day_txt[d.day - 1];
        }
        else if (d.day == 20)
        {
            out << day_txt[20];
        }
        else if (d.day < 30)
        {
            out << day_txt[19] << L" " << day_txt[d.day - 21];
        }
        else if (d.day == 30)
        {
            out << day_txt[22];
        }
        else if (d.day == 31)
        {
            out << day_txt[21] << L" " << day_txt[0];
        }

        out << L" ";

        out << month_txt[(d.month - 1) * 2] << L" ";

        out << std::abs(d.year) << L" года";

        if (d.year < 0) {
            out << L" до нашей эры";
        }
    }
    return out;
}