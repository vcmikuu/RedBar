#include "main.hpp"
#include "config.hpp"

#include "GlobalNamespace/GameEnergyUIPanel.hpp"
#include "GlobalNamespace/GameEnergyCounter.hpp"
// #include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/OverrideEnvironmentSettings.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/PracticeSettings.hpp"
#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/MultiplayerLevelScenesTransitionSetupDataSO.hpp"
// #include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"

#include "beatsaber-hook/shared/utils/typedefs.h"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-type-check.hpp"

#include "UnityEngine/UI/Image.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"

#include "custom-types/shared/register.hpp"

#include "bsml/shared/BSML.hpp"

#include "scotland2/shared/modloader.h"


void SetColor(GameEnergyUIPanel* self, float energy) {
    UnityEngine::Color color;

    color = getModConfig().Defhp.GetValue();
    color.a = getModConfig().Alpha.GetValue();
    
    UnityEngine::UI::Image* energyBar = self->energyBar;
    UnityEngine::Material* energyBarMaterial = energyBar->get_material();

    if (energyBarMaterialStore != energyBarMaterial || energyBarStore != energyBar) {
        energyBarMaterialStore = energyBarMaterial;
        energyBarStore = energyBar;
    }
    if (energy != energyy) {
        energyy = energy;
    }

    if(getModConfig().FullFade.GetValue()) {
        double fraction = energy;
        color.r = getModConfig().Diehp.GetValue().r + (getModConfig().Highhp.GetValue().r - getModConfig().Diehp.GetValue().r) * fraction;
        color.g = getModConfig().Diehp.GetValue().g + (getModConfig().Highhp.GetValue().g - getModConfig().Diehp.GetValue().g) * fraction;
        color.b = getModConfig().Diehp.GetValue().b + (getModConfig().Highhp.GetValue().b - getModConfig().Diehp.GetValue().b) * fraction;
    } else {
        if (energy < 0.15) {
            if (!getModConfig().AlwaysRainbow.GetValue()) {
                color.r = getModConfig().Diehp.GetValue().r;
                color.g = getModConfig().Diehp.GetValue().g;
                color.b = getModConfig().Diehp.GetValue().b;
            }
        } else if (energy < 0.5) {
            if (!getModConfig().AlwaysRainbow.GetValue()) {
                double fraction = (energy - 0.15f)*2.8571428f;
                color.r = getModConfig().Diehp.GetValue().r + (getModConfig().Lowhp.GetValue().r - getModConfig().Diehp.GetValue().r) * fraction;
                color.g = getModConfig().Diehp.GetValue().g + (getModConfig().Lowhp.GetValue().g - getModConfig().Diehp.GetValue().g) * fraction;
                color.b = getModConfig().Diehp.GetValue().b + (getModConfig().Lowhp.GetValue().b - getModConfig().Diehp.GetValue().b) * fraction;
            }
        } else if (energy > 0.5 && energy < 0.7) {
            if (!getModConfig().AlwaysRainbow.GetValue()) {
                double fraction = (energy - 0.5f)*5.0f;
                color.r = getModConfig().Lowhp.GetValue().r + (getModConfig().Defhp.GetValue().r - getModConfig().Lowhp.GetValue().r) * fraction;
                color.g = getModConfig().Lowhp.GetValue().g + (getModConfig().Defhp.GetValue().g - getModConfig().Lowhp.GetValue().g) * fraction;
                color.b = getModConfig().Lowhp.GetValue().b + (getModConfig().Defhp.GetValue().b - getModConfig().Lowhp.GetValue().b) * fraction;
            }
        } else if (energy > 0.95) {
            if (!getModConfig().Rainbow.GetValue()) {
                double fraction = (energy - 0.95f)*20.0f;
                color.r = getModConfig().Midhp.GetValue().r + (getModConfig().Highhp.GetValue().r - getModConfig().Midhp.GetValue().r) * fraction;
                color.g = getModConfig().Midhp.GetValue().g + (getModConfig().Highhp.GetValue().g - getModConfig().Midhp.GetValue().g) * fraction;
                color.b = getModConfig().Midhp.GetValue().b + (getModConfig().Highhp.GetValue().b - getModConfig().Midhp.GetValue().b) * fraction;
            }
        } else if (energy > 0.7) {
            if (!getModConfig().AlwaysRainbow.GetValue()) {
                double fraction = (energy - 0.7f)*3.3333333f;
                color.r = getModConfig().Defhp.GetValue().r + (getModConfig().Midhp.GetValue().r - getModConfig().Defhp.GetValue().r) * fraction;
                color.g = getModConfig().Defhp.GetValue().g + (getModConfig().Midhp.GetValue().g - getModConfig().Defhp.GetValue().g) * fraction;
                color.b = getModConfig().Defhp.GetValue().b + (getModConfig().Midhp.GetValue().b - getModConfig().Defhp.GetValue().b) * fraction;
            }
        }
    }
    
    if (energy > 0.85 && getModConfig().Fadeout.GetValue()) {
        color.a = (6.667+(-6.667*energy))*getModConfig().Alpha.GetValue();
    } else {
        color.a = getModConfig().Alpha.GetValue();
    }
    energyBar->set_color(color);
}


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

    Logger.info("Installed all hooks!");
}
#pragma endregion