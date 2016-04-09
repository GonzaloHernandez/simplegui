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
    Message(const char title[], const char text[]) : Frame(100,100,strlen(text)*6+20,70,title) {
        message = this;
        info    = new Label(10,10,strlen(text)*6,20,text);
        ok      = new Button(10,40,strlen(text)*6,20,"Ok");
        add(info);
        add(ok);
        ok->action = &accept;
    }

    static int show(const char title[], const char text[]) {
        Message msg(title,text);
        msg.run();
        return 0;
    }

};

#endif // MESSAGE_H
