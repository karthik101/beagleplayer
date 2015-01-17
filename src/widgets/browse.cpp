/*
 *  Written By: Grant Hutchinson
 *  License: GPLv3.
 *  h.g.utchinson@gmail.com
 *  Copyright (C) 2012 by Grant Hutchinson
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "browse.h"
#include "ui_browse.h"

browse::browse(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::browse)
{
    ui->setupUi(this);
    MenuMode = 0;
    songCount = 0;
    vidCount = 0;
}

browse::~browse()
{
    delete ui;
}
/*
 *  Initialize/update/re-read from cache.
 */
void browse::Sync(int type){
    Artist.initFile(100); Song.initFile(100); VidDir.initFile(100); Video.initFile(100);

    if(type == 2 || type == 3){    /// import new entries into local db for each folder and file
        QDir usrDir = QString(getenv("HOME"));
        usrDir = QFileDialog::getExistingDirectory(this, tr("Import a directory"), QDir::currentPath());  /// get folder import directory
        if(type == 2){   /// import audio
           lclSync.Sync(usrDir, 0);
        }
        else if(type == 3){  /// import video
            lclSync.Sync(usrDir, 1);
        }
    }
    dbCon->readAll(Artist, Song, VidDir, Video);/// read from local database and sync to local objects
    updateMenu();
}
/*
 *  Update Left View List with Media file type, and their paths
 */
void browse::updateMenu(){

    QStringList curMenu;
    m_Model = new QStringListModel(this);
    if(MenuMode == 0 )  // artists + local
    {
        for(int i=0; i <= Artist.getSize(); i++){
            curMenu << Artist.getName(i);
        }
    }
    else if(MenuMode == 1 ) // vid Dirs + local
    {
        for(int i=0; i <= VidDir.getSize(); i++){
            curMenu << VidDir.getName(i);
        }
    }
    m_Model->setStringList(curMenu);
    ui->MenuList->setModel(m_Model);
}
/*
 *  Update Right View List with Media file tracks for a specific path
 */
void browse::updateTitle(int selected){
    int selID = 0;
    QStringList curSong;
    t_Model = new QStringListModel(this);
    songCount = 0;
    vidCount = 0;
        if(MenuMode == 0 ){    /// set Song + local
            selID = Artist.getID(selected);
            curSongID = new int[Song.getSize()];
            for(int i = 0; i<Song.getSize(); i++){
                if(Song.getPar(i) == selID){
                    curSong << Song.getName(i);
                    curSongID[songCount] = Song.getID(i);
                    songCount++;
                }
            }
           emit curListChanged(Song, curSongID);
        }
        else if(MenuMode == 1 ){    ///  set Video + local
            selID = VidDir.getID(selected);
            curVidID = new int[Video.getSize()];
            for(int i = 0; i<Video.getSize(); i++){
                if(Video.getPar(i) == selID){
                    curSong << Video.getName(i);
                    curVidID[vidCount] = Video.getID(i);
                    vidCount++;
                }
            }
            emit curListChanged(Video, curVidID);
        }

    t_Model->setStringList(curSong);
    ui->TrackList->setModel(t_Model);
}

void browse::on_MenuList_clicked(const QModelIndex &index)
{
    updateTitle(ui->MenuList->currentIndex().row());
}

void browse::on_TrackList_clicked(const QModelIndex &index)
{
    int selected = 0;
    selected = ui->TrackList->currentIndex().row();
    emit selectionChanged(selected);
    /// submit new clicked listing
    if(MenuMode == 0){             /// listing local songs
        for(int i=0; i<=Song.getSize(); i++){
            if(curSongID[selected] == Song.getID(i)){
                cout << Song.getName(i) << " " << Song.getPath(i) << " " << Song.getID(i) << " " << Song.getPar(i) << endl;
                emit plItemChanged(Song.getName(i), Song.getPath(i), Song.getID(i), Song.getPar(i));
            }
        }
    }
    else if(MenuMode == 1){         /// listing local videos
        for(int i =0; i<=vidCount; i++){
            if(curVidID[selected]== Video.getID(i)){
                emit plItemChanged(Video.getName(i), Video.getPath(i), Video.getID(i), Video.getPar(i));
            }
        }
    }  
 }

void browse::on_TrackList_doubleClicked(const QModelIndex &index)
{
    emit FullSelection(ui->TrackList->currentIndex().row());
}
