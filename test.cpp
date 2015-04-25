#include <simplegui.h>
#include <vector>
#include <iostream>
#include <filebrowser.h>

using namespace std;

class Package;
Package* package;


class Package :public Frame {
private:
    Button* browsefile;
    Label*  filename;
    List*   listfiles;
public:
    Package() {
        browsefile  = new Button(20,20,100,20,"...");
        filename    = new Label(150,20,430,20,"");
        listfiles   = new List(20,60,560,300);
        add(browsefile);
        add(filename);
        add(listfiles);
        package = this;
        browsefile->action = &browse;
        run();
    }

    static void browse() {
        const char* filename = FileBrowser::searchFile();
        if (strcmp(filename,"")==0) return;
        package->filename->setText(filename);
        package->listfiles->add(filename);
    }

};

int main() {
    Package p;//(100,100,600,400,"Empaquetador");
}
