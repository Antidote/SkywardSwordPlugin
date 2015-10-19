#include "WiiSaveLoaderThread.hpp"
#include <Athena/WiiSaveReader.hpp>
#include <Athena/WiiSave.hpp>

WiiSaveLoaderThread::WiiSaveLoaderThread(const QString& path, QObject* parent)
    : QObject(parent),
      m_path(path)
{
}

void WiiSaveLoaderThread::process()
{
    if (m_path.isEmpty())
    {
        emit error("Invalid path specified");
        return;
    }

    Athena::WiiSave* ret = nullptr;
    Athena::io::WiiSaveReader reader(m_path.toStdString());
    ret = reader.readSave();
    if (ret)
        emit finished(ret);
    else
        emit error("Error loading save, check application log for details");
}
