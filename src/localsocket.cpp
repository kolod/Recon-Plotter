//    Recon Plotter
//    Copyright (C) 2021  Oleksandr Kolodkin <alexandr.kolodkin@gmail.com>
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "localsocket.h"

QString localServerFullPath() {
#ifdef WIN32
    return "\\\\.\\pipe\\" + str2key(qApp->applicationName());
#else
    return QDir::tempPath() + "/" + str2key(qApp->applicationName());
#endif
}

bool trySendFilesPreviouslyOpenedApplication(const QStringList files) {
    auto fullpath = localServerFullPath();
    QLocalSocket localSocket;
    localSocket.connectToServer(fullpath);
    if (localSocket.waitForConnected(1000)) {
        foreach (auto filename, files) {
            qDebug() << "Send filename:" << filename;
            localSocket.write(QString("%1\n").arg(filename).toUtf8());
            localSocket.flush();
        }
        localSocket.close();
        return true;
    }

    // Remove local server pipe if not connected
    QFile f(fullpath);
    if (f.exists()) f.remove();

    return false;
}
