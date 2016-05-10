#include "simplegui.h"
#include "filebrowser.h"
#include <iostream>

using namespace std;

typedef unsigned char byte;

class FileScanner* scanner;

class FileScanner :public Frame {
    static const int MAX = 50;
private:
    Label*  pos[MAX];
    Button* unity[MAX];
    //--------------------------------------------------------------------
    void scan(const char fileName[]) {
        FILE* file = fopen(fileName,"r");
        if (!file) return;
        byte buffer;
        for (int i=0; i<MAX; i++) {
            fread(&buffer,1,1,file);
            char text[5];
            sprintf(text,"%d",buffer);
            unity[i]->setText(text);
        }
        fclose(file);
    }
    //--------------------------------------------------------------------
    void launchWidgets() {
        for (int i=0; i<MAX; i++) {
            char text[5];
            sprintf(text,"%d",i);
            add( pos[i] = new Label(   20+i*35,20,30,10,text,CENTER,rgb(255,0,0)) );
            add( unity[i] = new Button(20+i*35,35,30,20) );
            unity[i]-> referencedAction = &click;
        }
    }
    //--------------------------------------------------------------------
    static void click(Widget* ref) {
        char text[5];
        sprintf(text,"%c",atoi(ref->getText()));
        ref->setText(text);
    }

public:
    FileScanner() : Frame(100,100,800,70,"File Scanner") {
        scanner = this;
        launchWidgets();
        const char* fileName = FileBrowser::searchFile();
        scan(fileName);
        run();
    }
};

int main() {
    FileScanner();
}
