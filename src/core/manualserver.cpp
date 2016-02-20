// This file is part of the SpeedCrunch project
// Copyright (C) 2016 Pol Welter <polwelter@gmail.com>
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


#include "core/manualserver.h"
#include "core/settings.h"

#include <QFile>
#include <QtHelp/QHelpEngineCore>
#include <QString>

#define DEPLOY_LANG(lang)     QFile::copy(":/manual/manual-lang.qch", dest + "manual-lang.qhc");\
                              QFile::copy(":/manual/manual-lang.qch", dest + "manual-lang.qch")



ManualServer* ManualServer::s_instance = NULL;

QString ManualServer::deployDocs()
{
    Q_INIT_RESOURCE(manual);

    QString dest = Settings::getDataPath() + "/manual/";

    DEPLOY_LANG(en);
    return dest;
}

void ManualServer::setupHelpEngine()
{
    QString collectionFile = deployDocs() + "manual-en.qhc";

    m_helpEngine = new QHelpEngineCore(collectionFile, this);
    if (!m_helpEngine->setupData()) {
        delete m_helpEngine;
        m_helpEngine = 0;
    }
    QStringList filters = m_helpEngine->customFilters();
    m_helpEngine->setCurrentFilter(filters.first());
}

ManualServer *ManualServer::instance()
{
    if(!s_instance)
        s_instance = new ManualServer();
    return s_instance;
}

void ManualServer::update() {}

bool ManualServer::URLforKeyword(const QString &id, QUrl &result) const
{
    result = "";
    if(!m_helpEngine)
        return 0;
    QMap<QString, QUrl> l;
    l = m_helpEngine->linksForIdentifier(id);
    if(l.isEmpty())
        return 0;
    result = l.first();
    return 1;
}

QByteArray ManualServer::fileData(const QUrl &url) const
{
    return m_helpEngine->fileData(url);
}

ManualServer::ManualServer()
{
    setupHelpEngine();
}

