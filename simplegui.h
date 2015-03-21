#ifndef GUI
#define GUI

#include <X11/Xlib.h>
#include <cstring>
#include <iostream>
#include <X11/Xutil.h>

class Widget {
protected:
    int x,y;
    int width,height;
    char text[30];
    bool focused;
    Display*    display;
    int         screen;
    Window      window;
    GC          gc;
public:
    Widget(int x,int y,int width,int height,const char text[])
        : x(x), y(y), width(width), height(height), focused(false) {
        strcpy(this->text,text);
    }
    //-------------------------------------------------------------------------------
    void updateGraphVariables(Display* display,int screen,Window window,GC gc) {
        this->display   = display;
        this->screen    = screen;
        this->window    = window;
        this->gc        = gc;
    }
    //-------------------------------------------------------------------------------
    virtual ~Widget() {
    }
    //-------------------------------------------------------------------------------
    virtual void draw() {
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
    //-------------------------------------------------------------------------------
    virtual void setFocused(bool) {
        this->focused = false;
    }
};

class Label : public Widget {
public:
    Label(int x,int y,int width,int height,const char text[]) : Widget(x,y,width,height,text){
    }
    //-------------------------------------------------------------------------------
    void draw() {
        XDrawString(display,window,gc,x+10,y+height/2+5,text,strlen(text));
    }
};

class TextField : public Widget {
public:
    TextField(int x,int y,int width,int height) : Widget(x,y,width,height,""){
    }
    //-------------------------------------------------------------------------------
    void draw() {
        XDrawRectangle(display,window,gc,x,y,width,height);
        XClearArea(display,window,x+1,y+1,width-2,height-2,false);
        XDrawString(display,window,gc,x+10,y+height/2+5,text,strlen(text));
    }
    //-------------------------------------------------------------------------------
    bool triggerEvent(XEvent& event) {
        switch (event.type) {
        case ButtonPress:
            if (mouseInArea(event)) return true;
            break;
        case KeyPress:
//            std::cout << event.xkey.keycode << std::endl;
            switch(event.xkey.keycode) {
            case  9:    // ESC
                return false;
            case 36:    // ENTER
                return true;
            case 23:    // TAB
                return true;
            case 22:    // BACKSPACE
                if (strlen(text)>1) {
                    text[strlen(text)-2]='_';
                    text[strlen(text)-1]=0;
                }
                break;
            default:
                char buffer[40];
                KeySym keysym;
                XComposeStatus compose;
                XLookupString(&event.xkey,buffer,10,&keysym,&compose);
                if (keysym==XK_Shift_L || keysym==XK_Shift_R) break;
                if (keysym>=XK_F1 && keysym<=XK_F35) break;
                text[strlen(text)-1]=keysym;
                text[strlen(text)+1]=0;
                text[strlen(text)]='_';
                break;
            }
            draw();
            return true;
        }
        return false;
    }
    //-------------------------------------------------------------------------------
    void setFocused(bool focused) {
        if (focused && !this->focused) {
            strcat(text,"_");
            draw();
        }
        else if (!focused && this->focused) {
            text[strlen(text)-1]=0;
            draw();
        }
        this->focused = focused;
    }
};

class Button : public Widget {
public:
    Button(int x,int y,int width,int height,const char text[]) : Widget(x,y,width,height,text){
    }
    //-------------------------------------------------------------------------------
    void draw() {
        XDrawRectangle(display,window,gc,x,y,width,height);
        XClearArea(display,window,x+1,y+1,width-2,height-2,false);
        XDrawString(display,window,gc,x+10,y+height/2+5,text,strlen(text));
    }
    //-------------------------------------------------------------------------------
    bool triggerEvent(XEvent& event) {
        switch (event.type) {
        case ButtonPress:
            if (mouseInArea(event)) return true;
            break;
        }
        return false;
    }
    //-------------------------------------------------------------------------------
    void setFocused(bool focused) {
        if (focused && !this->focused) {
            strcat(text,"_");
            draw();
        }
        else if (!focused && this->focused) {
            text[strlen(text)-1]=0;
            draw();
        }
        this->focused = focused;
    }
};


class Frame : public Widget {
    static const int   MAX = 100;
private:
    Display*    display;
    int         screen;
    Window      window;
    GC          gc;
    Widget*     widgets[MAX];
    Widget*     current;
public:
    Frame(int x,int y,int width,int height,const char title[])
        : Widget(x,y,width,height,title) {
        strcpy(this->text,title);
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
            if (current) if (current->triggerEvent(event)) goto done;

            for (int i=0; i<MAX; i++) {
                if (widgets[i]) {
                    if (widgets[i]->triggerEvent(event)) {
                        if (current) current->setFocused(false);
                        current = widgets[i];
                        current->setFocused(true);
                        goto done;
                    }
                }
            }

            switch (event.type) {
            case KeyPress:
                if (event.xkey.keycode==9) keepruning=false;
                break;
            case Expose:
                for (int i=0; i<MAX; i++) if (widgets[i]) {
                    widgets[i]->draw();
                }
            default:
                break;
            }
            done: {}
        }
    }
    //-------------------------------------------------------------------------------
    void add(Widget* widget) {
        for (int i=0; i<MAX; i++) if (!widgets[i]) {
            widgets[i] = widget;
            widgets[i]->updateGraphVariables(display,screen,window,gc);
            return;
        }
    }
};


#endif // GUI

