#ifndef MESSAGE_H
#define MESSAGE_H

#include <simplegui.h>
#include <cstdio>

enum MessageBoxType {
    MSG_OK,MSG_OKCANCEL,MSG_YESNO
};

class MessageBox* messageBox = NULL;

class MessageBox :public Frame {
private:
    Label*  info;
    Button* buttons[2];
    MessageBoxType type;
    int returnValue;

    static void triggerOk() {
        messageBox->returnValue = 0;
        messageBox->dispose();
    }

    static void triggerCancel() {
        messageBox->returnValue = 1;
        messageBox->dispose();
    }

    static void triggerYes() {
        messageBox->returnValue = 0;
        messageBox->dispose();
    }

    static void triggerNo() {
        messageBox->returnValue = 1;
        messageBox->dispose();
    }

    static void triggerButton(MessageBoxType type) {
        if (type == MSG_OK) {
        }
    }

public:
    MessageBox(const char title[], const char text[], MessageBoxType type)
        : Frame(100,100,strlen(text)*6+20,70,title) {
        messageBox = this;
        int msgWidth = strlen(text)*6;
        this->type = type;
        add( info    = new Label(10,10,msgWidth,20,text) );
        switch (type) {
        case MSG_OK:
            add( buttons[0] = new Button(10,40,msgWidth,20,"Ok") );
            buttons[0]->action = &triggerOk;
            break;
        case MSG_OKCANCEL:
            add( buttons[0] = new Button(10,40,msgWidth/2-5,20,"Ok") );
            add( buttons[1] = new Button(20+msgWidth/2-5,40,msgWidth/2-5,20,"Cancel") );
            buttons[0]->action =  &triggerOk;
            buttons[1]->action =  &triggerCancel;
            break;
        case MSG_YESNO:
            add( buttons[0] = new Button(10,40,msgWidth/2-5,20,"Yes") );
            add( buttons[1] = new Button(20+msgWidth/2-5,40,msgWidth/2-5,20,"No") );
            buttons[0]->action =  &triggerYes;
            buttons[1]->action =  &triggerNo;
            break;
        }
    }

    static int show(const char title[], const char text[], MessageBoxType type=MSG_OK) {
        MessageBox msg(title,text,type);
        msg.run();
        return messageBox->returnValue;
    }

};

#endif // MESSAGE_H
