#include <simplegui.h>

Frame* frame;

void quit() {
    frame->dispose();
}

int main() {
    frame = new Frame(10,10,600,400,"My Window");
    frame->add(new Label(20,20,100,20,"Nombre"));
    frame->add(new TextField(150,20,300,20));
    frame->add(new Label(20,50,100,20,"Direccion"));
    frame->add(new TextField(150,50,300,20));
    frame->add(new Button(20,360,100,20,"Salir"))->action = &quit;
    frame->run();
}
