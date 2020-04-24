#include "RestClient.h"
#include <QCoreApplication>
#include <QApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonParseError>


RestClient::RestClient()
{
    QString env = qgetenv("NVISION_ENV");
    QString prefix = "";

    if (env == "DEV")
        prefix = "dev-";
    else if (env == "STAGING")
        prefix = "staging-";

    scheme = "https";
    host = prefix + "va-api-meta.neuronaware.net";
}

std::pair<int, QJsonDocument> RestClient::reqPost(QString api, QByteArray body, QString token, QString custID)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QNetworkAccessManager* manager = new QNetworkAccessManager;
    QEventLoop loop;
    QObject::connect(manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

    QUrl url;
    url.setScheme(scheme);
    url.setHost(host);
    url.setPath(api);

    QNetworkRequest req;
    req.setUrl(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("customerid", custID.toLocal8Bit());

    if (token != "") {
        auto header = QString("Bearer %1").arg(token);
        req.setRawHeader(QByteArray("authorization"), header.toUtf8());
    }

    QNetworkReply* reply = manager->post(req, body);
    loop.exec();
    QByteArray data = reply->readAll();

    int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QJsonParseError parsingResult;
    QJsonDocument json;
    json = QJsonDocument::fromJson(data, &parsingResult);

    reply->deleteLater();
    manager->deleteLater();

    QApplication::restoreOverrideCursor();
    return std::make_pair(status, json);
}

std::pair<int, QJsonDocument> RestClient::reqGet(QString api, QUrlQuery query, QString token, QString custID)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QNetworkAccessManager* manager = new QNetworkAccessManager;
    QEventLoop loop;
    QObject::connect(manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

    QUrl url;
    url.setScheme(scheme);
    url.setHost(host);
    url.setPath(api);
    url.setQuery(query);

    QNetworkRequest req;
    req.setUrl(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("customerid", custID.toLocal8Bit());

    if (token != "") {
        auto header = QString("Bearer %1").arg(token);
        req.setRawHeader(QByteArray("authorization"), header.toUtf8());
    }

    QNetworkReply* reply = manager->get(req);
    loop.exec();
    QByteArray data = reply->readAll();

    int status = -1;
    status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QJsonParseError parsingResult;
    QJsonDocument json;
    json = QJsonDocument::fromJson(data, &parsingResult);

    reply->deleteLater();
    manager->deleteLater();

    QApplication::restoreOverrideCursor();
    return std::make_pair(status, json);
}
