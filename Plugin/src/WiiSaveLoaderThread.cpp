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
    try
    {
        Athena::io::WiiSaveReader reader(m_path.toStdString());
        ret = reader.readSave();
        emit finished(ret);
    }
    catch(const Athena::error::Exception& e)
    {
        delete ret;
        emit error(QString::fromStdString(e.message()));
    }
}
