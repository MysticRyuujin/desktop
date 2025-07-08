// Copyright (c) 2025 Private Internet Access, Inc.
//
// This file is part of the Private Internet Access Desktop Client.
//
// The Private Internet Access Desktop Client is free software: you can
// redistribute it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// The Private Internet Access Desktop Client is distributed in the hope that
// it will be useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Private Internet Access Desktop Client.  If not, see
// <https://www.gnu.org/licenses/>.

#include "common.h"
#line HEADER_FILE("networktaskwithretry.h")

#ifndef NETWORKTASKWITHRETRY_H
#define NETWORKTASKWITHRETRY_H

#include "async.h"
#include "apibase.h"
#include "apiretry.h"
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <memory>

// NetworkTaskWithRetry executes an API request until either it succeeds or
// the maximum attempt count is reached.  It uses a NetworkReplyHandler for each
// attempt.
class COMMON_EXPORT NetworkTaskWithRetry : public Task<QByteArray>
{
    CLASS_LOGGING_CATEGORY("apiclient")

public:
    // Create NetworkTaskWithRetry with the verb and request that will be used
    // for each attempt.
    //
    // An ApiBase is passed to specify the base URIs to use for this request.
    // Each attempt uses the next base URI from the ApiBase.  (The ApiBase does
    // not need to outlive NetworkTaskWithRetry, we just use it initially to
    // create the ApiBaseSequence.)
    //
    // A retry strategy is passed to control the attempt count / duration /
    // delays for the request.  NetworkTaskWithRetry takes ownership of the
    // ApiRetry object.
    //
    // The attempts are spread across the base URIs (for example, with 2 base
    // URIs and 4 max attempts, each URI could be tried twice).
    //
    // If authHeaderVal is not empty, it is applied as an authorization header
    // to each request.
    NetworkTaskWithRetry(QNetworkAccessManager::Operation verb,
                         ApiBase &apiBaseUris, QString resource,
                         std::unique_ptr<ApiRetry> pRetryStrategy,
                         const QJsonDocument &data, QByteArray authHeaderVal);
    ~NetworkTaskWithRetry();

private:
    // Schedule an attempt, or reject if all attempts have been used.
    void scheduleNextAttempt(std::chrono::milliseconds nextDelay);

    // Execute an attempt (used by scheduleNextAttempt())
    void executeNextAttempt();

    // Create task to issue a single request and return its body.
    Async<QByteArray> sendRequest();

    // Trace a leaf certificate; used by checkSslErrorPeerName().
    void traceLeafCert(const QSslCertificate &leafCert) const;

    // Check the SSL certificate for a request using a custom CA and peer name.
    // If the certificate is accepted, calls reply.ignoreSslErrors().
    void checkSslCertificate(QNetworkReply &reply, const BaseUri &baseUri,
                             const QList<QSslError> &errors);

private:
    QNetworkAccessManager::Operation _verb;
    ApiBaseSequence _baseUriSequence;
    std::unique_ptr<ApiRetry> _pRetryStrategy;
    ApiResource _resource;
    QByteArray _data;
    QByteArray _authHeaderVal;
    Async<QByteArray> _pNetworkReply;
    // ApiRateLimitedError is retriable but causes us to return that instead of
    // the generic error if we don't encounter an auth error.
    // This field keeps track of the worst retriable error we have seen, if we
    // fail due to all attempts failing, this is the error we return.
    Error::Code _worstRetriableError;
};

#endif
