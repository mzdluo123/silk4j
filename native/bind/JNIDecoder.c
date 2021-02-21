//
// Created by mzdlu on 2021-02-04.
//

/***********************************************************************
Copyright (c) 2006-2012, Skype Limited. All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, (subject to the limitations in the disclaimer below)
are permitted provided that the following conditions are met:
- Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
- Neither the name of Skype Limited, nor the names of specific
contributors, may be used to endorse or promote products derived from
this software without specific prior written permission.
NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED
BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
CONTRIBUTORS ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************/


/*****************************/
/* Silk decoder test program */
/*****************************/

#include "io_github_mzdluo123_silk4j_SilkCoder.h"
#include "coder.h"


/* Seed for the random number generator, which is used for simulating packet loss */
static SKP_int32 rand_seed = 1;


JNIEXPORT void JNICALL Java_io_github_mzdluo123_silk4j_SilkCoder_decode
        (JNIEnv *env, jclass jclass1, jstring source, jstring dest, jint fs_hz, jint loss) {

    unsigned long tottime, starttime;
    size_t counter;
    SKP_int32 totPackets, i, k;
    SKP_int16 ret, len, tot_len;
    SKP_int16 nBytes;
    SKP_uint8 payload[MAX_BYTES_PER_FRAME * MAX_INPUT_FRAMES * (MAX_LBRR_DELAY + 1)];
    SKP_uint8 *payloadEnd = NULL, *payloadToDec = NULL;
    SKP_uint8 FECpayload[MAX_BYTES_PER_FRAME * MAX_INPUT_FRAMES], *payloadPtr;
    SKP_int16 nBytesFEC;
    SKP_int16 nBytesPerPacket[MAX_LBRR_DELAY + 1], totBytes;
    SKP_int16 out[((FRAME_LENGTH_MS * MAX_API_FS_KHZ) << 1) * MAX_INPUT_FRAMES], *outPtr;
    const char *speechOutFileName, *bitInFileName;
    FILE *bitInFile, *speechOutFile;
    SKP_int32 packetSize_ms = 0, API_Fs_Hz = 0;
    SKP_int32 decSizeBytes;
    void *psDec;
    SKP_float loss_prob;
    SKP_int32 frames, lost, quiet;
    SKP_SILK_SDK_DecControlStruct DecControl;


    /* default settings */

    loss_prob = 0.0f;


    loss_prob = loss;
    API_Fs_Hz = fs_hz;
    bitInFileName = (char *) (*env)->GetStringUTFChars(env, source, NULL);
    speechOutFileName = (char *) (*env)->GetStringUTFChars(env, dest, NULL);


    /* Open files */
    bitInFile = fopen(bitInFileName, "rb");
    /* Check Silk header */
    {
        char header_buf[50];
        fread(header_buf, sizeof(char), 1, bitInFile);
        header_buf[strlen("")] = '\0'; /* Terminate with a null character */
        if (strcmp(header_buf, "") != 0) {
            counter = fread(header_buf, sizeof(char), strlen("!SILK_V3"), bitInFile);
            header_buf[strlen("!SILK_V3")] = '\0'; /* Terminate with a null character */
            if (strcmp(header_buf, "!SILK_V3") != 0) {
                /* Non-equal strings */
//                printf("Error: Wrong Header %s\n", header_buf);
//                exit(0);
                return ;
            }
        } else {
            counter = fread(header_buf, sizeof(char), strlen("#!SILK_V3"), bitInFile);
            header_buf[strlen("#!SILK_V3")] = '\0'; /* Terminate with a null character */
            if (strcmp(header_buf, "#!SILK_V3") != 0) {
                /* Non-equal strings */
//                printf("Error: Wrong Header %s\n", header_buf);
//
                return ;
            }
        }
    }

    speechOutFile = fopen(speechOutFileName, "wb");
    if (speechOutFile == NULL) {
//        printf("Error: could not open output file %s\n", speechOutFileName);
//        exit(0);
        return ;
    }

    /* Set the samplingrate that is requested for the output */
    if (API_Fs_Hz == 0) {
        DecControl.API_sampleRate = 24000;
    } else {
        DecControl.API_sampleRate = API_Fs_Hz;
    }

    /* Initialize to one frame per packet, for proper concealment before first packet arrives */
    DecControl.framesPerPacket = 1;

    /* Create decoder */
    ret = SKP_Silk_SDK_Get_Decoder_Size(&decSizeBytes);
//    if (ret) {
//        printf("\nSKP_Silk_SDK_Get_Decoder_Size returned %d", ret);
//    }
    psDec = malloc(decSizeBytes);

    /* Reset decoder */
    ret = SKP_Silk_SDK_InitDecoder(psDec);
//    if (ret) {
//        printf("\nSKP_Silk_InitDecoder returned %d", ret);
//    }

    totPackets = 0;
    tottime = 0;
    payloadEnd = payload;

    /* Simulate the jitter buffer holding MAX_FEC_DELAY packets */
    for (i = 0; i < MAX_LBRR_DELAY; i++) {
        /* Read payload size */
        counter = fread(&nBytes, sizeof(SKP_int16), 1, bitInFile);
#ifdef _SYSTEM_IS_BIG_ENDIAN
        swap_endian( &nBytes, 1 );
#endif
        /* Read payload */
        counter = fread(payloadEnd, sizeof(SKP_uint8), nBytes, bitInFile);

        if ((SKP_int16) counter < nBytes) {
            break;
        }
        nBytesPerPacket[i] = nBytes;
        payloadEnd += nBytes;
        totPackets++;
    }

    while (1) {
        /* Read payload size */
        counter = fread(&nBytes, sizeof(SKP_int16), 1, bitInFile);
#ifdef _SYSTEM_IS_BIG_ENDIAN
        swap_endian( &nBytes, 1 );
#endif
        if (nBytes < 0 || counter < 1) {
            break;
        }

        /* Read payload */
        counter = fread(payloadEnd, sizeof(SKP_uint8), nBytes, bitInFile);
        if ((SKP_int16) counter < nBytes) {
            break;
        }

        /* Simulate losses */
        rand_seed = SKP_RAND(rand_seed);
        if ((((float) ((rand_seed >> 16) + (1 << 15))) / 65535.0f >= (loss_prob / 100.0f)) && (counter > 0)) {
            nBytesPerPacket[MAX_LBRR_DELAY] = nBytes;
            payloadEnd += nBytes;
        } else {
            nBytesPerPacket[MAX_LBRR_DELAY] = 0;
        }

        if (nBytesPerPacket[0] == 0) {
            /* Indicate lost packet */
            lost = 1;

            /* Packet loss. Search after FEC in next packets. Should be done in the jitter buffer */
            payloadPtr = payload;
            for (i = 0; i < MAX_LBRR_DELAY; i++) {
                if (nBytesPerPacket[i + 1] > 0) {
                    starttime = GetHighResolutionTime();
                    SKP_Silk_SDK_search_for_LBRR(payloadPtr, nBytesPerPacket[i + 1], (i + 1), FECpayload, &nBytesFEC);
                    tottime += GetHighResolutionTime() - starttime;
                    if (nBytesFEC > 0) {
                        payloadToDec = FECpayload;
                        nBytes = nBytesFEC;
                        lost = 0;
                        break;
                    }
                }
                payloadPtr += nBytesPerPacket[i + 1];
            }
        } else {
            lost = 0;
            nBytes = nBytesPerPacket[0];
            payloadToDec = payload;
        }

        /* Silk decoder */
        outPtr = out;
        tot_len = 0;
        starttime = GetHighResolutionTime();

        if (lost == 0) {
            /* No Loss: Decode all frames in the packet */
            frames = 0;
            do {
                /* Decode 20 ms */
                ret = SKP_Silk_SDK_Decode(psDec, &DecControl, 0, payloadToDec, nBytes, outPtr, &len);
//                if (ret) {
//                    printf("\nSKP_Silk_SDK_Decode returned %d", ret);
//                }

                frames++;
                outPtr += len;
                tot_len += len;
                if (frames > MAX_INPUT_FRAMES) {
                    /* Hack for corrupt stream that could generate too many frames */
                    outPtr = out;
                    tot_len = 0;
                    frames = 0;
                }
                /* Until last 20 ms frame of packet has been decoded */
            } while (DecControl.moreInternalDecoderFrames);
        } else {
            /* Loss: Decode enough frames to cover one packet duration */
            for (i = 0; i < DecControl.framesPerPacket; i++) {
                /* Generate 20 ms */
                ret = SKP_Silk_SDK_Decode(psDec, &DecControl, 1, payloadToDec, nBytes, outPtr, &len);
//                if (ret) {
//                    printf("\nSKP_Silk_Decode returned %d", ret);
//                }
                outPtr += len;
                tot_len += len;
            }
        }

        packetSize_ms = tot_len / (DecControl.API_sampleRate / 1000);
        tottime += GetHighResolutionTime() - starttime;
        totPackets++;

        /* Write output to file */
#ifdef _SYSTEM_IS_BIG_ENDIAN
        swap_endian( out, tot_len );
#endif
        fwrite(out, sizeof(SKP_int16), tot_len, speechOutFile);

        /* Update buffer */
        totBytes = 0;
        for (i = 0; i < MAX_LBRR_DELAY; i++) {
            totBytes += nBytesPerPacket[i + 1];
        }
        /* Check if the received totBytes is valid */
        if (totBytes < 0 || totBytes > sizeof(payload)) {
//            fprintf(stderr, "\rPackets decoded:             %d", totPackets);
//            return -1;
            return ;
        }
        SKP_memmove(payload, &payload[nBytesPerPacket[0]], totBytes * sizeof(SKP_uint8));
        payloadEnd -= nBytesPerPacket[0];
        SKP_memmove(nBytesPerPacket, &nBytesPerPacket[1], MAX_LBRR_DELAY * sizeof(SKP_int16));

//        if (!quiet) {
//            fprintf(stderr, "\rPackets decoded:             %d", totPackets);
//        }
    }

    /* Empty the recieve buffer */
    for (k = 0; k < MAX_LBRR_DELAY; k++) {
        if (nBytesPerPacket[0] == 0) {
            /* Indicate lost packet */
            lost = 1;

            /* Packet loss. Search after FEC in next packets. Should be done in the jitter buffer */
            payloadPtr = payload;
            for (i = 0; i < MAX_LBRR_DELAY; i++) {
                if (nBytesPerPacket[i + 1] > 0) {
                    starttime = GetHighResolutionTime();
                    SKP_Silk_SDK_search_for_LBRR(payloadPtr, nBytesPerPacket[i + 1], (i + 1), FECpayload, &nBytesFEC);
                    tottime += GetHighResolutionTime() - starttime;
                    if (nBytesFEC > 0) {
                        payloadToDec = FECpayload;
                        nBytes = nBytesFEC;
                        lost = 0;
                        break;
                    }
                }
                payloadPtr += nBytesPerPacket[i + 1];
            }
        } else {
            lost = 0;
            nBytes = nBytesPerPacket[0];
            payloadToDec = payload;
        }

        /* Silk decoder */
        outPtr = out;
        tot_len = 0;
        starttime = GetHighResolutionTime();

        if (lost == 0) {
            /* No loss: Decode all frames in the packet */
            frames = 0;
            do {
                /* Decode 20 ms */
                ret = SKP_Silk_SDK_Decode(psDec, &DecControl, 0, payloadToDec, nBytes, outPtr, &len);
//                if (ret) {
//                    printf("\nSKP_Silk_SDK_Decode returned %d", ret);
//                }

                frames++;
                outPtr += len;
                tot_len += len;
                if (frames > MAX_INPUT_FRAMES) {
                    /* Hack for corrupt stream that could generate too many frames */
                    outPtr = out;
                    tot_len = 0;
                    frames = 0;
                }
                /* Until last 20 ms frame of packet has been decoded */
            } while (DecControl.moreInternalDecoderFrames);
        } else {
            /* Loss: Decode enough frames to cover one packet duration */

            /* Generate 20 ms */
            for (i = 0; i < DecControl.framesPerPacket; i++) {
                ret = SKP_Silk_SDK_Decode(psDec, &DecControl, 1, payloadToDec, nBytes, outPtr, &len);
//                if (ret) {
//                    printf("\nSKP_Silk_Decode returned %d", ret);
//                }
                outPtr += len;
                tot_len += len;
            }
        }

        packetSize_ms = tot_len / (DecControl.API_sampleRate / 1000);
        tottime += GetHighResolutionTime() - starttime;
        totPackets++;

        /* Write output to file */
#ifdef _SYSTEM_IS_BIG_ENDIAN
        swap_endian( out, tot_len );
#endif
        fwrite(out, sizeof(SKP_int16), tot_len, speechOutFile);

        /* Update Buffer */
        totBytes = 0;
        for (i = 0; i < MAX_LBRR_DELAY; i++) {
            totBytes += nBytesPerPacket[i + 1];
        }

        /* Check if the received totBytes is valid */
        if (totBytes < 0 || totBytes > sizeof(payload)) {

//            fprintf(stderr, "\rPackets decoded:              %d", totPackets);
            return ;
        }

        SKP_memmove(payload, &payload[nBytesPerPacket[0]], totBytes * sizeof(SKP_uint8));
        payloadEnd -= nBytesPerPacket[0];
        SKP_memmove(nBytesPerPacket, &nBytesPerPacket[1], MAX_LBRR_DELAY * sizeof(SKP_int16));

//        if (!quiet) {
//            fprintf(stderr, "\rPackets decoded:              %d", totPackets);
//        }
    }
//
//    if (!quiet) {
//        printf("\nDecoding Finished \n");
//    }

    /* Free decoder */
    free(psDec);

    /* Close files */
    fclose(speechOutFile);
    fclose(bitInFile);

//    filetime = totPackets * 1e-3 * packetSize_ms;
//    if (!quiet) {
//        printf("\nFile length:                 %.3f s", filetime);
//        printf("\nTime for decoding:           %.3f s (%.3f%% of realtime)", 1e-6 * tottime, 1e-4 * tottime / filetime);
//        printf("\n\n");
//    } else {
//        /* print time and % of realtime */
//        printf("%.3f %.3f %d\n", 1e-6 * tottime, 1e-4 * tottime / filetime, totPackets);
//    }
    (*env)->ReleaseStringUTFChars(env, source, bitInFileName);
    (*env)->ReleaseStringUTFChars(env, dest, speechOutFileName);

}
