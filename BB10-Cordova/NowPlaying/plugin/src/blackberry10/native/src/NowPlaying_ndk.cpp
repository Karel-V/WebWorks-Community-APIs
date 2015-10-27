/*
 * Copyright (c) 2013 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <sstream>
#include <QtCore>
#include <QMetaObject>
#include <QMetaEnum>
#include <QObject>
#include <bb/platform/NotificationError>
#include <json/reader.h>
#include <json/value.h>
#include <json/writer.h>
#include "NowPlaying_ndk.hpp"
#include "NowPlaying_js.hpp"
#include "ApplicationThread.hpp"

#include <bb/multimedia/MediaPlayer>
#include <bb/multimedia/NowPlayingConnection>
#include <bb/multimedia/MetaData>

namespace webworks {

    using namespace std;
    using namespace bb::multimedia;

    void NowPlayingNDK::sendEvent(const string& msg) {
        m_pParent->NotifyEvent(msg);
    }

    string NowPlayingNDK::NowPlayingRequestPlayback(const string& data) {
        string returnValue = "";

        npc->acquire();

        Json::Value root;
        Json::Reader reader;

        bool parse = reader.parse(data, root);
        if (!parse) {
            returnValue = "Error requesting playback";
        } else {
            NowPlayingSetMusic(root["songURL"].asString());
            returnValue = "Playback requested successfully.";
        }

        return returnValue;
    }

    void NowPlayingNDK::NowPlayingBindPlayCallback(const string& callbackId) {
        playCallbackId = callbackId;

        bool connectResult;
        Q_UNUSED(connectResult);
        connectResult = this->connect(
                            this,
                            SIGNAL(playSignal()), // can't use bb::multimedia::NowPlayingConnection::play()?
                            this,
                            SLOT(playSlot())
                        );
        Q_ASSERT(connectResult);
    }

    void NowPlayingNDK::NowPlayingBindPauseCallback(const string& callbackId) {
        pauseCallbackId = callbackId;

        bool connectResult;
        Q_UNUSED(connectResult);
        connectResult = this->connect(
                            this,
                            SIGNAL(pauseSignal()), // can't use bb::multimedia::NowPlayingConnection::play()?
                            this,
                            SLOT(pauseSlot())
                        );
        Q_ASSERT(connectResult);
    }

    void NowPlayingNDK::NowPlayingBindStopCallback(const string& callbackId) {
        stopCallbackId = callbackId;

        bool connectResult;
        Q_UNUSED(connectResult);
        connectResult = this->connect(
                            this,
                            SIGNAL(stopSignal()), // can't use bb::multimedia::NowPlayingConnection::play()?
                            this,
                            SLOT(stopSlot())
                        );
        Q_ASSERT(connectResult);
    }

    void NowPlayingNDK::NowPlayingBindResumeCallback(const string& callbackId) {
        resumeCallbackId = callbackId;

        bool connectResult;
        Q_UNUSED(connectResult);
        connectResult = this->connect(
                            this,
                            SIGNAL(resumeSignal()), // can't use bb::multimedia::NowPlayingConnection::play()?
                            this,
                            SLOT(resumeSlot())
                        );
        Q_ASSERT(connectResult);
    }

    string NowPlayingNDK::NowPlayingSetMusic(const string& data) {
        QString str = QString::fromUtf8(data.c_str());

        if (! str.startsWith("http", Qt::CaseInsensitive)) {
            char cwd[PATH_MAX];
            getcwd(cwd, PATH_MAX);
            str.prepend(QString(cwd).append("/app/native/"));
        }

        QUrl url(str);

        if (url.isValid()){
          mp->setSourceUrl(url);
          return "Music set to " + data;
        } else {
          return "Music couldn't be set to " + data + " because it is invalid.";
        }
    }

//    string NowPlayingNDK::NowPlayingSetMetadata(const string& data) {
//        string returnValue = "";
//
//        Json::Value root;
//        Json::Reader reader;
//
//        bool parse = reader.parse(data, root);
//        if (!parse) {
//            returnValue = "Error setting metadata";
//        } else {
//            QVariantMap metadata;
//
//            metadata[MetaData::Title] = QString::fromStdString(root["Title"].asString());
//            metadata[MetaData::Artist] = QString::fromStdString(root["Artist"].asString());
//            metadata[MetaData::Album] = QString::fromStdString(root["Album"].asString());
//
//            if (npc->isAcquired()) {
//                npc->setMetaData(metadata);
//            } else {
//                npc->acquire();
//                npc->setMetaData(metadata);
//                npc->revoke();
//            }
//
//            returnValue = "Metadata set successfully.";
//        }
//
//        return returnValue;
//    }

//    string NowPlayingNDK::NowPlayingSetIcon(const string& data) {
//        QString str = QString::fromUtf8(data.c_str());
//
//        if (! str.startsWith("http", Qt::CaseInsensitive)) {
//            char cwd[PATH_MAX];
//            getcwd(cwd, PATH_MAX);
//            str.prepend(QString(cwd).append("/app/native/"));
//        }
//
//        QUrl url(str);
//
//        if (url.isValid()){
//            if (str.startsWith("http", Qt::CaseInsensitive)) {
//                return "Icon couldn't be set to " + data + " because HTTP URLs"
//                       + "to icons aren't currently supported.";
//            } else {
//                if (npc->isAcquired()) {
//                    npc->setIconUrl(url);
//                } else {
//                    npc->acquire();
//                    npc->setIconUrl(url);
//                    npc->revoke();
//                }
//                return "Icon set to " + data;
//            }
//        } else {
//            return "Icon couldn't be set to " + data + " because it is invalid.";
//        }
//    }

//    string NowPlayingNDK::NowPlayingChangeTrack(const string& callbackId, const string& data) {
//        return "Changed track.";
//    }
//
//    string NowPlayingNDK::NowPlayingEnableNextPrevious() {
//        if (npc->isAcquired()) {
//            npc->setNextEnabled(true);
//        } else {
//            npc->acquire();
//            npc->setNextEnabled(true);
//            npc->revoke();
//        }
//
//        return "Enabled Next Previous.";
//    }
//
//    string NowPlayingNDK::NowPlayingDisableNextPrevious() {
//        return "Disabled Next Previous.";
//    }

    string NowPlayingNDK::NowPlayingPlay() {
        emit playSignal();
        return "Player started.";
    }

    string NowPlayingNDK::NowPlayingPause() {
        emit pauseSignal();
        return "Player paused.";
    }

    string NowPlayingNDK::NowPlayingStop() {
        emit stopSignal();
        return "Player stopped.";
    }

    string NowPlayingNDK::NowPlayingResume() {
        emit resumeSignal();
        return "Player resumed.";
    }
//    string NowPlayingNDK::NowPlayingPause(const string& callbackId) {
//        emit pauseSNignal();
//        return "Player Paused.";
//    }
//
//    string NowPlayingNDK::NowPlayingResume(const string& callbackId) {
//        emit resumeSignal();
//        return "Player Resumed.";
//    }
//
//    string NowPlayingNDK::NowPlayingStop(const string& callbackId) {
//        emit stopSignal();
//        return "Player Stopped.";
//    }

    string NowPlayingNDK::NowPlayingGetState() {
        string state = "State: ";
        switch (mp->mediaState()) {
            case bb::multimedia::MediaState::Unprepared:
                state += "Unprepared";
                break;
            case bb::multimedia::MediaState::Prepared:
                state += "Prepared";
                break;
            case bb::multimedia::MediaState::Started:
                state += "Started";
                break;
            case bb::multimedia::MediaState::Paused:
                state += "Paused";
                break;
            case bb::multimedia::MediaState::Stopped:
                state += "Stopped";
                break;
            default:
                state += "Unknown";
        }

        state += " Acquired: ";
        state += npc->isAcquired() ? "True" : "False";

        state += " Preempted: ";
        state += npc->isPreempted() ? "True" : "False";

        return state.c_str();
    }

    /***
     * Slots
     ***/

    void NowPlayingNDK::playSlot() {
        mp->play();

        npc->setMediaState(mp->mediaState());

        // Callback
        Json::FastWriter writer;
        Json::Value root;
        root["result"] = "Playing! (callback).";
        sendEvent(playCallbackId + " " + writer.write(root));
    }

    void NowPlayingNDK::pauseSlot() {
        mp->pause();

        npc->setMediaState(mp->mediaState());

        // Callback
        Json::FastWriter writer;
        Json::Value root;
        root["result"] = "Pausing! (callback).";
        sendEvent(pauseCallbackId + " " + writer.write(root));
    }

    void NowPlayingNDK::stopSlot() {
        mp->stop();
        npc->setMediaState(mp->mediaState());

        // Callback
        Json::FastWriter writer;
        Json::Value root;
        root["result"] = "Stopping! (callback).";
        sendEvent(stopCallbackId + " " + writer.write(root));
    }

    void NowPlayingNDK::resumeSlot() {
        mp->play();
        npc->setMediaState(mp->mediaState());

        // Callback
        Json::FastWriter writer;
        Json::Value root;
        root["result"] = "Resuming! (callback).";
        sendEvent(resumeCallbackId + " " + writer.write(root));
    }

//    void NowPlayingNDK::resume() {
//        mp->play();
//
//        npc->setMediaState(mp->mediaState());
//    }
//
//    void NowPlayingNDK::pause() {
//        mp->pause();
//
//        npc->setMediaState(mp->mediaState());
//    }
//
//    void NowPlayingNDK::stop() {
//        npc->revoke();
//
//        mp->stop();
//
//        npc->setMediaState(mp->mediaState());
//    }
//
//    void NowPlayingNDK::next() {
//        npc->revoke();
//
//        mp->stop();
//
//        npc->setMediaState(mp->mediaState());
//    }

} /* namespace webworks */
