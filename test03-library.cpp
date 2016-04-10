#include <simplegui.h>

class Library* library;
class Library :public Frame {
private:
    TextField*  book;
    Button*     insert;
    Button*     lend;
    Button*     restore;
    List*       availables;
    List*       takens;

    static void addBook() {
        library->availables->add( library->book->getText() );
        library->book->setText("");
    }

    static void lendBook() {
        library->takens->add( library->availables->getText() );
        library->availables->remove( library->availables->getCurrentIndex() );
    }

    static void restoreBook() {
        library->availables->add( library->takens->getText() );
        library->takens->remove( library->takens->getCurrentIndex() );
    }

public:
    Library() : Frame(100,100,500,300,"Library") {
        library = this;
        add( new Label(20,20,80,20,"New Book:") );
        add( book = new TextField(100,20,280,20) );
        add( insert = new Button(400,20,80,20,"Add Book") );
        add (new Label( 20,50,200,20,"Available Books") );
        add (new Label(280,50,200,20,"Taken Books") );
        add( availables = new List(20,70,200,210) );
        add( takens = new List(280,70,200,210) );
        add( lend = new Button(240,100,20,20,">") );
        add( restore = new Button(240,140,20,20,"<") );

        insert->action = &addBook;
        book->action = &addBook;
        lend->action = &lendBook;
        restore->action = &restoreBook;

        run();
    }
};

int main() {
    Library l;
}
