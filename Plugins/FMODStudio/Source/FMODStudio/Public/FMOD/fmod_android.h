#ifndef _FMOD_ANDROID_H
#define _FMOD_ANDROID_H

/*
[ENUM]
[
    [DESCRIPTION]
    Cores available for mapping threads onto.

    [REMARKS]

    [SEE_ALSO]
    FMOD_ANDROID_THREADAFFINITY
]
*/
typedef unsigned int FMOD_THREAD;

#define FMOD_THREAD_DEFAULT                0         /* Use default thread assignment. */
#define FMOD_THREAD_CORE0                  (1 << 0)
#define FMOD_THREAD_CORE1                  (1 << 1)
#define FMOD_THREAD_CORE2                  (1 << 2)
#define FMOD_THREAD_CORE3                  (1 << 3)
#define FMOD_THREAD_CORE4                  (1 << 4)
#define FMOD_THREAD_CORE5                  (1 << 5)
#define FMOD_THREAD_CORE6                  (1 << 6)
#define FMOD_THREAD_CORE7                  (1 << 7)


/*
[STRUCTURE]
[
    [DESCRIPTION]
    Mapping of cores to threads.

    [REMARKS]

    [SEE_ALSO]
    FMOD_THREAD
    FMOD_Android_SetThreadAffinity
]
*/
typedef struct FMOD_ANDROID_THREADAFFINITY
{
    FMOD_THREAD mixer;             /* Software mixer thread. */
    FMOD_THREAD stream;            /* Stream thread. */
    FMOD_THREAD nonblocking;       /* Asynchronous sound loading thread. */
    FMOD_THREAD file;              /* File thread. */
    FMOD_THREAD geometry;          /* Geometry processing thread. */
    FMOD_THREAD profiler;          /* Profiler threads. */
    FMOD_THREAD studioUpdate;      /* Studio update thread. */
    FMOD_THREAD studioLoadBank;    /* Studio bank loading thread. */
    FMOD_THREAD studioLoadSample;  /* Studio sample loading thread. */
} FMOD_ANDROID_THREADAFFINITY;


/*
[API]
[
    [DESCRIPTION]
    Control which core particular FMOD threads are created on.

    [PARAMETERS]
    'affinity'    Pointer to a structure that describes the affinity for each thread.

    [REMARKS]
    Call before System::init or affinity values will not apply.

    [SEE_ALSO]
    FMOD_ANDROID_THREADAFFINITY
]
*/
extern "C" FMOD_RESULT F_API FMOD_Android_SetThreadAffinity(FMOD_ANDROID_THREADAFFINITY *affinity);

#endif /* _FMOD_ANDROID_H */
