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
#include "core/settings.h"
#include "core/manualserver.h"

#include <QtCore/QEvent>
#include <QKeyEvent>
#include <QtHelp/QHelpEngineCore>


ManualWindow::ManualWindow(QWidget* parent)
    : QTextBrowser(parent)
{
    this->resize(640, 480);

    m_server = ManualServer::instance();
    retranslateText();
    showHelpForKeyword("home");
}

void ManualWindow::showHelpForKeyword(const QString &id)
{
    QUrl url;
    if(m_server->URLforKeyword(id, url))
        openPage(url);
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
    if (event->type() == QEvent::LanguageChange) {
        retranslateText();
        m_server->ensureCorrectLanguage();
        this->reload();
    }
    else
        QTextBrowser::changeEvent(event);
}

void ManualWindow::keyPressEvent(QKeyEvent *ev)
{
    if(ev->key() == Qt::Key_Escape) {
        ev->accept();
        this->close();
    }
    else
        QTextBrowser::keyPressEvent(ev);
}

void ManualWindow::closeEvent(QCloseEvent* event)
{
    emit windowClosed();
    QTextBrowser::closeEvent(event);
}

QVariant ManualWindow::loadResource(int type, const QUrl &name)
{
    QByteArray ba;
    if (type < 4 && name.scheme()=="qthelp") {
        QUrl url(name);
        if (name.isRelative())
            url = source().resolved(name);
        ba = m_server->fileData(url);
    }
    return ba;
}
