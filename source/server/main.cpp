#include "daScript/daScript.h"
#include "application/Application.hpp"

const char* tutorial_text = R""""(
[export]
def test
    print("this is nano tutorial\n")
)"""";

int main()
{
    using namespace das;
    // request all da-script built in modules
    NEED_MODULE(Module_BuiltIn);
    // Initialize modules
    das::Module::Initialize();
    // make file access, introduce string as if it was a file
    auto f_access = das::make_smart<das::FsFileAccess>();
    auto file_info =
      std::make_unique<das::TextFileInfo>(tutorial_text, uint32_t(strlen(tutorial_text)), false);
    f_access->setFileInfo("dummy.das", das::move(file_info));
    // compile script
    das::TextPrinter tout;
    das::ModuleGroup dummy_lib_group;
    auto program = compileDaScript("dummy.das", f_access, tout, dummy_lib_group);
    if (program->failed()) {
        return -1;
    }
    // create context
    das::Context ctx(program->getContextStackSize());
    if (!program->simulate(ctx, tout)) {
        return -2;
    }
    // find function. its up to application to check, if function is not null
    auto* function = ctx.findFunction("test");
    if (function == nullptr) {
        return -3;
    }
    // call context function
    ctx.evalWithCatch(function, nullptr);
    // shut-down daScript, free all memory
    das::Module::Shutdown();

    Soldat::Application application;
    application.Run();

    return 0;
}
