#ifndef SIMPLEGUI_H
#define SIMPLEGUI_H

#include <X11/Xlib.h>
#include <cstring>
#include <X11/Xutil.h>
#include <sys/time.h>
#include <algorithm>

typedef unsigned char   byte;
typedef unsigned int    uing;
typedef unsigned long   ulong;

using namespace std;

ulong rgb(ulong red, ulong green, ulong blue) {
    return red<<16 | green<<8 | blue;
}

//===================================================================================

/**
 * @brief The Widget class
 * is the base of all classes in the simplegui library, this
 * class has the main visual information like a position, dimensions, presentation
 * and so on.  In addition to these properties, this class has two default actions,
 * which can be programmed in the future by a programmer user.
 */
class Widget {
protected:
    int x,y;
    int width,height;
    char text[256];
    bool focused;
    bool hidden;
    ulong forecolor;
    ulong backcolor;
    Display*    display;
    int         screen;
    Window      window;
    GC          gc;

    //-------------------------------------------------------------------------------
    static void defaultAction() {}
    //-------------------------------------------------------------------------------
    static void defaultTaggedAction(char []) {}
    //-------------------------------------------------------------------------------
    static void defaultReferencedAction(Widget*) {}

public:
    void (*action)();
    void (*taggedAction)(char text[]);
    void (*referencedAction)(Widget* ref);

    Widget(int x,int y,int width,int height,const char text[]="",
           ulong forecolor=rgb(0,0,0),ulong backcolor=rgb(255,255,255))
        : x(x), y(y), width(width), height(height), focused(false), hidden(true),
          forecolor(forecolor), backcolor(backcolor) {
        strcpy(this->text,text);
        action = &defaultAction;
        taggedAction = &defaultTaggedAction;
        referencedAction = &defaultReferencedAction;
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
    int getX() {
        return x;
    }
    //-------------------------------------------------------------------------------
    int getY() {
        return y;
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
    //-------------------------------------------------------------------------------
    ulong getForecolor() {
        return forecolor;
    }
    //-------------------------------------------------------------------------------
    void setForecolor(ulong forecolor) {
        this->forecolor = forecolor;
        draw();
    }
};

//===================================================================================

/**
 * @brief The Label class
 * is the most simple visual class, useful show information to
 * an application final user.  This class does not programm any action.
 */

enum AlignmentType {
    LEFT,RIGHT,CENTER
};

class Label : public Widget {
private:
    AlignmentType aligment;
    bool transparent;
public:
    Label(int x,int y,int width,int height=20,const char text[]="",
          AlignmentType aligment=LEFT,ulong forecolor = rgb(0,0,0),
          bool transparent=true,ulong backcolor=rgb(255,255,255))
        : Widget(x,y,width,height,text,forecolor,backcolor),aligment(aligment),
          transparent(transparent){
    }
    //-------------------------------------------------------------------------------
    void draw() {
        if (hidden) return;
        XClearArea(display,window,x+1,y+1,width-1,height-1,false);
        uint columns = (width-8)/6;
        char* visibletext = new char[columns+1];
        strncpy(visibletext,text,columns);
        visibletext[columns]=0;
        if (!transparent) {
            XSetForeground(display,gc,backcolor);
            XFillRectangle(display,window,gc,x+1,y+1,width-1,height-1);
        }

        XSetForeground(display,gc,forecolor);
        int px;
        switch (aligment) {
        case LEFT:
            px = (x+5);
            break;
        case CENTER:
            px = (x+width/2) - (strlen(text)*7)/2;
            break;
        case RIGHT:
            px = (x+width-5) - (strlen(text)*7);
            break;
        }

        XDrawString(display,window,gc,px,y+height/2+5,visibletext,strlen(visibletext));
        XSetForeground(display,gc,rgb(0,0,0));
    }
};

//===================================================================================

/**
 * @brief The TextField class
 * is the object used to get information since the final user.  To activate its
 * functionality, the user must do click over its area.
 */
class TextField : public Widget {
private:
    bool editing;
    bool tilde;
public:
    TextField(int x,int y,int width,int height=20)
        : Widget(x,y,width,height), editing(false), tilde(false){
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
            if (!editing) return false;
            char buffer[40];
            KeySym keysym;
            XComposeStatus compose;
            XLookupString(&event.xkey,buffer,10,&keysym,&compose);
            switch(keysym) {
            case XK_Escape:
                return false;
            case XK_Return:
                (*action)();
                (*taggedAction)(text);
                (*referencedAction)(this);
                return true;
            case XK_Tab:
                return false;
            case XK_BackSpace:
                if (strlen(text)>0) {
                    text[strlen(text)-1]=0;
                }
                break;
            default:
                uint columns = (width-10)/6;
                if (strlen(text) >= columns) break;

                if (keysym==65105) { // Tilde
                    tilde = true;
                    break;
                }

                if (keysym>65000) break; // Special Keys

                if (tilde) {
                    switch (keysym) {
                    case XK_a: keysym = XK_aacute; break;
                    case XK_e: keysym = XK_eacute; break;
                    case XK_i: keysym = XK_iacute; break;
                    case XK_o: keysym = XK_oacute; break;
                    case XK_u: keysym = XK_uacute; break;
                    }
                    tilde=false;
                }

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

//===================================================================================

/**
 * @brief The Button class
 * is the prefered object usted to execute actions in common applications.  This
 * object is actionated by means of a click with the mouse device.  This class can
 * program the two actions inherited of Widget class.
 */
class Button : public Widget {
private:
    bool active;
public:
    Button(int x,int y,int width,int height=20,const char text[]="")
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
        XSetForeground(display,gc,forecolor);
        XDrawString(display,window,gc,cx-twidth/2,cy+theight/2,text,strlen(text));
        XSetForeground(display,gc,rgb(0,0,0));
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
            if (active && mouseInArea(event)) {
                active = false;
                draw();
                (*action)();
                (*taggedAction)(text);
                (*referencedAction)(this);
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

//===================================================================================

#include <vector>

/**
 * @brief The List class
 * is a containter of textual items, it object can be stored more items as it can to
 * show.  The final user can use the scroll mouse to browse more item hiden behind
 * the object space.
 */
class List : public Widget {
private:
    std::vector<char*> items;
    int current,first;
    timeval previewsclicktime;

    //-------------------------------------------------------------------------------
    static bool less(char* item1,char* item2) {
        return (strcmp(item1,item2)<0);
    }

    //-------------------------------------------------------------------------------
    static void defaultSelectionChanged(const char[]) {}
    static void defaultDoubleClicked(const char[]){}

public:
    void (*selectionChanged)(const char[]);
    void (*doubleClicked)(const char[]);

    List(int x,int y,int width=100,int height=100)
        : Widget(x,y,width,height) {
        current=-1;
        first=0;
        selectionChanged = &defaultSelectionChanged;
        doubleClicked = &defaultDoubleClicked;
        gettimeofday(&previewsclicktime,NULL);
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

        uint rows    = height/15;
        uint columns = (width-8)/6;

        strcpy(text,"");
        if (current>=0 && items.size()>0) {
            XSetForeground(display,gc,rgb(150,150,255));
            XFillRectangle(display,window,gc,x+1,y+1+current*15,width-1,15);
            XSetForeground(display,gc,rgb(0,0,0));
            strcpy(text,items.at(current));
        }
        for (uint i=first,r=0; i<items.size() && r<rows; i++) {
            char* item = new char[columns+1];
            strncpy(item,items[i],columns);
            item[columns]=0;
            XDrawString(display,window,gc,x+5,y+12+r*15,item,strlen(item));
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
                    uint position = (event.xbutton.y-y)/15;
                    uint rows    = height/15;
                    if (position<items.size()-first && position<rows) {
                        bool sameitem = false;
                        if ((uint)current==position) {
                            sameitem = true;
                        }
                        current = position;
                        (*selectionChanged)(items.at(current+first));

                        struct timeval currentclicktime;
                        gettimeofday(&currentclicktime,NULL);
                        if (sameitem) {
                            ulong dif =
                                    currentclicktime.tv_sec%10000*1000000+currentclicktime.tv_usec -
                                    previewsclicktime.tv_sec%10000*1000000+previewsclicktime.tv_usec;
                            if (dif <2000000) {
                                (*doubleClicked)(items.at(current+first));
                            }
                        }
                        previewsclicktime = currentclicktime;
                    }

                    draw();
                }
                break;
            case 5: // scrollmouse down
                if (mouseInArea(event)) if (first>0) {
                    int rows    = height/15;
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
    int getCurrentIndex() {
        return current;
    }

    //-------------------------------------------------------------------------------
    int count() {
        return items.size();
    }
    //-------------------------------------------------------------------------------
    void remove(int i) {
        if (i<0) return;
        delete items[i];
        items.erase(items.begin()+i);
        current = -1;
        first = 0;
        draw();
    }
    //-------------------------------------------------------------------------------
    void removeAll() {
        for (uint i=0; i<items.size(); i++) {
            delete items[i];
        }
        items.clear();
        current = -1;
        first = 0;
        draw();
    }
    //-------------------------------------------------------------------------------
    void sort() {
        std::sort(items.begin(), items.end(), less );
        draw();
    }
};

//===================================================================================

/**
 * @brief The Frame class
 * manage all visual objects: Labels, InputText, Buttons, Lists, so on. One frame
 * works like a window, and send all actions to each object after use them.
 */
class Frame : public Widget {
    static const int   MAX = 500;
private:
    bool        active;
    bool        resizable;
    Widget*     widgets[MAX];
    Widget*     current;

public:
    Frame(int x=100,int y=100,int width=600,int height=400,const char title[]="",bool resizable=false)
        : Widget(x,y,width,height,title), active(true), resizable(resizable) {
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

        const char* fontname = "-bitstream-bitstream vera sans mono-medium-r-normal--10-0-0-0-m-0-iso8859-1";

        XFontStruct* font = XLoadQueryFont (display, fontname);
        if (!font) {
            font = XLoadQueryFont (display, "fixed");
        }
        XSetFont (display, gc, font->fid);

        for (int i=0; i<MAX; i++) widgets[i]=NULL;
        current = NULL;

        if (resizable) {
            XStoreName(display,window,text);
        }
        else {
            XWMHints *wm_hints;
            XClassHint *class_hints;
            XTextProperty windowName, iconName;
            char *window_name = (char*)title;
            char *icon_name = (char*)"";
            XSizeHints* size_hints = XAllocSizeHints();
            wm_hints = XAllocWMHints();
            class_hints = XAllocClassHint();
            size_hints->flags = PPosition|PSize|PMinSize|PMaxSize;
            size_hints->min_width = width;
            size_hints->min_height = height;
            size_hints->max_width = width;
            size_hints->max_height = height;

            XStringListToTextProperty(&window_name, 1, &windowName);
            XStringListToTextProperty(&icon_name, 1, &iconName);

            wm_hints->initial_state = NormalState;
            wm_hints->input = True;
            wm_hints->flags = StateHint | IconPixmapHint | InputHint;
            class_hints->res_name = (char*)"progname";
            class_hints->res_class = (char*)"Basicwin";

            int argc=1;
            char* argv[] = {(char*)"test"};

            XSetWMProperties(display,window,&windowName,&iconName,argv,argc,size_hints,
                             wm_hints,class_hints);
        }
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
                        changeFocus(widgets[i]);
                        goto done;
                    }
                }
            }

            switch (event.type) {
            case KeyPress:
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
    //-------------------------------------------------------------------------------
    void changeFocus(Widget* widget) {
        if (current) current->setFocused(false);
        current = widget;
        current->setFocused(true);
    }
};

/*! \mainpage Simple library to create graphic applications
 * This library will be used to create basic applications coding in C++ language and
 * including the XLib library (X Window System implementation)
 *
 * \section screenshot_sec Screenshot
 * Clasic application using Labels, InputTexts, Buttons and Lists.
 *
 * <img src="library-test.png">
 */


#endif // SIMPLEGUI_H
