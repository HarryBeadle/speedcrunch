// This file is part of the SpeedCrunch project
// Copyright (C) 2014 Helder Correia <helder.pereira.correia@gmail.com>
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

#include "gui/manualwindow.h"

#include <QtCore/QEvent>
#include <QtHelp/QHelpEngineCore>
#include "core/settings.h"

ManualWindow::ManualWindow(QWidget* parent)
    : QTextBrowser(parent)
{
    this->resize(640, 480);

    QString collectionFile = Settings::getDataPath() + "/manual/SpeedCrunch.qhc";

    m_helpEngine = new QHelpEngineCore(collectionFile, this);
    if (!m_helpEngine->setupData()) {
        delete m_helpEngine;
        m_helpEngine = 0;
    }

    retranslateText();
    QStringList filters = m_helpEngine->customFilters();
    m_helpEngine->setCurrentFilter(filters.first());
    showHelpForKeyword("home");
}

void ManualWindow::showHelpForKeyword(const QString &id)
{
    if (m_helpEngine) {
        QMap<QString, QUrl> links = m_helpEngine->linksForIdentifier(id);
        if (links.count())
            setSource(links.constBegin().value());
    }
}

void ManualWindow::openPage(const QUrl& url)
{
    setSource(url);
}

void ManualWindow::retranslateText()
{
    setWindowTitle(tr("User Manual"));
}


void ManualWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
        retranslateText();
    else
        QTextBrowser::changeEvent(event);
}

void ManualWindow::closeEvent(QCloseEvent* event)
{
    emit windowClosed();
    QTextBrowser::closeEvent(event);
}

QVariant ManualWindow::loadResource(int type, const QUrl &name)
{
    QByteArray ba;
    if (type < 4 && m_helpEngine && name.scheme()=="qthelp") {
        QUrl url(name);
        if (name.isRelative())
            url = source().resolved(url);
        ba = m_helpEngine->fileData(url);
    }
    return ba;
}
