#ifndef PROFILER_H
#define PROFILER_H

extern u32 osClockRate;

struct ProfilerFrameData {
    /* 0x00 */ s16 numSoundTimes;
    /* 0x02 */ s16 numVblankTimes;
    /* 0x04 */ s32 padding;          // Add this to keep the 0x08 offset correct
    // gameTimes:
    // 0: thread 5 start
    // 1: level script execution
    // 2: render
    // 3: display lists
    // 4: thread 4 end (0 terminated)
    /* 0x08 */ u32 gameTimes[5];     // Force to u32
    // gfxTimes:
    // 0: processors queued
    // 1: rsp completed
    // 2: rdp completed
    /* 0x1C */ u32 gfxTimes[3];      // Corrected offset for 32-bit
    /* 0x28 */ u32 soundTimes[8];    // Corrected offset for 32-bit
    /* 0x48 */ u32 vblankTimes[8];   // Corrected offset for 32-bit
};

// thread event IDs
enum ProfilerGameEvent { THREAD5_START, LEVEL_SCRIPT_EXECUTE, BEFORE_DISPLAY_LISTS, AFTER_DISPLAY_LISTS, THREAD5_END };

enum ProfilerGfxEvent { TASKS_QUEUED, RSP_COMPLETE, RDP_COMPLETE };

void profiler_log_thread5_time(enum ProfilerGameEvent eventID);
void profiler_log_thread4_time(void);
void profiler_log_gfx_time(enum ProfilerGfxEvent eventID);
void profiler_log_vblank_time(void);
void draw_profiler(void);
void resource_display(void);

extern s32 gEnableResourceMeters;

#endif /* PROFILER_H */

