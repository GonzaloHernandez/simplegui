#ifndef GUI
#define GUI

#include <X11/Xlib.h>
#include <cstring>
#include <iostream>
#include <X11/Xutil.h>

unsigned long rgb(unsigned long red, unsigned long green, unsigned long blue) {
    return red<<16 | green<<8 | blue;
}

/**
 * @brief The Widget class
 * ==================================================================================
 */
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

    //-------------------------------------------------------------------------------
    static void defaultAction() {}

public:
    void (*action)();

    Widget(int x,int y,int width,int height,const char text[])
        : x(x), y(y), width(width), height(height), focused(false) {
        strcpy(this->text,text);
        action = &defaultAction;
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

/**
 * @brief The Label class
 * ==================================================================================
 */
class Label : public Widget {
public:
    Label(int x,int y,int width,int height,const char text[])
        : Widget(x,y,width,height,text){
    }
    //-------------------------------------------------------------------------------
    void draw() {
        XDrawString(display,window,gc,x+10,y+height/2+5,text,strlen(text));
    }
};

class TextField : public Widget {
public:
    TextField(int x,int y,int width,int height)
        : Widget(x,y,width,height,""){
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
            char buffer[40];
            KeySym keysym;
            XComposeStatus compose;
            XLookupString(&event.xkey,buffer,10,&keysym,&compose);
            switch(keysym) {
            case XK_Escape:
                return false;
            case XK_Return:
                return true;
            case XK_Tab:
                return true;
            case XK_BackSpace:
                if (strlen(text)>1) {
                    text[strlen(text)-2]='_';
                    text[strlen(text)-1]=0;
                }
                break;
            default:
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

/**
 * @brief The Button class
 * ==================================================================================
 */
class Button : public Widget {
private:
    bool active;
public:
    Button(int x,int y,int width,int height,const char text[])
        : Widget(x,y,width,height,text), active(false) {
    }
    //-------------------------------------------------------------------------------
    void draw() {
        XDrawRectangle(display,window,gc,x,y,width,height);

        if (active) {
            XSetForeground(display,gc,rgb(200,200,200));
            XFillRectangle(display,window,gc,x+1,y+1,width-1,height-1);
            XSetForeground(display,gc,rgb(0,0,0));
        }
        else {
            XClearArea(display,window,x+1,y+1,width-2,height-2,false);
        }
        int cx = x+width/2;
        int cy = y+height/2;
        int twidth = strlen(text)*7;
        int theight = 10;
        XDrawString(display,window,gc,cx-twidth/2,cy+theight/2,text,strlen(text));
    }
    //-------------------------------------------------------------------------------
    bool triggerEvent(XEvent& event) {
        switch (event.type) {
        case ButtonPress:
            if (mouseInArea(event)) {
                active = true;
                draw();
            }
            break;
        case ButtonRelease:
            if (mouseInArea(event)) {
                (*action)();
                return true;
            }
            break;
        case MotionNotify:
            if (active && !mouseInArea(event)) {
                active = false;
                draw();
            }
        }
        return false;
    }
};

/**
 * @brief The Frame class
 * ==================================================================================
 */
class Frame : public Widget {
    static const int   MAX = 100;
private:
    bool        active;

    Display*    display;
    int         screen;
    Window      window;
    GC          gc;
    Widget*     widgets[MAX];
    Widget*     current;

public:
    Frame(int x,int y,int width,int height,const char title[])
        : Widget(x,y,width,height,title), active(true) {
        strcpy(this->text,title);
        display = XOpenDisplay(NULL);
        screen  = DefaultScreen(display);
        window  = XCreateSimpleWindow(display,RootWindow(display,screen),
                                      x,y,width,height,1,0,255<<16|255<<8|255);
        gc      = XDefaultGC(display,screen);
        XMapWindow(display,window);
        XSelectInput(display,window,ExposureMask|ButtonPressMask|ButtonReleaseMask
                     |KeyPressMask|PointerMotionMask);
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
        while(active) {
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
                if (event.xkey.keycode==9) active=false;
                break;
            case Expose:
                for (int i=0; i<MAX; i++) if (widgets[i]) {
                    widgets[i]->draw();
                }
            case MotionNotify:
                break;
            default:
                break;
            }
            done: {}
        }
    }
    //-------------------------------------------------------------------------------
    Widget* add(Widget* widget) {
        for (int i=0; i<MAX; i++) if (!widgets[i]) {
            widgets[i] = widget;
            widgets[i]->updateGraphVariables(display,screen,window,gc);
            return widget;
        }
        return NULL;
    }
    //-------------------------------------------------------------------------------
    void dispose() {
        active = false;
    }
};


#endif // GUI

