#include "mathchannelcomponent.h"
#include <QDebug>

UIMathChannelComponent::~UIMathChannelComponent()
{
    //  Deletion of object is called from MainWindow
    //  Here we only need to delete the layout
    while (QLayoutItem* item = layout->takeAt(0))
    {
        if (QWidget* widget = item->widget())
            widget->deleteLater();
        delete item;
    }
    layout->deleteLater();
}

UIMathChannelComponent::UIMathChannelComponent(uint8_t id): _id(id)
{
    labels[0] = new QLabel("Input channel");
    labels[1] = new QLabel("Math channel");
    labels[2] = new QLabel("Math");

    mathChSelector = new QComboBox();
    inChSelector = new QSpinBox();

    mathSelector = new QComboBox();
    mathSelector->addItem("Add");
    mathSelector->addItem("Subtract");

    deleteButton = new QPushButton();
    deleteButton->setText("X");
    deleteButton->setFixedWidth(25);

    layout = new QHBoxLayout();
    layout->setObjectName("mathChannel_"+QString::number(_id));

    horSpacer = new QSpacerItem (20,20,QSizePolicy::Expanding);

    //  Construct layout with all the elements
    layout->addWidget(labels[0], 0, Qt::AlignLeft);
    layout->addWidget(inChSelector, 0, Qt::AlignLeft);
    layout->addWidget(labels[1], 0, Qt::AlignLeft);
    layout->addWidget(mathChSelector, 0, Qt::AlignLeft);
    layout->addWidget(labels[2], 0, Qt::AlignLeft);
    layout->addWidget(mathSelector, 0, Qt::AlignLeft);
    layout->addSpacerItem(horSpacer);
    layout->addWidget(deleteButton, 0, Qt::AlignLeft);

    for (int i = 0; i < 6; i++)
        mathChSelector->addItem(QString::number(i+1));

    QObject::connect(deleteButton, &QPushButton::pressed, this, &UIMathChannelComponent::deleteComponent);
}

void UIMathChannelComponent::UpdateMathCh(int *mathCh, int size)
{
    int oldCh = mathChSelector->currentText().toInt(),
        index = -1;
    mathChSelector->clear();

    for (int i = 0; i < size; i++)
    {
        mathChSelector->addItem(QString::number(mathCh[i]));
        //  Attempt to preserve selected channel, if it
        //  exists in the new list save it's index and
        //  activate it afterwards
        if (mathCh[i] == oldCh)
            index = i;
    }
    mathChSelector->setCurrentIndex(index);
}
void UIMathChannelComponent::SetInCh(int inCh)
{
    inChSelector->setValue(inCh);
}

int UIMathChannelComponent::GetInCh()
{
    return inChSelector->value();
}

void UIMathChannelComponent::SetMathCh(int mathCh)
{
    bool valid = false;
    int i;

    for (i = 0; i < mathChSelector->count(); i++)
        if (mathChSelector->itemText(i).toInt() == mathCh)
        {
            valid = true;
            break;
        }

    if (valid)
        mathChSelector->setCurrentIndex(i);
}

int UIMathChannelComponent::GetMathCh()
{
    return mathChSelector->currentText().toInt();
}

void UIMathChannelComponent::SetMath(int math)
{
    //  0 - Add
    //  1 - Subtract
    mathSelector->setCurrentIndex(math);
}

int UIMathChannelComponent::GetMath()
{
    //  0 - Add
    //  1 - Subtract
    return mathSelector->currentIndex();
}

void UIMathChannelComponent::deleteComponent()
{
    emit deleteRequested(_id);
}

QHBoxLayout* UIMathChannelComponent::GetLayout()
{
    return layout;
}

void UIMathChannelComponent::SetID(uint8_t id)
{
    _id = id;
}
uint8_t  UIMathChannelComponent::GetID()
{
    return _id;
}
