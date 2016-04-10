#include <simplegui.h>
#include <messagebox.h>

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

    static void nameEntered() {
        registerForm->changeFocus(registerForm->address);
    }

    static void addressEntered() {
        registerForm->changeFocus(registerForm->phone);
    }

    static void saveAction() {
        MessageBox::show("Result","Your information was saved in the Data Base !");
        cancelAction();
    }

    static void cancelAction() {
        registerForm->name->setText("");
        registerForm->address->setText("");
        registerForm->phone->setText("");
    }

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
        save->action = &saveAction;
        cancel->action = &cancelAction;

        run();
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
