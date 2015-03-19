#ifndef GUI
#define GUI

#include <X11/Xlib.h>
#include <cstring>
#include <iostream>

class Widget {
protected:
    int x,y;
    int width,height;
public:
    Widget(int x,int y,int width,int height)
        : x(x), y(y), width(width), height(height) {
    }
    //-------------------------------------------------------------------------------
    virtual ~Widget() {
    }
    //-------------------------------------------------------------------------------
    virtual void draw(Display*,Window,GC) {
    }
    //-------------------------------------------------------------------------------
    virtual bool triggerEvent(XEvent&) {
        return false;
    }
    //-------------------------------------------------------------------------------
    bool mouseInArea(XEvent& event) {
        int mx = event.xbutton.x;
        int my = event.xbutton.y;
        if (mx > x && mx < x+width && my > y && my < y+height) {
            return true;
        }
        return false;
    }
};

class Label : public Widget {
private:
    char text[80];
public:
    Label(int x,int y,int width,int height,const char text[]) : Widget(x,y,width,height){
        strcpy(this->text,text);
    }
    //-------------------------------------------------------------------------------
    void draw(Display* display,Window window,GC gc) {
        XDrawString(display,window,gc,x+10,y+height/2+5,text,strlen(text));
    }
};


class TextField : public Widget {
private:
    char text[80];
public:
    TextField(int x,int y,int width,int height) : Widget(x,y,width,height){
        strcpy(text,"");
    }
    //-------------------------------------------------------------------------------
    void draw(Display* display,Window window,GC gc) {
        XDrawRectangle(display,window,gc,x,y,width,height);
        XDrawString(display,window,gc,x+10,y+height/2+5,text,strlen(text));
    }
    //-------------------------------------------------------------------------------
    bool triggerEvent(XEvent& event) {
        switch (event.type) {
        case KeyPress:
            std::cout << event.xkey.keycode << std::endl;
            return true;
        default:
            break;
        }
        return false;
    }
};

class Frame : public Widget {
    static const int   MAX = 100;
private:
    char        title[];
    Display*    display;
    int         screen;
    Window      window;
    GC          gc;
    Widget*     widgets[MAX];
    Widget*     current;
public:
    Frame(int x,int y,int width,int height,const char title[])
        : Widget(x,y,width,height) {
        strcpy(this->title,title);
        display = XOpenDisplay(NULL);
        screen  = DefaultScreen(display);
        window  = XCreateSimpleWindow(display,RootWindow(display,screen),
                                      x,y,width,height,1,0,255<<16|255<<8|255);
        gc      = XDefaultGC(display,screen);
        XMapWindow(display,window);
        XSelectInput(display,window,ExposureMask|ButtonPressMask|KeyPressMask);
        for (int i=0; i<MAX; i++) widgets[i]=NULL;
        current = NULL;
    }
    //-------------------------------------------------------------------------------
    ~Frame() {
        for (int i=0; i<MAX; i++) {
            if (widgets[i]) {
                delete widgets[i];
            }
        }
        XCloseDisplay(display);
    }
    //-------------------------------------------------------------------------------
    void run() {
        bool keepruning=true;
        while(keepruning) {
            XEvent event;
            XNextEvent(display,&event);
            for (int i=0; i<MAX; i++) {
                if (widgets[i]) {
                    if (widgets[i]->triggerEvent(event)) goto done;
                }
            }
            switch (event.type) {
            case KeyPress:
                if (event.xkey.keycode==9) keepruning=false;
                break;
            case Expose:
                for (int i=0; i<MAX; i++) if (widgets[i]) {
                    widgets[i]->draw(display,window,gc);
                }
            case ButtonPress:
                for (int i=0; i<MAX; i++)if (widgets[i]) {
                    widgets[i]->draw(display,window,gc);
                }
            default:
                break;
            }
            done:
            {
            }
        }
    }
    //-------------------------------------------------------------------------------
    void add(Widget* widget) {
        for (int i=0; i<MAX; i++) if (!widgets[i]) {
            widgets[i] = widget;
            return;
        }
    }
};


#endif // GUI

