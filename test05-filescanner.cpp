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
    vector<Button*> fields;
    Widget* lbound,*ubound;
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
            add( pos[i] = new Label(   20,20+i*15,30,13,text,RIGHT,rgb(255,0,0)) );
            add( unity[i] = new Button(60,20+i*15,30,13) );
            unity[i]-> referencedAction = &click;
        }
    }

public:
    FileScanner() : Frame(100,100,800,70,"File Scanner",true) {
        scanner = this;
        lbound = ubound = NULL;
        launchWidgets();
        const char* fileName = FileBrowser::searchFile("/home/chalo/Music/");
        if (strcmp(fileName,"")!=0) {
            scan(fileName);
            run();
        }
    }
private:
    //--------------------------------------------------------------------
    static void click(Widget* ref) {
        if (ref->getForecolor()==rgb(0,0,0)) {
            ref->setForecolor(rgb(255,0,0));
            if (!scanner->lbound) {
                scanner->lbound = ref;
            }
            else {
                scanner->ubound = ref;
                Button* n = new Button(150,scanner->lbound->getY(),20,scanner->ubound->getY()-scanner->lbound->getY()+20);
                scanner->add(n);

            }
        }
    }
};

int main() {
    FileScanner();
}
