#include <QString>

#ifndef SCPICOMMANDS_H
#define SCPICOMMANDS_H

const QString REMOTE_ON = "SYST:REM";
const QString REMOTE_OFF = "SYST:LOC";
const QString MODE_AC = "SYST:MODE AC";
const QString MODE_PF = "SYST:CFPF:MODE PF";
const QString SET_CURR = "CURR ";
const QString SET_PF = "PFAC ";
const QString GET_CURR = "MEAS:CURR:RMS?";
const QString GET_VOLT = "MEAS:VOLT:RMS?";
const QString GET_PF = "MEAS:POW:PFAC?";
const QString GET_POW = "MEAS:POW?";
const QString TURN_ON = "INP ON";
const QString TURN_OFF = "INP OFF";

#endif // SCPICOMMANDS_H
