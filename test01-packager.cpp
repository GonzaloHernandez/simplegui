#include <simplegui.h>
#include <filebrowser.h>

/**
 * @brief Packager
 * ==================================================================================
 */
class Packager* package;

class Packager :public Frame {
private:
    Button* browsefile;
    Label*  filename;
    List*   listfiles;
public:
    Packager() : Frame(100,100,600,380,"File browser"){
        package = this;
        browsefile  = new Button(20,20,100,20,"Search File");
        filename    = new Label(150,20,430,20);
        listfiles   = new List(20,60,560,300);
        add(browsefile);
        add(filename);
        add(listfiles);
        browsefile->action = &browse;
        run();
    }

    static void browse() {
        const char* filename = FileBrowser::searchFile();
        if (strcmp(filename,"")==0) return;
        package->filename->setText(filename);
        package->listfiles->add(filename);
    }
};

/**
 * @brief main
 * @return
 * ==================================================================================
 */

int main() {
    Packager();
}
