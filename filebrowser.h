#ifndef FILEBROWSER
#define FILEBROWSER

#include <simplegui.h>
#include <dirent.h>
#include <cstdio>

class FileBrowser;

FileBrowser* p = NULL;

class FileBrowser :public Frame {
private:
    Button* back;
    Label*  path;
    List*   folders;
    List*   files;
public:
    FileBrowser() : Frame(100,100,400,200,"Seleccione un Archivo") {
        launchWidgets();
        loadFolders();
        //loadFiles();
        launchEvents();
        run();
    }

    void launchWidgets() {
        back    = new Button(10,13,13,14,"<");
        path    = new Label(25,10,365,20,"/home/chalo");
        folders = new List(10,40,185,145);
        files   = new List(205,40,185,145);
        add(back);
        add(path);
        add(folders);
        add(files);
    }

    void loadFolders() {
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir (path->getText())) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
                if (ent->d_name[0]=='.') continue;
                if (ent->d_type == 4) {
                    folders->add(ent->d_name);
                }
            }
            closedir (dir);
        }
    }

    void loadFiles(const char path[]) {
        files->removeAll();
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir (path)) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
                if (ent->d_name[0]=='.') continue;
                if (ent->d_type != 4) {
                    files->add(ent->d_name);
                }
            }
            closedir (dir);
        }
    }

    void launchEvents() {
        p = this;
        folders->selectionChanged = &load;
    }

    static void load(const char item[]) {
        char fullpath[256];
        sprintf(fullpath,"%s/%s",p->path->getText(),item);
        p->loadFiles(fullpath);
    }

};


#endif // FILEBROWSER

