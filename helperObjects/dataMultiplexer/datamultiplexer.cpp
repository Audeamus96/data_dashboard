#include "datamultiplexer.h"
#include <QDebug>

DataMultiplexer& DataMultiplexer::GetI()
{
    static DataMultiplexer inst;
    return inst;
}

DataMultiplexer* DataMultiplexer::GetP()
{
    return &(GetI());
}

DataMultiplexer::DataMultiplexer(): _threadQuit(false)
{
    _SerialdataReady.release();
}

DataMultiplexer::~DataMultiplexer()
{
    //  Pass kill signal to the thread and wait for it to terminate
    _threadQuit = true;
    wait();
}

/**
 * @brief DataMultiplexer::SetSerialFrameFormat
 * @param start
 * @param delim
 * @param end
 */
void DataMultiplexer::SetSerialFrameFormat(const char &start, const char &delim, const char &end)
{
    _SerialframeFormat[0] = start;
    _SerialframeFormat[1] = delim;
    _SerialframeFormat[2] = end;

    logLine(QString("Updated frame format with ")+_SerialframeFormat[0]+_SerialframeFormat[1]+_SerialframeFormat[2]);
}

void DataMultiplexer::RegisterSerialCh(uint8_t n, QString *chName)
{
    _SerialLabels.clear();
    qDebug() << "Attempting to reconfigure";

    for (uint8_t i = 0; i < n; i++)
        _SerialLabels.push_back(chName[i]);
    qDebug() << "Attempting to reconfigure2";


    _channelCount[SignalSource::SerialSignal] = n;
    qDebug() << "Attempting to reconfigure3";

    _InternalChannelUpdate();

    emit ChannelsUpdated();

}

/**
 * @brief RegisterMathChannel
 * @param channelId Math channel id (1-indexed)
 * @param label
 * @param operations
 * @param serialChannels
 * @param n
 */
void DataMultiplexer::RegisterMathChannel(uint8_t channelId,
                                          QString label,
                                          uint8_t *operation,
                                          uint8_t *serialChannels,
                                          uint8_t n)
{
    _mChannel[channelId-1].Clear();

    for (uint8_t i = 0; i < n; i++)
    {
        _mChannel[channelId-1].SetLabel(label);
        _mChannel[channelId-1].AddComponent(static_cast<MathOperation>(operation[i]), serialChannels[i]);
    }
    _mChannel[channelId-1].Enabled = true;
    qDebug() << "Registered channel " << channelId << "with " << n << "components";
    _channelCount[SignalSource::MathSignal]++;
}

void DataMultiplexer::RegisterMathChannel(uint8_t channelId,
                                          MathChannel *mc)
{
    //  Clean up old variable
    _mChannel[channelId-1].Clear();
    //delete &(_mChannel[channelId-1]);

    //  Move new data in
    _mChannel[channelId-1] = *mc;
    //  Enable math channel
    _mChannel[channelId-1].Enabled = true;
    _channelCount[SignalSource::MathSignal]++;

    _InternalChannelUpdate();
    emit ChannelsUpdated();

}

/**
 * @brief DataMultiplexer::UnegisterMathChannel
 * @param channelId Math channel (1-indexed)
 */
void DataMultiplexer::UnegisterMathChannel(uint8_t channelId)
{
    _mChannel[channelId-1].Clear();
    _channelCount[SignalSource::MathSignal]--;

    _InternalChannelUpdate();
    emit ChannelsUpdated();
}

QStringList DataMultiplexer::GetChannelList()
{
    QStringList retVal;

    for (QString &X : _SerialLabels)
        retVal.append(X);

    for (MathChannel &X : _mChannel)
        if (X.Enabled)
            retVal.append(X.GetLabel());

   qDebug() << retVal;

    return retVal;
}

void DataMultiplexer::_InternalChannelUpdate()
{
    qDebug() << "Attempting to reconfigure0";
    if (isRunning())
    {
        _threadQuit = true;
        wait();
    }

    qDebug() << "Attempting to reconfigureA";

    if (_channelCount[SignalSource::AllChannels] != 0)
    {
        qDebug() << "Trying to delete";
        delete [] _channelData;
        qDebug() << "Done to delete";
    }

    qDebug() << "Attempting to reconfigureB";

    _channelCount[SignalSource::AllChannels] =
            _channelCount[SignalSource::SerialSignal] +
            _channelCount[SignalSource::MathSignal];

    qDebug() << "Attempting to reconfigureC";

    _channelData = new double[_channelCount[SignalSource::AllChannels]];

}

/**
 * @brief DataMultiplexer::RegisterGraph
 * @param name
 * @param numCh
 * @param receiver
 */
void DataMultiplexer::RegisterGraph(QString name,
                                uint8_t numCh,
                                OrientationWidget* receiver)
{
    _Graphs.push_back(GraphClient(name,numCh,receiver));
}

/**
 * @brief DataMultiplexer::UnregisterGraph
 * @param reciver
 */
void DataMultiplexer::UnregisterGraph(OrientationWidget* reciver)
{
    for (uint8_t i = 0; i < _Graphs.size(); i++)
    {
        if (_Graphs[i].Receiver() == reciver)
        {
            _Graphs.erase(_Graphs.begin()+i);
            break;
        }
    }
}

/**
 * @brief DataMultiplexer::ComputeMathChannels
 */
void DataMultiplexer::ComputeMathChannels()
{
    for (uint8_t i = 0; i < 6; i++)
    {
        //  Skip channel if it's not enabled
        if (!_mChannel[i].Enabled)
            continue;

        //  Math channels are offset by the count of serial channels
        _channelData[ _channelCount[SignalSource::SerialSignal] + i] = 0;

        for (uint8_t j = 0; j < _mChannel[i]._component.size(); j++)
            if (std::get<0>(_mChannel[i]._component[j]) == MathOperation::Add_Signal)
                _channelData[ _channelCount[SignalSource::SerialSignal] + i] += \
                        _channelData[ std::get<1>(_mChannel[i]._component[j]) ];
        else if (std::get<0>(_mChannel[i]._component[j]) == MathOperation::Subtract_Signal)
                _channelData[ _channelCount[SignalSource::SerialSignal] + i] -= \
                        _channelData[ std::get<1>(_mChannel[i]._component[j]) ];
    }
}

/**
 * @brief [Slot] Entry point for incoming serial data
 * @param buffer buffer of received data that's passed to the main thread
 * for parsing and dispatching
 */
void DataMultiplexer::ReceiveSerialData(const QString &buffer)
{
    _SerialdataReady.acquire(1);
    _buffer = buffer;

    if (!isRunning())
    {
        _threadQuit = false;
        start();
    }

    _SerialdataReady.release(2);

}

/**
 * @brief Multiplexer main thread
 * This thread is started upon constructing the multiplexer, and is spun up
 * to listen for incoming data, compute math channels and update data in the
 * registered graphs
 */
void DataMultiplexer::run()
{
    while (!_threadQuit)
    {
        if (!_SerialdataReady.tryAcquire(2,1))
            continue;

        QString buffer = _buffer;
        //  Sanity check, has this been properly initialized?
        if (_SerialLabels.size() == 0)
        {
            emit logLine(tr("Channel number discrepancy"));
            goto end_goto;
        }

        //  Loop through received data and split it in frames
        for (int32_t i = 0; i < buffer.length(); i++)
        {
            QString tmp("");
            //  Find start of the frame
            while (buffer[i].cell() != _SerialframeFormat[0])
                //  If we're about to overflow the buffer, terminate further
                //  processing and jump to the end of this loop to wait for
                //  new data frame
                if ((i+1) >= buffer.length())
                    goto end_goto;
                else
                    i++;

            if ((i+1) >= buffer.length())
                goto end_goto;
            else
                i++;

            //  Loop until the end char has been found
            while (buffer[i].cell() != _SerialframeFormat[2])
            {
                tmp += buffer[i];
                if ((i+1) >= buffer.length())
                    goto end_goto;
                else
                    i++;
            }

            QStringList chnValues = tmp.split( _SerialframeFormat[1] );

            //  Check for discrepancy in channel count
            if (chnValues.size() != _SerialLabels.size())
            {
                emit logLine(tr("Channel number discrepancy"));
                goto end_goto;
            }

            //  Go through the list of channels and move it into data buffer
            for (uint8_t j = 0; j < chnValues.size(); j++)
            {
                bool ok = false;

                double tmp = chnValues[j].toDouble(&ok);

                if (ok)
                    _channelData[j] = tmp;
                else
                {
                    _channelData[j] = 0.0;
                    emit logLine(tr("Error converting to double"));
                    continue;
                }
            }

            ComputeMathChannels();
    //        //  Update graphs
            for (GraphClient& X : _Graphs)
                X.SendData(_channelCount[SignalSource::AllChannels], _channelData);

            for (uint8_t i = 0; i < _channelCount[0]; i++)
                qDebug()<<_channelCount[0]<<":"<<_channelData[0]<<","<<_channelData[1]<<","<<_channelData[2]<<"," \
                        <<_channelData[3]<<","<<_channelData[4];
        }
end_goto:
        _SerialdataReady.release(1);

    }
}