// This file is part of the SpeedCrunch project
// Copyright (C) 2014 Sébastien Szymanski <seb.szymanski@gmail.com>
// Copyright (C) 2014 @heldercorreia
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#include "bitfieldwidget.h"

#include "math/quantity.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListIterator>
#include <QPaintEvent>
#include <QApplication>
#include <QPushButton>

BitWidget::BitWidget(int bitPosition, QWidget* parent)
    : QLabel(parent),
    m_state(false)
{
    HNumber number(HMath::raise(HNumber(2), bitPosition));
    setToolTip(QString("2<sup>%1</sup> = %2")
        .arg(bitPosition)
        .arg(HMath::format(number, Quantity::Format::Decimal())));

    setText(QString("%1").arg(bitPosition));
    setObjectName("BitWidget");
}

void BitWidget::setState(bool state)
{
    if (state != m_state) {
        m_state = state;
        setStyleSheet(
            QString("QLabel { background-color : %1; color : %2; }")
                .arg(QApplication::palette().color(state ? QPalette::WindowText : QPalette::Window).name())
                .arg(QApplication::palette().color(state ? QPalette::Window : QPalette::WindowText).name())
        );
        update();
    }
}

void BitWidget::mouseReleaseEvent(QMouseEvent*)
{
    setState(!m_state);
    emit stateChanged(m_state);
}

BitFieldWidget::BitFieldWidget(QWidget* parent) :
    QWidget(parent)
{
    // Build the CSS border color using 50% opacity (same result as previous method with painting)
    const QColor& borderColor = QApplication::palette().color(QPalette::WindowText);
    QString cssBorderColor = QString("rgba(%1, %2, %3, %4)")
                                .arg(borderColor.red())
                                .arg(borderColor.green())
                                .arg(borderColor.blue())
                                .arg(0.5);

    setStyleSheet(QString("QLabel#BitWidget, QLabel#FirstBitWidget {"
                          " qproperty-alignment: 'AlignHCenter | AlignVCenter';"
                          " border-top: 1px solid %3;"
                          " border-bottom: 1px solid %3;"
                          " border-right: 1px solid %3;"
                          " padding: 1px;"
                          " background-color : %1; color : %2;"
                          "}"
                          /* Extra style for first bit of each group */
                          "QLabel#FirstBitWidget {"
                          " border-left: 1px solid %3;"
                          "}"
                          )
                      .arg(QApplication::palette().color(QPalette::Window).name())
                      .arg(QApplication::palette().color(QPalette::WindowText).name())
                      .arg(cssBorderColor)
                 );
    m_bitWidgets.reserve(NumberOfBits);
    for (int i = 0; i < NumberOfBits; ++i) {
        BitWidget* bitWidget = new BitWidget(i);
        connect(bitWidget, SIGNAL(stateChanged(bool)), this, SLOT(onBitChanged()));
        m_bitWidgets.append(bitWidget);
    }

    QGridLayout* fieldLayout = new QGridLayout;
    int bitOffset = 0;

    for (int column = 1; column < 17; ++column) {
        if ((column % 2) == 0) {
            if ((column % 4) != 0)
                continue;

            QLabel* topNumberLabel = new QLabel;
            QLabel* bottomNumberLabel = new QLabel;

            fieldLayout->addWidget(topNumberLabel, 0, column);
            fieldLayout->addWidget(bottomNumberLabel, 1, column);

        } else {
            QHBoxLayout* bottomLayout(new QHBoxLayout);
            QHBoxLayout* topLayout(new QHBoxLayout);

            // Disable items spacing so that it looks like a table
            bottomLayout->setSpacing(0);
            topLayout->setSpacing(0);

            for (int j = 0; j < 4; ++j) {
                const int topIndex = NumberOfBits - 1 - bitOffset * 4 - j;
                topLayout->addWidget(m_bitWidgets.at(topIndex));
                bottomLayout->addWidget(m_bitWidgets.at(topIndex - NumberOfBits / 2));
            }

            // Change the name of first bits so that the proper style is applied
            topLayout->itemAt(0)->widget()->setObjectName("FirstBitWidget");
            bottomLayout->itemAt(0)->widget()->setObjectName("FirstBitWidget");

            ++bitOffset;

            fieldLayout->addLayout(bottomLayout, 1, column, Qt::AlignCenter);
            fieldLayout->addLayout(topLayout, 0, column, Qt::AlignCenter);
        }
    }

    m_resetButton = new QPushButton("0");
    m_resetButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(m_resetButton, SIGNAL(clicked()), this, SLOT(resetBits()));

    m_invertButton = new QPushButton("~");
    m_invertButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(m_invertButton, SIGNAL(clicked()), this, SLOT(invertBits()));

    m_shiftLeftButton = new QPushButton("<<");
    m_shiftLeftButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(m_shiftLeftButton, SIGNAL(clicked()), this, SLOT(shiftBitsLeft()));

    m_shiftRightButton = new QPushButton(">>");
    m_shiftRightButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(m_shiftRightButton, SIGNAL(clicked()), this, SLOT(shiftBitsRight()));

    QGridLayout* buttonsLayout = new QGridLayout;
    buttonsLayout->addWidget(m_resetButton, 0, 0);
    buttonsLayout->addWidget(m_invertButton, 0, 1);
    buttonsLayout->addWidget(m_shiftLeftButton, 1, 0);
    buttonsLayout->addWidget(m_shiftRightButton, 1, 1);

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->addStretch();
    mainLayout->addLayout(fieldLayout);
    mainLayout->addLayout(buttonsLayout);
    mainLayout->addStretch();

    // The following needs to be done AFTER the widgets have been added to BitFieldWidget
    // as the style sheet will not be applied otherwise
    this->updateSize();
}

void BitFieldWidget::updateSize()
{
    // Compute bit widgets max size and apply it to all bit widgets
    QSize maxSize(0, 0);

    for (const auto &bitWidget : m_bitWidgets) {
        const QSize &widgetSize = bitWidget->sizeHint();
        if (maxSize.width() < widgetSize.width())
            maxSize.setWidth(widgetSize.width());
        if (maxSize.height() < widgetSize.height())
            maxSize.setHeight(widgetSize.height());
    }

    // Make the box be a square
    if (maxSize.width() < maxSize.height())
        maxSize.setWidth(maxSize.height());
    else
        maxSize.setHeight(maxSize.width());

    // Apply maxSize to all bit widgets
    for (auto &widget : m_bitWidgets)
        widget->setFixedSize(maxSize);

    // TODO: find some more justifiable size calculation.
    int buttonHeight = maxSize.height() * 4 / 3;
    int buttonWidth = buttonHeight * 2;

    m_resetButton->setFixedSize(buttonWidth, buttonHeight);
    m_invertButton->setFixedSize(buttonWidth, buttonHeight);
    m_shiftLeftButton->setFixedSize(buttonWidth, buttonHeight);
    m_shiftRightButton->setFixedSize(buttonWidth, buttonHeight);
}

void BitFieldWidget::wheelEvent(QWheelEvent* we)
{
    if (we->angleDelta().y() > 0)
        shiftBitsLeft();
    else
        shiftBitsRight();
}

void BitFieldWidget::updateBits(const Quantity& number)
{
    QString binaryNumberString = DMath::format(number, Quantity::Format::Fixed() + Quantity::Format::Binary());
    QListIterator<BitWidget*> bitsIterator(m_bitWidgets);

    if (number.isZero() || !number.isInteger())
        binaryNumberString.clear();
    else if (number.isNegative())
        binaryNumberString.remove(0, 3); // Remove '-0b'.
    else
        binaryNumberString.remove(0, 2); // Remove '0b'.

    auto iterator = binaryNumberString.end();
    while (bitsIterator.hasNext()) {
        if (iterator != binaryNumberString.begin()) {
            --iterator;
            bitsIterator.next()->setState(*iterator == '1');
        } else
            bitsIterator.next()->setState(false);
    }
}

void BitFieldWidget::onBitChanged()
{
    QListIterator<BitWidget*> bitsIterator(m_bitWidgets);
    QString expression;

    while (bitsIterator.hasNext())
        expression.prepend(bitsIterator.next()->state() ? "1" : "0");

    expression.remove(QRegExp(QString("^0{,%1}").arg(NumberOfBits - 1)));
    expression.prepend("0b");

    emit bitsChanged(expression);
}

void BitFieldWidget::invertBits()
{
    foreach (BitWidget* w, m_bitWidgets)
        w->setState(!w->state());

    onBitChanged();
}

void BitFieldWidget::clear()
{
    for (auto w : m_bitWidgets)
        w->setState(false);
}

void BitFieldWidget::resetBits()
{
    clear();
    onBitChanged();
}

void BitFieldWidget::shiftBitsLeft()
{
    auto it = m_bitWidgets.constEnd();
    auto itBegin = m_bitWidgets.constBegin();

    --it;
    while (it != itBegin) {
        (*it)->setState((*(it-1))->state());
        --it;
    }

    (*itBegin)->setState(false);
    onBitChanged();
}

void BitFieldWidget::shiftBitsRight()
{
    auto it = m_bitWidgets.constBegin();
    auto itEnd = m_bitWidgets.constEnd();

    --itEnd;
    while (it != itEnd) {
        (*it)->setState((*(it+1))->state());
        it++;
    }

    (*itEnd)->setState(false);
    onBitChanged();
}
