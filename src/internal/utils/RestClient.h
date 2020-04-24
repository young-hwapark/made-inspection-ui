#ifndef RESTCLIENT_H
#define RESTCLIENT_H

#include <QObject>
#include <QUrlQuery>


class RestClient
{
public:
    RestClient();

    std::pair<int, QJsonDocument> reqPost(QString, QByteArray, QString = "", QString = "");
    std::pair<int, QJsonDocument> reqGet(QString, QUrlQuery, QString = "", QString = "");

private:
    QString scheme;
    QString host;
    QString path;
};

#endif // RESTCLIENT_H
