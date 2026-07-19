#include "HM_Intro.h"
#include "TrackBrowser.h"
#include "fast/interpreter.h"
// #include <cstdlib>
#include "port/Engine.h"

extern "C" {
#include "main.h"
#include "menu_items.h"
#include "menus.h"
#include "external.h"
#include "defines.h"
#include "code_800029B0.h" 
// #include "audio/external.h"
}

HarbourMastersIntro::HarbourMastersIntro() {}

static void PSC_PrewarmShaders(void) {
    auto interpreter = GetInterpreter();
    if (interpreter == nullptr) return;

    struct ExactWarmup {
        uint64_t mode; 
        uint8_t opt;
        uint64_t v0;
        uint64_t v1;
    };

    static const ExactWarmup list[] = {
        // --- System & Global UI ---
        {0xe4f3000e4f3ULL, 0, 3489661441ULL, 0},
        {0x3ff3fff3ff3fffULL, 1, 2147516416ULL, 769},
        {0x3ff3fff3ff3fffULL, 1, 2147516416ULL, 1},
        {0x00002000ULL, 128, 3489693696ULL, 128},
        {0xef99fffef99fffULL, 1, 17305600ULL, 769},
        {0xef9e3f1ef9e3f1ULL, 1, 17301768ULL, 1},
        {0x3ffe5f13ffe5f1ULL, 1, 2147483912ULL, 1},
        {0xf399ffff399fffULL, 1, 17305600ULL, 769},
        {0xf399ffff399fffULL, 1, 3489693696ULL, 768},
        {0xf399ffff399fffULL, 1, 17301768ULL, 513},

        // --- Starting up Mushroom cup with Luigi Raceway ---
//        {0xf39e4f1f39e4f1ULL, 5, 17301768ULL, 5},
//        {0x3ff3fff3ff3fffULL, 5, 2147516416ULL, 5},
        {0xe7b6156e7b6156ULL, 33, 134293532ULL, 33},
        {0xe7b6156e7b6156ULL, 5, 134293532ULL, 5},
        {0xef97fffef97fffULL, 1, 17305600ULL, 1},
        {0xef9a153ef9a153ULL, 1, 17311777ULL, 1},
        {0x02002000ULL, 37, 2147516416ULL, 805},
        {0xe7b6156e7b6156ULL, 1, 134293532ULL, 769},
        {0xe7b6156e7b6156ULL, 1, 2147516416ULL, 773},
        {0xe7b6156e7b6156ULL, 1, 17301768ULL, 769},

        // --- while in Luigi Raceway ---
        {0xef9a153ef9a153ULL, 9, 17311777ULL, 9},
        {0xf39e4f1f39e4f1ULL, 33, 17301768ULL, 33},
        {0xef9e3f1ef9e3f1ULL, 5, 17301768ULL, 5},
        {0x3ffe4f13ffe4f1ULL, 1, 2147483912ULL, 1},
        {0x3ff7fff3ff7fffULL, 1, 2147487744ULL, 1},

        // --- farm and beach ---
//        {0x1fff000e4f1ULL, 18, 14988208261797249288ULL, 18},
        {0xf39e4f1f39e4f1ULL, 1, 17301768ULL, 1},
        {0xef9e3f1ef9e3f1ULL, 9, 17301768ULL, 9},

        // --- Flower Cup: Toad's Turnpike ---


        // --- Stage Specific Variants ---

    };

    constexpr int count = sizeof(list) / sizeof(ExactWarmup);

    for (int i = 0; i < count; i++) {
        ColorCombinerKey fakeKey;
        fakeKey.combine_mode = list[i].mode;
        fakeKey.options = list[i].opt;
        
        Fast::ColorCombiner* comb = &interpreter->mColorCombinerPool[fakeKey];
        interpreter->GenerateCC(comb, fakeKey);
        interpreter->LookupOrCreateShaderProgram(list[i].v0, list[i].v1);
    }
//    printf("[PSC-SYSTEM] GLES Context: %d Shaders successfully pre-warmed.\n", count);
}

void HarbourMastersIntro::HM_InitIntro() {
    // Run pre-warm once on boot to prevent GLES stutter
    PSC_PrewarmShaders();
}

void HarbourMastersIntro::HM_TickIntro() {
    static bool jumpTriggered = false;

    // This runs every frame, but the flag stops it from looping
    if (!jumpTriggered) {

    //    system("killall splash");
        system("killall -9 splash");
        
        // 1. Force the TrackBrowser to the target
        TrackBrowser_SetTrack("hm:test_track"); 
        
        // 2. Sync the actual course ID variable
        gCurrentCourseId = TrackBrowser_GetTrackIndex(); 

        // 3. Mirror the splash_menu_act variables EXACTLY
        gDebugMenuSelection = DEBUG_MENU_OPTION_SELECTED; 
        gDebugGotoScene = 0;   // Scene 0 = Racing
        gPlayerCount = 1;      // 1 Player
        gModeSelection = 2;    // Time Trial mode

        // --- PSC BOOT FLAG ---
        // We set this CVar so TestTrack::BeginPlay knows this is the 
        // initial GLES warmup and not a manual player selection.
        CVarSetInteger("gDisableLod", 0);
        CVarSetInteger("gPSCIsBooting", 1);
        // ---------------------

        // 4. Trigger the hardware transition calls
        func_8009E1C0();     // Fade/Stop Music
        func_800CA330(0x19); // Jump to Scene (0x19 = 25)

//        play_sound2(SOUND_INTRO_ENTER_MENU);
        
//        printf("[BOOT] PSC Warmup Sequence Started. Loading: %d\n", gCurrentCourseId);

        jumpTriggered = true;
        return; 
    }
}

void HarbourMastersIntro::HM_DrawIntro() {
    // Keep frame empty during the force-load
}
