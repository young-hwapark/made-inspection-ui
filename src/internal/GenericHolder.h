#ifndef GENERICHOLDER_H
#define GENERICHOLDER_H

#include <QObject>
#include <QMutex>
#include <QDebug>

class QMutex;

class HolderEmitter : public QObject
{
    Q_OBJECT

public:
    HolderEmitter() : QObject(nullptr) {}
    void forwardGetSignal() { emit getSignal(); }
    void forwardSetSignal() { emit setSignal(); }

signals:
    void getSignal();
    void setSignal();
};

template <typename T>
class GenericHolder
{
public:
    GenericHolder() {
        mutex = new QMutex(QMutex::NonRecursive);
    }
    GenericHolder(const T& rhs) : value(rhs) {
        mutex = new QMutex(QMutex::NonRecursive);
    }

    void connectGet(QObject * receiver, const char * slot, Qt::ConnectionType connectionType = Qt::AutoConnection)
    {
        QObject::connect(&emitter, SIGNAL(getSignal()), receiver, slot, connectionType);
    }

    void connectSet(QObject * receiver, const char * slot, Qt::ConnectionType connectionType = Qt::AutoConnection)
    {
        QObject::connect(&emitter, SIGNAL(setSignal()), receiver, slot, connectionType);
    }

    void disconnectGet(QObject * receiver, const char * slot)
    {
        QObject::disconnect(&emitter, SIGNAL(getSignal()), receiver, slot);
    }

    void disconnectSet(QObject * receiver, const char * slot)
    {
        QObject::disconnect(&emitter, SIGNAL(setSignal()), receiver, slot);
    }

    const T get() {
        mutex->lock();
        T copy = value;
        mutex->unlock();
        emitter.forwardGetSignal();
        return copy;
    }

    void set(T rhs) {
        mutex->lock();
        value = rhs;
        mutex->unlock();
        emitter.forwardSetSignal();
    }

private:
    HolderEmitter emitter;
    QMutex* mutex;
    T value;
};

#endif // GENERICHOLDER_H
