#ifndef ABSTRACTTRANSPORTER_H
#define ABSTRACTTRANSPORTER_H

#include <QObject>
#include <functional>
#include "connection.h"
#include "command.h"
#include "response.h"

namespace RedisClient {

class Response;

class AbstractTransporter : public QObject
{
    Q_OBJECT
public:
    AbstractTransporter(Connection * c);
    virtual ~AbstractTransporter();

signals:
    void errorOccurred(const QString&);
    void logEvent(const QString&);
    void connected();
    void commandAdded();

public slots:
    virtual void init() = 0;
    virtual void disconnect() = 0;
    virtual void addCommand(const Command&);
    virtual void cancelCommands(QObject *);

protected:
    bool m_isInitialized;
    bool m_isCommandRunning;
    Connection * m_connection;
    Command runningCommand;
    Response m_response;
    QByteArray readingBuffer;
    QQueue<Command> commands;
    QSharedPointer<QTimer> executionTimer;

    virtual void runCommand(const Command &cmd) = 0;
    virtual void sendResponse();
    virtual void processCommandQueue();
    void sendProgressValue();

protected slots:
    void executionTimeout();
};

class ResponseEmitter : public QObject {
    Q_OBJECT
public:
    ResponseEmitter(const Response& r)
        : m_response(r){}

    void sendResponse(QObject* owner, std::function<void(Response)> callback)
    {
        QObject::connect(this, &ResponseEmitter::response, owner, callback, Qt::AutoConnection);

        emit response(m_response);
    }

signals:
    void response(Response);
private:
    const Response& m_response;
};

}
#endif // ABSTRACTTRANSPORTER_H