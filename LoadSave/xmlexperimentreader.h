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
#include "../DataManagement/DataMessengerClass.h"

class XmlExperimentReader: public QObject
{
   Q_OBJECT
public:
    XmlExperimentReader(QObject *parent = nullptr, MessengerClass* Messenger_ = nullptr );
    bool read(QIODevice *device);
    QString errorString() { return QString();};
private:
    QXmlStreamReader reader;
    void readExperiment();
    void readTab();
    void LoadForm(QString Name);
    void readDevices();
    void LoadDevice();
    void readWidgets();
    void readWidget();
    void ReadState();
    void CreateFigureWindows();
    void CreateFigureWindow();
    void CreateConnections();
    void CreateConnection();

    MessengerClass* Messenger = nullptr;

signals:
    void NewDeviceRegistration(QObject *NewDevice);
    void LoadFormFromXML(QString UiFileName, QString LastFormName, bool skip);

};
