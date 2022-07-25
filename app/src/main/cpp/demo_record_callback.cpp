//
// Created by hu.xiaotian on 2022/7/25.
//

//
// Created by hu.xiaotian on 2022/7/25.
//

#ifndef AAUDIODEMO_DEMO_RECORD_CALLBACK_H
#define AAUDIODEMO_DEMO_RECORD_CALLBACK_H

#include <jni.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <aaudio/AAudio.h>
#include "AAudioExampleUtils.h"
#include "AAudioSimpleRecorder.h"


#define DEBUG 1
#define LOG_TAG "demo-record"
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#if DEBUG
#define ALOGV(...)  __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#else
#define ALOGV(...)
#endif

bool stopFlag = false;
FILE *saveFile;

// Callback function that fills the audio output buffer.
aaudio_data_callback_result_t DataCallbackProc(
        AAudioStream *stream,
        void *userData,
        void *audioData,
        int32_t numFrames
) {

    // should not happen but just in case...
    if (userData == nullptr) {
        ALOGE("ERROR - SimpleRecorderDataCallbackProc needs userData\n");
        return AAUDIO_CALLBACK_RESULT_STOP;
    }
    PeakTrackerData_t *data = (PeakTrackerData_t *) userData;
    // printf("MyCallbackProc(): frameCount = %d\n", numFrames);
    int32_t samplesPerFrame = AAudioStream_getChannelCount(stream);
    float sample;
    // This code assume mono or stereo.
    switch (AAudioStream_getFormat(stream)) {
        //  case AAUDIO_FORMAT_PCM_I24_PACKED:
        case AAUDIO_FORMAT_PCM_I16: {
            fwrite(audioData, 2, numFrames, saveFile);
        }
            break;
        case AAUDIO_FORMAT_PCM_FLOAT: {
            fwrite(audioData, 4, numFrames, saveFile);
        }
            break;

        case AAUDIO_FORMAT_PCM_I24_PACKED: {
            //  u08 *audioBuffer = (u08 *) audioData;
            fwrite(audioData, 3, numFrames, saveFile);

        }
            break;
        default:
            return AAUDIO_CALLBACK_RESULT_STOP;
    }

    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

void ErrorCallbackProc(
        AAudioStream *stream __unused,
        void *userData __unused,
        aaudio_result_t error) {
    ALOGE("Error Callback, error: %d\n", (int) error);
}


int startRecord(const char *path) {
    if ((saveFile = fopen(path, "a+")) == NULL) {
        ALOGE("cant open the file");
        return -1;
    }

    AAudioArgsParser argParser;
    AAudioSimpleRecorder recorder;
    PeakTrackerData_t myData = {0.0};
    AAudioStream *aaudioStream = nullptr;
    aaudio_result_t result;
    aaudio_stream_state_t state;

    int loopsNeeded = 0;
    const int displayRateHz = 20; // arbitrary

    // Make printf print immediately so that debug info is not stuck
    // in a buffer if we hang or crash.
    setvbuf(stdout, nullptr, _IONBF, (size_t) 0);
    // printf("%s - Display audio input using an AAudio callback, V0.1.3\n", argv[0]);

//    if (argParser.parseArgs(argc, argv)) {
//        return EXIT_FAILURE;
//    }

    argParser.setDeviceId(0);
    //r
    argParser.setSampleRate(16 * 1000); //48*1000
    //f
    argParser.setFormat(AAUDIO_FORMAT_PCM_I16);//AAUDIO_FORMAT_PCM_I24_PACKED
    //c
    argParser.setChannelCount(1);//16
    //i
    argParser.setInputPreset(6);
    //s
    argParser.setDurationSeconds(3600);

    result = recorder.open(argParser,
                           DataCallbackProc,
                           ErrorCallbackProc,
                           &myData);
    if (result != AAUDIO_OK) {
        ALOGE("ERROR -  recorder.open() returned %d\n", result);
        ALOGE("IMPORTANT - Did you remember to enter:   adb root\n");
        goto error;
    }
    aaudioStream = recorder.getStream();
    argParser.compareWithStream(aaudioStream);

    ALOGE("recorder.getFramesPerSecond() = %d\n", recorder.getFramesPerSecond());
    ALOGE("recorder.getSamplesPerFrame() = %d\n", recorder.getSamplesPerFrame());

    result = recorder.start();
    if (result != AAUDIO_OK) {
        ALOGE("ERROR -  recorder.start() returned %d\n", result);
        goto error;
    }

    ALOGE("Sleep for %d seconds while audio record in a callback thread.\n",
          argParser.getDurationSeconds());
    while (!stopFlag) {
        const struct timespec request = {.tv_sec = 0,
                .tv_nsec = NANOS_PER_SECOND / displayRateHz};
        (void) clock_nanosleep(CLOCK_MONOTONIC, 0 /*flags*/, &request, NULL /*remain*/);
        ALOGE("%08d: ", (int) recorder.getFramesRead());
        displayPeakLevel(myData.peakLevel);

        result = AAudioStream_waitForStateChange(aaudioStream,
                                                 AAUDIO_STREAM_STATE_CLOSED,
                                                 &state,
                                                 0);
        if (result != AAUDIO_OK) {
            ALOGE("ERROR - AAudioStream_waitForStateChange() returned %d\n", result);
            goto error;
        }
        if (state != AAUDIO_STREAM_STATE_STARTING && state != AAUDIO_STREAM_STATE_STARTED) {
            ALOGE("Stream state is %d %s!\n", state, AAudio_convertStreamStateToText(state));
            break;
        }
    }

    ALOGE("Woke up. Stop for a moment.\n");

    result = recorder.stop();
    if (result != AAUDIO_OK) {
        goto error;
    }
    usleep(2000 * 1000);
    result = recorder.start();
    if (result != AAUDIO_OK) {
        fprintf(stderr, "ERROR -  recorder.start() returned %d\n", result);
        goto error;
    }

    ALOGE("Sleep for %d seconds while audio records in a callback thread.\n",
          argParser.getDurationSeconds());
    while (!stopFlag) {
        const struct timespec request = {.tv_sec = 0,
                .tv_nsec = NANOS_PER_SECOND / displayRateHz};
        (void) clock_nanosleep(CLOCK_MONOTONIC, 0 /*flags*/, &request, NULL /*remain*/);
        printf("%08d: ", (int) recorder.getFramesRead());
        displayPeakLevel(myData.peakLevel);

        state = AAudioStream_getState(aaudioStream);
        if (state != AAUDIO_STREAM_STATE_STARTING && state != AAUDIO_STREAM_STATE_STARTED) {
            printf("Stream state is %d %s!\n", state, AAudio_convertStreamStateToText(state));
            break;
        }
    }
    ALOGE("Woke up now.\n");
    argParser.compareWithStream(aaudioStream);

    stopFlag = false;
    fclose(saveFile);
    result = recorder.stop();
    if (result != AAUDIO_OK) {
        goto error;
    }
    result = recorder.close();
    if (result != AAUDIO_OK) {
        goto error;
    }

    ALOGE("SUCCESS\n");
    return EXIT_SUCCESS;
    error:
    recorder.close();
    ALOGE("exiting - AAudio result = %d = %s\n", result, AAudio_convertResultToText(result));
    return EXIT_FAILURE;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_xiaomi_aaudiodemo_TestJni_startReocrd(JNIEnv *env, jclass clazz, jstring saveFileName) {
    ALOGE("111111");
    startRecord(env->GetStringUTFChars(saveFileName, JNI_FALSE));
}


#endif //AAUDIODEMO_DEMO_RECORD_CALLBACK_H


extern "C"
JNIEXPORT void JNICALL
Java_com_xiaomi_aaudiodemo_TestJni_stopReocrd(JNIEnv *env, jclass clazz) {
    stopFlag = true;
}