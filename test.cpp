#include <simplegui.h>
#include <vector>
#include <iostream>

using namespace std;

Frame* frame;
List* list;
TextField* name;
void quit() {
    frame->dispose();
}

void append() {
    list->add(name->getText());
    name->setText("");
}

int main() {
    frame = new Frame(10,10,600,400,"My Window");
    frame->add(new Label(20,20,100,20,"Nombre"));
    name = new TextField(150,20,300,20);
    name->action = &append;
    frame->add(name);
    frame->add(new Label(20,50,100,20,"Direccion"));
    frame->add(new TextField(150,50,300,20));
    frame->add(new Button(20,360,100,20,"Salir"))->action = &quit;

    list = new List(20,80,200,100);
    list->add("One");
    list->add("Two");
    list->add("Three");
    frame->add(list);
    frame->run();
}
