#include <simplegui.h>
#include <vector>
#include <iostream>
#include <filebrowser.h>

using namespace std;

class Package* package;
class Package :public Frame {
private:
    Button* browsefile;
    TextField*  filename;
    List*   listfiles;
public:
    Package() {
        package = this;
        browsefile  = new Button(20,20,100,20,"...");
        filename    = new TextField(150,20,430,20);
        listfiles   = new List(20,60,560,300);
        add(browsefile);
        add(filename);
        add(listfiles);
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

class RegisterForm* registerForm;

class RegisterForm :public Frame {
private:
    TextField*  name;
    TextField*  address;
    TextField*  phone;
    Button*     save;
    Button*     cancel;
public:
    RegisterForm() : Frame(100,100,400,300,"Register Form") {
        registerForm = this;
        add( new Label(20,20,60,20,"Name:"));
        add( new Label(20,50,60,20,"Address:"));
        add( new Label(20,80,60,20,"Phone:"));
        add( name = new TextField(100,20,200,20));
        add( address = new TextField(100,50,200,20));
        add( phone = new TextField(100,80,200,20));

        add( save = new Button(20,110,100,20,"Save"));
        add( cancel = new Button(140,110,100,20,"Cancel"));

        name->action = &nameEntered;
        address->action = &addressEntered;

        run();
    }

    static void nameEntered() {
        registerForm->changeFocus(registerForm->address);
    }

    static void addressEntered() {
        registerForm->changeFocus(registerForm->phone);
    }

};

int main() {
    //Package p2;
    RegisterForm();
}
