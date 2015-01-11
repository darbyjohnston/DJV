//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

//! \file djvViewImagePrefs.cpp

#include <djvViewImagePrefs.h>

#include <djvViewApplication.h>

#include <djvPrefs.h>

#include <djvImage.h>

//------------------------------------------------------------------------------
// djvViewImagePrefs
//------------------------------------------------------------------------------

djvViewImagePrefs::djvViewImagePrefs(QObject * parent) :
    djvViewAbstractPrefs(parent),
    _frameStoreFileReload(frameStoreFileReloadDefault()),
    _mirror              (mirrorDefault()),
    _scale               (scaleDefault()),
    _rotate              (rotateDefault()),
    _colorProfile        (colorProfileDefault()),
    _displayProfileIndex(displayProfileIndexDefault()),
    _channel             (channelDefault())
{
    djvPrefs prefs("djvViewImagePrefs");

    prefs.get("frameStoreFileReload", _frameStoreFileReload);
    prefs.get("mirror", _mirror);
    prefs.get("scale", _scale);
    prefs.get("rotate", _rotate);
    prefs.get("colorProfile", _colorProfile);

    djvPrefs displayProfilePrefs("djvViewImagePrefs.displayProfile");
    displayProfilePrefs.get("index", _displayProfileIndex);
    int displayProfilesCount = 0;
    displayProfilePrefs.get("size", displayProfilesCount);

    for (int i = 0; i < displayProfilesCount; ++i)
    {
        djvViewDisplayProfile value;
        displayProfilePrefs.get(QString("%1").arg(i), value);
        djvViewUtil::loadLut(value.lutFile, value.lut);
        _displayProfiles += value;
    }

    prefs.get("channel", _channel);
}

djvViewImagePrefs::~djvViewImagePrefs()
{
    djvPrefs prefs("djvViewImagePrefs");

    prefs.set("frameStoreFileReload", _frameStoreFileReload);
    prefs.set("mirror", _mirror);
    prefs.set("scale", _scale);
    prefs.set("rotate", _rotate);
    prefs.set("colorProfile", _colorProfile);

    djvPrefs displayProfilePrefs("djvViewImagePrefs.displayProfile");
    displayProfilePrefs.set("index", _displayProfileIndex);
    displayProfilePrefs.set("size", _displayProfiles.count());

    for (int i = 0; i < _displayProfiles.count(); ++i)
    {
        displayProfilePrefs.set(QString("%1").arg(i), _displayProfiles[i]);
    }

    prefs.set("channel", _channel);
}

bool djvViewImagePrefs::frameStoreFileReloadDefault()
{
    return false;
}

bool djvViewImagePrefs::hasFrameStoreFileReload() const
{
    return _frameStoreFileReload;
}

djvPixelDataInfo::Mirror djvViewImagePrefs::mirrorDefault()
{
    return djvPixelDataInfo::Mirror();
}

const djvPixelDataInfo::Mirror & djvViewImagePrefs::mirror() const
{
    return _mirror;
}

djvViewUtil::IMAGE_SCALE djvViewImagePrefs::scaleDefault()
{
    return static_cast<djvViewUtil::IMAGE_SCALE>(0);
}

djvViewUtil::IMAGE_SCALE djvViewImagePrefs::scale() const
{
    return _scale;
}

djvViewUtil::IMAGE_ROTATE djvViewImagePrefs::rotateDefault()
{
    return static_cast<djvViewUtil::IMAGE_ROTATE>(0);
}

djvViewUtil::IMAGE_ROTATE djvViewImagePrefs::rotate() const
{
    return _rotate;
}

bool djvViewImagePrefs::colorProfileDefault()
{
    return true;
}

bool djvViewImagePrefs::hasColorProfile() const
{
    return _colorProfile;
}

int djvViewImagePrefs::displayProfileIndexDefault()
{
    return -1;
}

int djvViewImagePrefs::displayProfileIndex() const
{
    return _displayProfileIndex;
}

djvViewDisplayProfile djvViewImagePrefs::displayProfile() const
{
    return
        (_displayProfileIndex >= 0 &&
            _displayProfileIndex < _displayProfiles.count()) ?
        _displayProfiles[_displayProfileIndex] :
        djvViewDisplayProfile();
}

const QVector<djvViewDisplayProfile> & djvViewImagePrefs::displayProfiles() const
{
    return _displayProfiles;
}

QStringList djvViewImagePrefs::displayProfileNames() const
{
    QStringList out;

    for (int i = 0; i < _displayProfiles.count(); ++i)
    {
        out += _displayProfiles[i].name;
    }

    return out;
}

djvOpenGlImageOptions::CHANNEL djvViewImagePrefs::channelDefault()
{
    return static_cast<djvOpenGlImageOptions::CHANNEL>(0);
}

djvOpenGlImageOptions::CHANNEL djvViewImagePrefs::channel() const
{
    return _channel;
}

djvViewImagePrefs * djvViewImagePrefs::global()
{
    static djvViewImagePrefs * prefs = 0;

    if (!prefs)
    {
        prefs = new djvViewImagePrefs(DJV_VIEW_APP);
    }

    return prefs;
}

void djvViewImagePrefs::setFrameStoreFileReload(bool in)
{
    _frameStoreFileReload = in;
}

void djvViewImagePrefs::setMirror(const djvPixelDataInfo::Mirror & mirror)
{
    if (mirror == _mirror)
        return;

    _mirror = mirror;

    Q_EMIT mirrorChanged(_mirror);
    Q_EMIT prefChanged();
}

void djvViewImagePrefs::setScale(djvViewUtil::IMAGE_SCALE in)
{
    if (in == _scale)
        return;

    _scale = in;

    Q_EMIT scaleChanged(_scale);
    Q_EMIT prefChanged();
}

void djvViewImagePrefs::setRotate(djvViewUtil::IMAGE_ROTATE in)
{
    if (in == _rotate)
        return;

    _rotate = in;

    Q_EMIT rotateChanged(_rotate);
    Q_EMIT prefChanged();
}

void djvViewImagePrefs::setColorProfile(bool in)
{
    if (in == _colorProfile)
        return;

    _colorProfile = in;

    Q_EMIT colorProfileChanged(_colorProfile);
    Q_EMIT prefChanged();
}

void djvViewImagePrefs::setDisplayProfileIndex(int index)
{
    if (index == _displayProfileIndex)
        return;

    //DJV_DEBUG("setDisplayProfileIndex");
    //DJV_DEBUG_PRINT("index = " << index);

    _displayProfileIndex = index;

    djvViewDisplayProfile tmp;

    Q_EMIT displayProfileChanged(displayProfile());
    Q_EMIT prefChanged();
}

void djvViewImagePrefs::setDisplayProfiles(
    const QVector<djvViewDisplayProfile> & in)
{
    if (in == _displayProfiles)
        return;

    //DJV_DEBUG("setDisplayProfiles");
    //DJV_DEBUG_PRINT("in = " << in);

    _displayProfiles = in;

    setDisplayProfileIndex(djvMath::min(
        _displayProfileIndex,
        _displayProfiles.count() - 1));

    Q_EMIT displayProfilesChanged(_displayProfiles);
    Q_EMIT prefChanged();
}

void djvViewImagePrefs::setChannel(djvOpenGlImageOptions::CHANNEL in)
{
    if (in == _channel)
        return;

    _channel = in;

    Q_EMIT channelChanged(_channel);
    Q_EMIT prefChanged();
}
