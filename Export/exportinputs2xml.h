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
#include "../plugins/InterfaceDataType.h"
#include "../DataManagement/DataManagementSetClass.h"
#include <QXmlStreamReader>

class ExportInputs2Xml
{
public:

    ExportInputs2Xml(DataManagementSetClass* Manager_);
    bool Export2XML(QString Filename_,  QStringList Ids_);
    void WriteParameterSet();
    void WriteEntry();
    void WriteParameter();
    void WriteValue();


    QStringList Ids;

private:
    QString Filename;
    std::map<QString, InterfaceData> Data;
    DataManagementSetClass* Manager = nullptr;
    int ExportCounter = 0;

    QXmlStreamWriter xmlWriter;

};
