#ifndef WIISAVELOADERTHREAD_HPP
#define WIISAVELOADERTHREAD_HPP

#include <QObject>

namespace Athena
{
class WiiSave;
}

class WiiSaveLoaderThread : public QObject
{
    Q_OBJECT
public:
    explicit WiiSaveLoaderThread(const QString& path, QObject* parent = 0);

public slots:
    void process();
signals:
    void error(QString);
    void finished(Athena::WiiSave* save);
private:
    QString m_path;
};

#endif // WIISAVELOADERTHREAD_HPP
