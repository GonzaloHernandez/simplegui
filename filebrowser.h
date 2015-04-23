#ifndef FILEBROWSER
#define FILEBROWSER

#include <simplegui.h>
#include <dirent.h>
#include <cstdio>

class FileBrowser;

FileBrowser* fileBrowser = NULL;

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
        loadFiles();
        launchEvents();
        run();
    }

    void launchWidgets() {
        back    = new Button(10,13,13,14,"<");
        path    = new Label(25,10,365,20,"/home/");
        folders = new List(10,40,185,145);
        files   = new List(205,40,185,145);
        add(back);
        add(path);
        add(folders);
        add(files);
    }

    void loadFolders() {
        folders->removeAll();
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

    void loadFiles() {
        files->removeAll();
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir (path->getText())) != NULL) {
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
        fileBrowser = this;
        folders->doubleClicked = &forwardFolder;
        back->action = &backFolder;
    }

    static void forwardFolder(const char item[]) {
        char fullpath[256];
        sprintf(fullpath,"%s%s/",fileBrowser->path->getText(),item);
        fileBrowser->path->setText(fullpath);
        fileBrowser->loadFolders();
        fileBrowser->loadFiles();
    }

    static void backFolder() {
        char fullpath[256];
        strcpy(fullpath,fileBrowser->path->getText());
        for (int i=strlen(fullpath)-2; i>=0; i--) {
            if (fullpath[i]=='/') {
                fullpath[i+1] = 0;
                break;
            }
        }
        fileBrowser->path->setText(fullpath);
        fileBrowser->loadFolders();
        fileBrowser->loadFiles();
    }

};


#endif // FILEBROWSER

