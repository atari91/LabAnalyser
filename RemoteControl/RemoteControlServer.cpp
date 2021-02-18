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

#include "RemoteControlServer.h"

RemoteControlServer::RemoteControlServer(std::map<QString, ToFormMapper*> *DataContainerI)
{
    this->DataContainer = DataContainerI;

    connect(&tcpServer, SIGNAL(newConnection()),
           this, SLOT(acceptConnection()));

    tcpServer.listen(QHostAddress::LocalHost, 4080);

}

void RemoteControlServer::acceptConnection()
{
    //if(tcpServerConnection)
     //    tcpServerConnection->close();
    tcpServerConnection = tcpServer.nextPendingConnection();
    DataBuffer.clear();

   connect(tcpServerConnection, SIGNAL(readyRead()),
         this, SLOT(HeaderReceived()));
   connect(tcpServerConnection, SIGNAL(error(QAbstractSocket::SocketError)),
          this, SLOT(displayError(QAbstractSocket::SocketError)));

}

void RemoteControlServer::HeaderReceived()
{
    while(tcpServerConnection->bytesAvailable())
    {
        DataBuffer = DataBuffer.append(tcpServerConnection->readAll());
        uint32_t DataSize = *((uint32_t*)DataBuffer.data());
        if(DataBuffer.size() < DataSize)
        {
            return;
        }
        while(DataBuffer.size() >= DataSize)
        {
            if(DataBuffer.size() > DataSize)
            {
                NextDataBuffer = DataBuffer.mid(DataSize,DataBuffer.size()-DataSize);
                DataBuffer.remove(DataSize,DataBuffer.size()-DataSize);
            }

            QByteArray Data = DataBuffer;
            DataBuffer = NextDataBuffer;
            NextDataBuffer.clear();

            int DataPointer = 0;
            while(DataPointer < DataSize)
            {
                char* DataArrayStart = &((Data.data())[DataPointer]);
                QString Command = QString::fromLatin1(Data.mid(4, 3 ));
                uint32_t LengthID = *((uint32_t*)&DataArrayStart[4+3]);
                uint32_t LengthofData = *((uint32_t*)&DataArrayStart[3+8]);
                ReceivedID = QString::fromLatin1(Data.mid(15, LengthID ));

                if(Command.compare("set")==0)
                {
                    if(this->DataContainer)
                        {
                            auto it = this->DataContainer->find(ReceivedID);
                            if(it != this->DataContainer->end())
                            {
                                InterfaceData Data_ = *((*(this->DataContainer))[ReceivedID]);
                                if(Data_.IsNumeric())
                                    Data_.SetDataKeepType(*((double*) &DataArrayStart[15+LengthID]));
                                else if(Data_.IsString())
                                {
                                    int Size = DataSize-LengthID-15 ;
                                    QString TS = QString::fromLatin1(Data.mid(15+LengthID, Size));
                                    Data_.SetData(TS);
                                }
                                else if(Data_.IsGuiSelection())
                                {
                                    auto Sel =    Data_.GetGuiSelection();
                                    int Size = DataSize-LengthID-15 ;
                                    QString TS = QString::fromLatin1(Data.mid(15+LengthID, Size));
                                    Sel.first = TS;
                                    if(Sel.second.contains(TS))
                                        Data_.SetData(Sel);
                                }
                                emit MessageSender(Command, ReceivedID,Data_);
                            }
                        }
                }
                if(Command.compare("get")==0)
                {
                    QByteArray DataOut;
                    DataOut.resize(1);
                    DataOut[0] = 0;

                    if(this->DataContainer)
                    {
                        auto it = this->DataContainer->find(ReceivedID);
                        if(it != this->DataContainer->end())
                        {
                            InterfaceData Data_ = *((*(this->DataContainer))[ReceivedID]);
                            if(Data_.IsNumeric())
                            {
                                uint32_t Elements = 1;
                                DataOut.append((const char*)(&Elements),4);
                                double DataTmp = Data_.GetAsDouble();
                                DataOut.append((const char*)(&DataTmp),8);
                            }
                            else if(Data_.IsString())
                            {
                                DataOut[0] = 1;
                                uint32_t Elements = strlen(Data_.GetString().toStdString().c_str())+1;
                                DataOut.append((const char*)(&Elements),4);
                                std::vector<char> NDataOut;
                                NDataOut.resize(Elements*8);
                                for(int i = 0;i < NDataOut.size(); i++)
                                    NDataOut[i] = 0;
                                for(int i = 0;i < Elements; i++)
                                    NDataOut[i] = Data_.GetString().toStdString()[i];
                                DataOut.append((const char*)(&NDataOut[0]),NDataOut.size());
                            }
                            else if(Data_.IsGuiSelection())
                            {
                                DataOut[0] = 1;
                                uint32_t Elements = strlen(Data_.GetGuiSelection().first.toStdString().c_str())+1;
                                DataOut.append((const char*)(&Elements),4);
                                std::vector<char> NDataOut;
                                NDataOut.resize(Elements*8);
                                for(int i = 0;i < NDataOut.size(); i++)
                                    NDataOut[i] = 0;
                                for(int i = 0;i < Elements; i++)
                                    NDataOut[i] = Data_.GetGuiSelection().first.toStdString()[i];
                                DataOut.append((const char*)(&NDataOut[0]),NDataOut.size());

                            }
                            else if(Data_.IsPairOfVectorOfDoubles())
                            {
                                //Check for zero pointer
                                std::vector<double> Time;
                                std::vector<double> MeasuredDataOut;
                                auto TP = (Data_.GetPointerPair().first);
                                auto DP = (Data_.GetPointerPair().second);
                                if(TP)
                                {
                                    Time.insert(Time.end(), TP->begin(), TP->end());
                                    MeasuredDataOut.insert(MeasuredDataOut.end(), DP->begin(), DP->end());
                                }
                                else
                                {
                                    Time.push_back(0.0);
                                    MeasuredDataOut.push_back(0.0);
                                }
                                uint32_t Elements = Time.size() + MeasuredDataOut.size();
                                DataOut.append((const char*)(&Elements),4);
                                DataOut.append((const char*)(&Time[0]),8*Time.size());
                                DataOut.append((const char*)(&MeasuredDataOut[0]),8*MeasuredDataOut.size());
                            }
                             tcpServerConnection->write(DataOut);
                        }
                        else
                        {
                            uint32_t Elements = 0;
                            DataOut.append((const char*)(&Elements),4);
                            tcpServerConnection->write(DataOut);
                        }
                    }
                    else
                    {
                        uint32_t Elements = 0;
                        DataOut.append((const char*)(&Elements),4);
                        tcpServerConnection->write(DataOut);
                    }
                }
                DataPointer += 15+LengthID+LengthofData;
            }
            DataSize = *((uint32_t*)DataBuffer.data());

        }
    }
}

void RemoteControlServer::displayError(QAbstractSocket::SocketError socketError)
{
           return;
}
