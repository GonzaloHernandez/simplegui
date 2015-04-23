#ifndef GUI
#define GUI

#include <X11/Xlib.h>
#include <cstring>
#include <iostream>
#include <X11/Xutil.h>
#include <sys/time.h>

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
    char text[256];
    bool focused;
    bool hidden;
    Display*    display;
    int         screen;
    Window      window;
    GC          gc;

    //-------------------------------------------------------------------------------
    static void defaultAction() {}

public:
    void (*action)();

    Widget(int x,int y,int width,int height,const char text[])
        : x(x), y(y), width(width), height(height), focused(false), hidden(true) {
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
    //-------------------------------------------------------------------------------
    char* getText() {
        return text;
    }
    //-------------------------------------------------------------------------------
    void setText(const char text[]) {
        strcpy(this->text,text);
        draw();
    }
    //-------------------------------------------------------------------------------
    void show() {
        hidden = false;
        draw();
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
        if (hidden) return;
        XClearArea(display,window,x+1,y+1,width-1,height-1,false);
        unsigned int columns = (width-8)/6;
        char* visibletext = new char[columns+1];
        strncpy(visibletext,text,columns);
        visibletext[columns]=0;
        XDrawString(display,window,gc,x+5,y+height/2+5,visibletext,strlen(visibletext));
    }
};

class TextField : public Widget {
private:
    bool editing;
public:
    TextField(int x,int y,int width,int height)
        : Widget(x,y,width,height,""){
        editing=false;
    }
    //-------------------------------------------------------------------------------
    void draw() {
        if (hidden) return;
        XDrawRectangle(display,window,gc,x,y,width,height);
        XClearArea(display,window,x+1,y+1,width-1,height-1,false);
        XDrawString(display,window,gc,x+5,y+height/2+5,text,strlen(text));
        if (editing) {
            XDrawLine(display,window,gc,x+5+strlen(text)*6,y+height/2-6,x+5+strlen(text)*6,y+height/2+6);
        }
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
                (*action)();
                return true;
            case XK_Tab:
                return true;
            case XK_BackSpace:
                if (strlen(text)>0) {
                    text[strlen(text)-1]=0;
                }
                break;
            default:
                unsigned int columns = (width-10)/6;
                if (strlen(text) >= columns) break;
                if (keysym==XK_Shift_L || keysym==XK_Shift_R) break;
                if (keysym>=XK_F1 && keysym<=XK_F35) break;
                int pos = strlen(text);
                text[pos]=keysym;
                text[pos+1]=0;
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
            editing=true;
            draw();
        }
        else if (!focused && this->focused) {
            editing=false;
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
        if (hidden) return;
        XDrawRectangle(display,window,gc,x,y,width,height);

        if (active) {
            XSetForeground(display,gc,rgb(200,200,200));
            XFillRectangle(display,window,gc,x+1,y+1,width-1,height-1);
            XSetForeground(display,gc,rgb(0,0,0));
        }
        else {
            XClearArea(display,window,x+1,y+1,width-1,height-1,false);
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

#include <vector>

/**
 * @brief The List class
 */
class List : public Widget {
private:
    std::vector<char*> items;
    int current,first;
    timeval previewsclicktime;

    //-------------------------------------------------------------------------------
    static void defaultSelectionChanged(const char[]) {}
    static void defaultDoubleClicked(const char[]){}

public:
    void (*selectionChanged)(const char[]);
    void (*doubleClicked)(const char[]);

    List(int x,int y,int width,int height)
        : Widget(x,y,width,height,"") {
        current=-1;
        first=0;
        selectionChanged = &defaultSelectionChanged;
        doubleClicked = &defaultDoubleClicked;
    }
    //-------------------------------------------------------------------------------
    ~List() {
        removeAll();
    }
    //-------------------------------------------------------------------------------
    void draw() {
        if (hidden) return;
        XDrawRectangle(display,window,gc,x,y,width,height);
        XClearArea(display,window,x+1,y+1,width-1,height-1,false);

        unsigned int rows    = height/13;
        unsigned int columns = (width-8)/6;

        if (current>=0 && items.size()>0) {
            XSetForeground(display,gc,rgb(150,150,255));
            XFillRectangle(display,window,gc,x+1,y+1+current*13,width-1,13);
            XSetForeground(display,gc,rgb(0,0,0));
        }
        for (unsigned int i=first,r=0; i<items.size() && r<rows; i++) {
            char* item = new char[columns+1];
            strncpy(item,items[i],columns);
            item[columns]=0;
            XDrawString(display,window,gc,x+5,y+12+r*13,item,strlen(item));
            delete item;
            r++;
        }
    }
    //-------------------------------------------------------------------------------
    void add(const char item[]) {
        char* buffer = new char[strlen(item)+1];
        strcpy(buffer,item);
        items.push_back(buffer);
        draw();
    }
    //-------------------------------------------------------------------------------
    bool triggerEvent(XEvent& event) {
        switch (event.type) {
        case ButtonPress:
            switch (event.xbutton.button) {
            case 1: // left button
                if (mouseInArea(event)) {
                    unsigned int position = (event.xbutton.y-y)/13;
                    unsigned int rows    = height/13;
                    if (position<items.size()-first && position<rows) {
                        current = position;
                        (*selectionChanged)(items.at(current+first));

                        struct timeval currentclicktime;
                        gettimeofday(&currentclicktime,NULL);
                        if (currentclicktime.tv_sec*1000+currentclicktime.tv_usec/1000 -
                                previewsclicktime.tv_sec*1000+previewsclicktime.tv_usec/1000 <1000) {
                            (*doubleClicked)(items.at(current+first));
                        }
                        previewsclicktime = currentclicktime;
                    }

                    draw();
                }
                break;
            case 5: // scrollmouse down
                if (mouseInArea(event)) if (first>0) {
                    int rows    = height/13;
                    first--;
                    if (current>=0 && current<rows-1) {
                        current++;
                    }
                    draw();
                }
                break;
            case 4: // scrollmouse up
                if (mouseInArea(event)) if (first<(int)items.size()-1){
                    first++;
                    if (current>0) {
                        current--;
                    }
                    draw();
                }
                break;
            }
            break;
        }
        return false;
    }
    //-------------------------------------------------------------------------------
    int count() {
        return items.size();
    }
    //-------------------------------------------------------------------------------
    void removeAll() {
        for (unsigned int i=0; i<items.size(); i++) {
            delete items[i];
        }
        items.clear();
        current = -1;
        first = 0;
        draw();
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
        Atom WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(display, window, &WM_DELETE_WINDOW, 1);
        XStoreName(display,window,text);

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
        for (int i=0; i<MAX; i++) if (widgets[i]){
            widgets[i]->show();
        }
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
            case ClientMessage:
                dispose();
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

