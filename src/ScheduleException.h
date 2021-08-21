/***************************************************************
 * Name:      wxDFast.h
 * Author:    Max Magalhães Velasques (maxvelasques@gmail.com)
 *            David Vachulka (archdvx@dxsolutions.org)
 * Copyright: 2006
 * License:   GPL2
 **************************************************************/

#ifndef SCHEDULEEXCEPTION_H
#define SCHEDULEEXCEPTION_H

#include <wx/defs.h>
#include "UtilFunctions.h"

class mScheduleException
{
public:
    bool isactive;
    wxUint16 day;
    wxUint16 startHour;
    wxUint16 startMinute;
    wxUint16 finishHour;
    wxUint16 finishMinute;

    wxString ToWxstring()
    {
        wxString string;
        string << MyUtilFunctions::IntToWxstr(startHour,2) << ":" << MyUtilFunctions::IntToWxstr(startMinute,2) << " | ";
        string << MyUtilFunctions::IntToWxstr(finishHour,2) << ":" << MyUtilFunctions::IntToWxstr(finishMinute,2) << " | ";
        string << wxDateTime::GetWeekDayName(static_cast<wxDateTime::WeekDay>(day));
        return string;
    }
};

#endif // SCHEDULEEXCEPTION_H
