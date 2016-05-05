#include "simplegui.h"

class HelloWorld* hello;

class HelloWorld :public Frame {
private:
    Button* greet;
public:
    HelloWorld() : Frame(100,100,240,60,"My first frame"){
        hello = this;
        add( greet = new Button(20,20,200,20,"Greet!") );
        greet->action = &executeAction;
        run();
    }
private:
    static void executeAction() {
        hello->greet->setText("Hello Word!");
    }
};

int main() {
   HelloWorld();
}
