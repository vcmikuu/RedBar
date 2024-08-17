#include "main.hpp"

#include "scotland2/shared/modloader.h"

#pragma region Mod setup
/// @brief Called at the early stages of game loading
/// @param info
/// @return
MOD_EXPORT_FUNC void setup(CModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;

    Logger.info("Completed setup!");
}

/// @brief Called later on in the game loading - a good time to install function hooks
/// @return
MOD_EXPORT_FUNC void late_load() {
    il2cpp_functions::Init();

    getModConfig().Init(modInfo);
    // BSML::Init();

    Logger.info("Installing hooks...");

    INSTALL_HOOK(Logger, Results);

    Logger.info("Installed all hooks!");
}
#pragma endregion