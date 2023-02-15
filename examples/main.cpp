#include "ex/experiment_app.h"

i32 main(i32, char const**) {
    App app;
    if (i32 err = app.init(); err != AppExitCodes::APP_EXIT_SUCCESS) return err;
    return app.run();
}
