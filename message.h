#ifndef MESSAGE_H
#define MESSAGE_H

#include <simplegui.h>
#include <cstdio>

class Message* message = NULL;

class Message :public Frame {
private:
    Label*  info;
    Button* ok;

    static void accept() {
        message->dispose();
    }

public:
    Message(char title[], char text[]) : Frame(100,100,400,90,title) {
        message = this;
        info    = new Label(10,10,80,30,text);
        ok      = new Button(10,60,380,20,"Ok");
        add(info);
        add(ok);
        ok->action = &accept;
    }

    static const int show(char title[], char text[]) {
        Message msg(title,text);
        msg.run();
        return 0;
    }

};

#endif // MESSAGE_H
