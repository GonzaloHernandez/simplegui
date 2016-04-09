#include <simplegui.h>
#include <filebrowser.h>
#include <message.h>

/**
 * @brief Packager
 * ==================================================================================
 */
class Packager* package;

class Packager :public Frame {
private:
    Button* browsefile;
    TextField*  filename;
    List*   listfiles;
public:
    Packager() {
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

/**
 * @brief RegisterForm
 * ==================================================================================
 */
class RegisterForm* registerForm;

class RegisterForm :public Frame {
private:
    TextField*  name;
    TextField*  address;
    TextField*  phone;
    Button*     save;
    Button*     cancel;
public:
    RegisterForm() : Frame(100,100,320,150,"Register Form") {
        registerForm = this;
        add( new Label(20,20,60,20,"Name:") );
        add( new Label(20,50,60,20,"Address:") );
        add( new Label(20,80,60,20,"Phone:") );
        add( name = new TextField(100,20,200,20) );
        add( address = new TextField(100,50,200,20) );
        add( phone = new TextField(100,80,200,20) );

        add( save = new Button(20,110,130,20,"Save") );
        add( cancel = new Button(170,110,130,20,"Cancel") );

        name->action = &nameEntered;
        address->action = &addressEntered;
        save->action = &record;

        run();
    }

    static void nameEntered() {
        registerForm->changeFocus(registerForm->address);
    }

    static void addressEntered() {
        registerForm->changeFocus(registerForm->phone);
    }

    static void record() {
        Message::show("Result","Your information was saved");
    }
};

/**
 * @brief main
 * @return
 * ==================================================================================
 */

int main() {
    RegisterForm();
}
