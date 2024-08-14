#ifndef COURSE_APP_H
#define COURSE_APP_H

#include <latch>

class App {
public:
    App();
    ~App();
    void start();
    void wait_interrupt();
    bool is_interrupted();

protected:
    virtual void run() = 0;
};


#endif //COURSE_APP_H
