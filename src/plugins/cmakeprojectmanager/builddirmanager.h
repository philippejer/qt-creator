/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#pragma once

#include "builddirreader.h"
#include "cmakeconfigitem.h"

#include <utils/fileutils.h>

#include <QFutureWatcher>
#include <QObject>
#include <QTemporaryDir>
#include <QTimer>

#include <functional>
#include <memory>

namespace CppTools { class ProjectPartBuilder; }

namespace ProjectExplorer {
class FileNode;
class IOutputParser;
class Kit;
class Task;
} // namespace ProjectExplorer

namespace CMakeProjectManager {

class CMakeTool;

namespace Internal {

class CMakeBuildConfiguration;

class BuildDirManager : public QObject
{
    Q_OBJECT

public:
    BuildDirManager(CMakeBuildConfiguration *bc);
    ~BuildDirManager() final;

    bool isParsing() const;

    void clearCache();
    void forceReparse();
    void maybeForceReparse(); // Only reparse if the configuration has changed...
    void resetData();
    bool updateCMakeStateBeforeBuild();
    bool persistCMakeState();

    void generateProjectTree(CMakeListsNode *root);
    QSet<Core::Id> updateCodeModel(CppTools::ProjectPartBuilder &ppBuilder);

    QList<CMakeBuildTarget> buildTargets() const;
    CMakeConfig parsedConfiguration() const;

signals:
    void configurationStarted() const;
    void dataAvailable() const;
    void errorOccured(const QString &err) const;

protected:
    static CMakeConfig parseConfiguration(const Utils::FileName &cacheFile,
                                          QString *errorMessage);

    const Utils::FileName workDirectory() const;

private:
    void emitDataAvailable();
    void checkConfiguration();

    void updateReaderType(std::function<void()> todo);
    void updateReaderData();

    void parseOnceReaderReady(bool force);
    void maybeForceReparseOnceReaderReady();

    void parse();

    void becameDirty();

    void asyncScanForFiles(QFutureInterface<QList<ProjectExplorer::FileNode *>> *fi);

    CMakeBuildConfiguration *m_buildConfiguration = nullptr;
    mutable std::unique_ptr<QTemporaryDir> m_tempDir = nullptr;
    mutable CMakeConfig m_cmakeCache;

    QTimer m_reparseTimer;

    std::unique_ptr<BuildDirReader> m_reader;
    QFutureWatcher<QList<ProjectExplorer::FileNode*>> m_futureWatcher;
    QFuture<QList<ProjectExplorer::FileNode*>> m_scanFuture;
};

} // namespace Internal
} // namespace CMakeProjectManager
