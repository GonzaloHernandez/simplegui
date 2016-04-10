#ifndef FILEBROWSER
#define FILEBROWSER

#include <simplegui.h>
#include <dirent.h>
#include <cstdio>

class FileBrowser* fileBrowser = NULL;
char fullname[256];

class FileBrowser :public Frame {
private:
    Button* back;
    Label*  path;
    List*   folders;
    List*   files;

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

    static void forwardFolder(const char item[]) {
        char fullpath[256];
        sprintf(fullpath,"%s%s/",fileBrowser->path->getText(),item);
        fileBrowser->path->setText(fullpath);
        fileBrowser->loadFolders();
        fileBrowser->loadFiles();
    }

    static void selectFile(const char item[]) {
        sprintf(fullname,"%s%s",fileBrowser->path->getText(),item);
        fileBrowser->dispose();
    }

public:
    FileBrowser() : Frame(100,100,400,200,"Seleccione un Archivo") {
        fileBrowser = this;
        back    = new Button(10,13,13,14,"<");
        path    = new Label(25,10,365,20,"/home/");
        folders = new List(10,40,185,145);
        files   = new List(205,40,185,145);
        add(back);
        add(path);
        add(folders);
        add(files);

        loadFolders();
        loadFiles();

        back->action = &backFolder;
        folders->doubleClicked = &forwardFolder;
        files->doubleClicked = &selectFile;

        strcpy(fullname,"");
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
        folders->sort();
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
        files->sort();
    }


    static const char* searchFile() {
        FileBrowser fb;
        fb.run();
        return fullname;
    }

};


#endif // FILEBROWSER

