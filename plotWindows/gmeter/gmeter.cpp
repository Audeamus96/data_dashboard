#include "gmeter.h"
#include "helperObjects/dataMultiplexer/datamultiplexer.h"
#include <mainwindow.h>

uint32_t defaultTailSize = 25;

Gmeter::Gmeter(QString objName): _nInputs(2), _maxInChannel(0), _tailSize(defaultTailSize)
{

    //  Create container window
    _contWind = new QWidget();
    this->setObjectName(objName);
    windMainLayout = new QVBoxLayout(_contWind);
    this->setWidget(_contWind);

    _refresher = new QTimer(this);

    _ConstructUI();

//    _gmeterDrawingWidget = new GmeterWidget(this);
//    _gmeterDrawingWidget->setMinimumSize(QSize(200,200));
//    this->setObjectName(objName);
//    windMainLayout = new QVBoxLayout(_gmeterDrawingWidget);
//    this->setWidget(_gmeterDrawingWidget);

//    _refresher = new QTimer(this);

//    static qreal x = 0;
//    static qreal y = 0;

//    connect(_refresher, &QTimer::timeout, _gmeterDrawingWidget,[this]() {
//        // Update the angle for the moving dot
//        // This is a simple example; you might want to base this on actual data
//        _gmeterDrawingWidget->setPosition(x, y); // Increment the angle and update the widget
//        x+=0.1;
//        y+=0.1;
//    });

//    _refresher->start(500);

}

Gmeter::~Gmeter(){
    emit logLine("Gmeter: Destroying the plot");

}

void Gmeter::_ConstructUI(){

    QVector<uint8_t> selectedChannels;

    //  Basic header with input channel drop-downs
    _header = new graphHeaderWidget(_nInputs, this->objectName());
    windMainLayout->addLayout(_header->GetLayout());

    //  Reselect the channels
    _header->SetSelectedChannels(selectedChannels);

    //  Handle dynamic channel selection by drop-down
    QObject::connect(_header, &graphHeaderWidget::UpdateInputChannels,
                     this, &Gmeter::UpdateInputChannels);

    //  Make sure input channel drop-downs have updated list of channels
    QObject::connect(DataMultiplexer::GetP(),
                     &DataMultiplexer::ChannelsUpdated,
                     _header,
                     &graphHeaderWidget::UpdateChannelDropdown);

    //  Line to separate channels from config
    QFrame *_vertLine = new QFrame();
    _vertLine->setFrameShape(QFrame::VLine);
    _vertLine->setFrameShadow(QFrame::Sunken);
    _header->GetLayout()->addWidget(_vertLine);

    //  Extra parts of header, specific to Line plot
    QVBoxLayout *gmeterSpecificHeader = new QVBoxLayout();
    _header->GetLayout()->addLayout(gmeterSpecificHeader);
    _header->AppendHorSpacer();

    //  Textbox to update the tail size
    QLineEdit *tailSize = new QLineEdit();
    tailSize->setValidator( new QIntValidator(1, 5000, this) );
    tailSize->setToolTip(_STYLE_TOOLTIP_("Change the length of X axis: Number"
                                          " of past samples used to plot the curve with. Limited to 5000 samples"));
    tailSize->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    tailSize->setText( QString::number(_tailSize) );
    connect(tailSize, &QLineEdit::textChanged, this, &Gmeter::UpdateTailSize); // todo
    gmeterSpecificHeader->addWidget(new QLabel("Tail size"));
    gmeterSpecificHeader->addWidget(tailSize);

    gmeterSpecificHeader->addSpacerItem(new QSpacerItem (20,20,QSizePolicy::Expanding));

    // Create Gmeter
    _gmeterWidget = new GmeterWidget(this);
    _gmeterWidget->setMinimumSize(QSize(200,200));

    windMainLayout ->addWidget(_gmeterWidget);

    _refresher->start(20);

    // Register with the mux
    DataMultiplexer::GetI().RegisterGraph(this->objectName(), _nInputs, this);

}

void Gmeter::UpdateTailSize(const QString &_dataSize)
{
    emit logLine("Line: Updating tail size to: "+_dataSize);
    _tailSize = _dataSize.toUInt();

    _gmeterWidget->setTailSize(_tailSize);
}

/**
 * @brief [Slot] Function called whenever input channel has been
 *      changed in the drop-down fields of the header. It updates the channels
 *      used as data sources for the gmeter.
 * @param inChannels    Array of \ref _nInputs input channel indexes
 */
void Gmeter::UpdateInputChannels(uint8_t *inChannels)
{
    emit logLine("Line: Input channels");

    if (_header->GetLabels().size() != _nInputs)
    {
        emit logLine("Line: Inconsistency in received and required number of channels.");
        return;
    }

    //  Reset the max index of input channels
    _maxInChannel = 0;

    for (uint8_t i = 0; i < _nInputs; i++)
    {
        _inputChannels[i] = inChannels[i];

        if (inChannels[i] > _maxInChannel)
            _maxInChannel = inChannels[i];
    }
}

/**
 * @brief Function directly called by the multiplexer to push data into
 *      the graph
 * @param data  Array of available data
 * @param n     Size of data array
 */
void Gmeter::ReceiveData(double *data, uint8_t n){

    // Check if the largest index of input channels is available in the
    // received block of data
    if (n < _maxInChannel || n < 2)
        return;

    qreal xCoord = data[_inputChannels[0]];
    qreal yCoord = data[_inputChannels[1]];

    _gmeterWidget->setPosition(xCoord, yCoord);
}


