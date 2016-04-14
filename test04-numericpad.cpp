#include <simplegui.h>

class NumericPad* numericPad;
class NumericPad :public Frame {
private:
    Label*  display;
    Button* numbers[10];
    Button* operators[5];

    static void operation(char text[]) {
        char fulltext[256];
        sprintf(fulltext,"%s%s",numericPad->display->getText(),text);
        numericPad->display->setText(fulltext);
    }

public:
    NumericPad() : Frame(100,100,150,230,"Num Pad") {
        numericPad = this;
        add( display = new Label(20,20,110,20,"") );
        int h=20,v=60;
        for (int i=9; i>=0; i--) {
            char text[2];
            sprintf(text,"%d",i);
            add ( numbers[i] = new Button(h,v,30,30,text) );
            if ((i+2)%3==0) {
                h=20;
                v+=40;
            } else {
                h+=40;
            }
            numbers[i]->taggedAction = &operation;
        }
        run();
    }
};

int main() {
    NumericPad c;
}
