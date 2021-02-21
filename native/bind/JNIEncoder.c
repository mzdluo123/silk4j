//
// Created by mzdlu on 2021-02-05.
//

#include "coder.h"
#include "io_github_mzdluo123_silk4j_SilkCoder.h"


JNIEXPORT void JNICALL Java_io_github_mzdluo123_silk4j_SilkCoder_encode
        (JNIEnv *env, jclass cls, jstring source, jstring dest, jboolean tencent, jint fs_Hz,
         jint maxInternalSampleRate,
         jint packetSize,
         jint packetLossPercentage,
         jint useInBandFEC,
         jint useDTX,
         jint complexity,
         jint bitRate) {
    unsigned long tottime, starttime;
    double filetime;
    size_t counter;
    SKP_int32 k, args, totPackets, totActPackets, ret;
    SKP_int16 nBytes;
    double sumBytes, sumActBytes, avg_rate, act_rate, nrg;
    SKP_uint8 payload[MAX_BYTES_PER_FRAME * MAX_INPUT_FRAMES];
    SKP_int16 in[FRAME_LENGTH_MS * MAX_API_FS_KHZ * MAX_INPUT_FRAMES];
    char *speechInFileName, *bitOutFileName;
    FILE *bitOutFile, *speechInFile;
    SKP_int32 encSizeBytes;
    void *psEnc;
#ifdef _SYSTEM_IS_BIG_ENDIAN
    SKP_int16 nBytes_LE;
#endif

    /* default settings */
    SKP_int32 API_fs_Hz = fs_Hz;
    SKP_int32 max_internal_fs_Hz = 0;
    SKP_int32 targetRate_bps = 25000;
    SKP_int32 smplsSinceLastPacket, packetSize_ms = 20;
    SKP_int32 frameSizeReadFromFile_ms = 20;
    SKP_int32 packetLoss_perc = 0;
#if LOW_COMPLEXITY_ONLY
    SKP_int32 complexity_mode = 0;
#else
    SKP_int32 complexity_mode = 2;
#endif
    SKP_int32 DTX_enabled = 0, INBandFEC_enabled = 0, quiet = 0;
    SKP_SILK_SDK_EncControlStruct encControl; // Struct for input to encoder
    SKP_SILK_SDK_EncControlStruct encStatus;  // Struct for status of encoder

    speechInFileName = (char *) (*env)->GetStringUTFChars(env, source, NULL);
    bitOutFileName = (char *) (*env)->GetStringUTFChars(env, dest, NULL);


    /* Open files */
    speechInFile = fopen(speechInFileName, "rb");
    if (speechInFile == NULL) {
//        printf("Error: could not open input file %s\n", speechInFileName);
//        exit(0);
        (*env)->ReleaseStringUTFChars(env, source, speechInFileName);
        (*env)->ReleaseStringUTFChars(env, dest, bitOutFileName);
        return;
    }

    bitOutFile = fopen(bitOutFileName, "wb");
    if (bitOutFile == NULL) {
//        printf("Error: could not open output file %s\n", bitOutFileName);
//        exit(0);
        (*env)->ReleaseStringUTFChars(env, source, speechInFileName);
        (*env)->ReleaseStringUTFChars(env, dest, bitOutFileName);
        return;
    }

    /* Add Silk header to stream */
    {
        if (tencent) {
            static const char Tencent_break[] = "";
            fwrite(Tencent_break, sizeof(char), strlen(Tencent_break), bitOutFile);
        }

        static const char Silk_header[] = "#!SILK_V3";
        fwrite(Silk_header, sizeof(char), strlen(Silk_header), bitOutFile);
    }

    /* Create Encoder */
    ret = SKP_Silk_SDK_Get_Encoder_Size(&encSizeBytes);
    if (ret) {
//        printf("\nError: SKP_Silk_create_encoder returned %d\n", ret);
//        exit(0);
        return;
    }

    psEnc = malloc(encSizeBytes);

    /* Reset Encoder */
    ret = SKP_Silk_SDK_InitEncoder(psEnc, &encStatus);
    if (ret) {
//        printf("\nError: SKP_Silk_reset_encoder returned %d\n", ret);
//        exit(0);
        return;
    }

    /* Set Encoder parameters */
    encControl.API_sampleRate = API_fs_Hz;
    encControl.maxInternalSampleRate = maxInternalSampleRate;
    encControl.packetSize = packetSize;
    encControl.packetLossPercentage = packetLossPercentage;
    encControl.useInBandFEC = useInBandFEC;
    encControl.useDTX = useDTX;
    encControl.complexity = complexity;
    encControl.bitRate = (bitRate > 0 ? bitRate : 0);

    if (API_fs_Hz > MAX_API_FS_KHZ * 1000 || API_fs_Hz < 0) {
//        printf("\nError: API sampling rate = %d out of range, valid range 8000 - 48000 \n \n", API_fs_Hz);
//        exit(0);
        return;
    }

    tottime = 0;
    totPackets = 0;
    totActPackets = 0;
    smplsSinceLastPacket = 0;
    sumBytes = 0.0;
    sumActBytes = 0.0;
    smplsSinceLastPacket = 0;

    while (1) {
        /* Read input from file */
        counter = fread(in, sizeof(SKP_int16), (frameSizeReadFromFile_ms * API_fs_Hz) / 1000, speechInFile);
#ifdef _SYSTEM_IS_BIG_ENDIAN
        swap_endian( in, counter );
#endif
        if ((SKP_int) counter < ((frameSizeReadFromFile_ms * API_fs_Hz) / 1000)) {
            break;
        }

        /* max payload size */
        nBytes = MAX_BYTES_PER_FRAME * MAX_INPUT_FRAMES;

        starttime = GetHighResolutionTime();

        /* Silk Encoder */
        ret = SKP_Silk_SDK_Encode(psEnc, &encControl, in, (SKP_int16) counter, payload, &nBytes);
//        if (ret) {
//            printf("\nSKP_Silk_Encode returned %d", ret);
//        }

        tottime += GetHighResolutionTime() - starttime;

        /* Get packet size */
        packetSize_ms = (SKP_int) ((1000 * (SKP_int32) encControl.packetSize) / encControl.API_sampleRate);

        smplsSinceLastPacket += (SKP_int) counter;

        if (((1000 * smplsSinceLastPacket) / API_fs_Hz) == packetSize_ms) {
            /* Sends a dummy zero size packet in case of DTX period  */
            /* to make it work with the decoder test program.        */
            /* In practice should be handled by RTP sequence numbers */
            totPackets++;
            sumBytes += nBytes;
            nrg = 0.0;
            for (k = 0; k < (SKP_int) counter; k++) {
                nrg += in[k] * (double) in[k];
            }
            if ((nrg / (SKP_int) counter) > 1e3) {
                sumActBytes += nBytes;
                totActPackets++;
            }

            /* Write payload size */
#ifdef _SYSTEM_IS_BIG_ENDIAN
            nBytes_LE = nBytes;
            swap_endian( &nBytes_LE, 1 );
            fwrite( &nBytes_LE, sizeof( SKP_int16 ), 1, bitOutFile );
#else
            fwrite(&nBytes, sizeof(SKP_int16), 1, bitOutFile);
#endif

            /* Write payload */
            fwrite(payload, sizeof(SKP_uint8), nBytes, bitOutFile);

            smplsSinceLastPacket = 0;

//            if (!quiet) {
//                fprintf(stderr, "\rPackets encoded:                %d", totPackets);
//            }
        }
    }

    /* Write dummy because it can not end with 0 bytes */
    nBytes = -1;

    /* Write payload size */
    if (!tencent) {
        fwrite(&nBytes, sizeof(SKP_int16), 1, bitOutFile);
    }

    /* Free Encoder */
    free(psEnc);

    fclose(speechInFile);
    fclose(bitOutFile);

//    filetime = totPackets * 1e-3 * packetSize_ms;
//    avg_rate = 8.0 / packetSize_ms * sumBytes / totPackets;
//    act_rate = 8.0 / packetSize_ms * sumActBytes / totActPackets;
//    if (!quiet) {
//        printf("\nFile length:                    %.3f s", filetime);
//        printf("\nTime for encoding:              %.3f s (%.3f%% of realtime)", 1e-6 * tottime,
//               1e-4 * tottime / filetime);
//        printf("\nAverage bitrate:                %.3f kbps", avg_rate);
//        printf("\nActive bitrate:                 %.3f kbps", act_rate);
//        printf("\n\n");
//    } else {
//        /* print time and % of realtime */
//        printf("%.3f %.3f %d ", 1e-6 * tottime, 1e-4 * tottime / filetime, totPackets);
//        /* print average and active bitrates */
//        printf("%.3f %.3f \n", avg_rate, act_rate);
//    }

    (*env)->ReleaseStringUTFChars(env, source, speechInFileName);
    (*env)->ReleaseStringUTFChars(env, dest, bitOutFileName);

}
