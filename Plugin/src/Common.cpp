// This file is part of Sakura Suite.
//
// Sakura Suite is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Sakura Suite is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sakura Suite.  If not, see <http://www.gnu.org/licenses/>

#include "Common.hpp"
#include <ctime>
#include <QWidget>
#include <QSettings>
#include "SkywardSwordPlugin.hpp"

int convertRGB5A3ToBitMap(quint8* tplbuf, quint8** bitmapdata, quint32 width, quint32 height)
{
    quint32 x, y;
    quint32 x1, y1;
    quint32 iv;
    *bitmapdata = (quint8*)calloc(width * height, 4);
    if(*bitmapdata == NULL)
        return -1;
    quint32 outsz = width * height * 4;
    for(iv = 0, y1 = 0; y1 < height; y1 += 4) {
        for(x1 = 0; x1 < width; x1 += 4) {
            for(y = y1; y < (y1 + 4); y++) {
                for(x = x1; x < (x1 + 4); x++) {
                    quint16 oldpixel = *(quint16*)(tplbuf + ((iv++) * 2));
                    if((x >= width) || (y >= height))
                        continue;
                    oldpixel = qFromBigEndian(oldpixel);
                    if(oldpixel & (1 << 15)) {
                        // RGB5
                        quint8 b = (((oldpixel >> 10) & 0x1F) * 255) / 31;
                        quint8 g = (((oldpixel >> 5)  & 0x1F) * 255) / 31;
                        quint8 r = (((oldpixel >> 0)  & 0x1F) * 255) / 31;
                        quint8 a = 255;
                        quint32 rgba = (r << 0) | (g << 8) | (b << 16) | (a << 24);
                        (*(quint32**)bitmapdata)[x + (y * width)] = rgba;
                    }else{
                        // RGB4A3

                        quint8 a = (((oldpixel >> 12) & 0x7) * 255) / 7;
                        quint8 b = (((oldpixel >> 8)  & 0xF) * 255) / 15;
                        quint8 g = (((oldpixel >> 4)  & 0xF) * 255) / 15;
                        quint8 r = (((oldpixel >> 0)  & 0xF) * 255) / 15;
                        quint32 rgba = (r << 0) | (g << 8) | (b << 16) | (a << 24);
                        (*(quint32**)bitmapdata)[x + (y * width)] = rgba;
                    }
                }
            }
        }
    }
    return outsz;
}

QImage convertTextureToImage( const QByteArray &ba, quint32 w, quint32 h )
{
    //qDebug() << "SaveBanner::ConvertTextureToImage" << ba.size() << hex << w << h;
    quint8* bitmapdata = NULL;//this will hold the converted image
    int ret = convertRGB5A3ToBitMap( (quint8*)ba.constData(), &bitmapdata, w, h );
    if( !ret )
    {
        qWarning() << "SaveBanner::ConvertTextureToImage -> error converting image";
        return QImage();
    }
    QImage im = QImage( (const uchar*)bitmapdata, w, h, QImage::Format_ARGB32 );
    QImage im2 = im.copy( im.rect() );//make a copy of the image so the "free" wont delete any data we still want
    free( bitmapdata );
    return im2;
}

quint64 toWiiTime(QDateTime time)
{
    time_t sysTime, tzDiff;
    struct tm * gmTime;

    sysTime = time.toTime_t();
    // Account for DST where needed
    gmTime = localtime(&sysTime);
    if (!gmTime)
        return 0;

    // Lazy way to get local time in sec
    gmTime	= gmtime(&sysTime);
    tzDiff = sysTime - mktime(gmTime);

    return (quint64)(TICKS_PER_SECOND * ((sysTime + tzDiff) - SECONDS_TO_2000));
}

QDateTime fromWiiTime(quint64 wiiTime)
{
    QDateTime tmp(QDate(2000, 1, 1));
    tmp = tmp.addSecs(wiiTime / TICKS_PER_SECOND);
    return tmp;
}


void saveWidgetGeom(QWidget* target, QString key)
{
    QSettings settings;
    settings.beginGroup(SkywardSwordPlugin::instance()->name());
    settings.setValue(key, target->saveGeometry());
    settings.endGroup();
}


void restoreWidgetGeom(QWidget* target, QString key)
{
    QSettings settings;
    settings.beginGroup(SkywardSwordPlugin::instance()->name());
    target->restoreGeometry(settings.value(key).toByteArray());
    settings.endGroup();
}
