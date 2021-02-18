/***************************************************************************
**                                                                        **
**  This file is part of LabAnlyser.                                      **
**                                                                        **
**  LabAnlyser is free software: you can redistribute it and/or modify ´  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
****************************************************************************/

#pragma once
#include <QObject>
#include <QFile>
#include <QXmlStreamReader>
#include "../DataManagement/DataManagementClass.h"
#include "../DataManagement/DataMessengerClass.h"
#include <QDir>
class xmlexperimentwriter: public QObject
{
   Q_OBJECT
public:
    xmlexperimentwriter(QObject *parent = nullptr, MessengerClass* Messenger_ = nullptr );
    bool write(QIODevice *device, QDir T);
private:
    MessengerClass* Messenger = nullptr;
    DataManagementClass* Manager = nullptr;
    QXmlStreamWriter xmlWriter;
    QDir T;


    void writeExperiment();
    void writeTabs();
    void writeDevices();
    void writeFigureWindows();
    void writeWidgets();
    void writeConnections();
    void writeState();
};
