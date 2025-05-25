#include <SDL2/SDL.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/channel_layout.h>
#include <libavutil/frame.h>
#include <libavutil/samplefmt.h>

#define SDL_AUDIO_BUFFER_SIZE 1024

typedef struct AudioData {
    uint8_t *pos;
    uint32_t length;
} AudioData;

void audio_callback(void *userdata, Uint8 *stream, int len) {
    AudioData *audio = (AudioData *)userdata;

    if (audio->length == 0)
        return;

    len = (len > audio->length) ? audio->length : len;
    SDL_memcpy(stream, audio->pos, len);

    audio->pos += len;
    audio->length -= len;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        return -1;
    }

    const char *input_file = argv[1];

    // Initialize FFmpeg library
    avformat_network_init();

    AVFormatContext *format_ctx = NULL;

    // Open input file
    if (avformat_open_input(&format_ctx, input_file, NULL, NULL) < 0) {
        fprintf(stderr, "Could not open input file: %s\n", input_file);
        return -1;
    }

    // Retrieve stream information
    if (avformat_find_stream_info(format_ctx, NULL) < 0) {
        fprintf(stderr, "Could not find stream information.\n");
        avformat_close_input(&format_ctx);
        return -1;
    }

    // Find the best audio stream
    int audio_stream_index = av_find_best_stream(format_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (audio_stream_index < 0) {
        fprintf(stderr, "Could not find an audio stream in the input file.\n");
        avformat_close_input(&format_ctx);
        return -1;
    }

    AVStream *audio_stream = format_ctx->streams[audio_stream_index];

    // Find decoder for the audio stream
    AVCodec *decoder = avcodec_find_decoder(audio_stream->codecpar->codec_id);
    if (!decoder) {
        fprintf(stderr, "Failed to find decoder for the audio stream.\n");
        avformat_close_input(&format_ctx);
        return -1;
    }

    // Allocate codec context
    AVCodecContext *codec_ctx = avcodec_alloc_context3(decoder);
    if (!codec_ctx) {
        fprintf(stderr, "Failed to allocate codec context.\n");
        avformat_close_input(&format_ctx);
        return -1;
    }

    // Copy codec parameters from input stream to codec context
    if (avcodec_parameters_to_context(codec_ctx, audio_stream->codecpar) < 0) {
        fprintf(stderr, "Failed to copy codec parameters to codec context.\n");
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return -1;
    }

    // Open codec
    if (avcodec_open2(codec_ctx, decoder, NULL) < 0) {
        fprintf(stderr, "Failed to open codec.\n");
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return -1;
    }

    // Initialize SDL
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return -1;
    }

    // Set up SDL Audio
    SDL_AudioSpec wanted_spec, obtained_spec;
    AudioData audio_data = {0};

    wanted_spec.freq = codec_ctx->sample_rate;
    wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.channels = codec_ctx->channels;
    wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;
    wanted_spec.callback = audio_callback;
    wanted_spec.userdata = &audio_data;

    if (SDL_OpenAudio(&wanted_spec, &obtained_spec) < 0) {
        fprintf(stderr, "Could not open SDL audio: %s\n", SDL_GetError());
        SDL_Quit();
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return -1;
    }

    // Create SDL Window
    SDL_Window *window = SDL_CreateWindow("Now Playing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
    if (!window) {
        fprintf(stderr, "Could not create SDL window: %s\n", SDL_GetError());
        SDL_CloseAudio();
        SDL_Quit();
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return -1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Could not create SDL renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_CloseAudio();
        SDL_Quit();
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return -1;
    }

    // Start playing audio
    SDL_PauseAudio(0);

    // Allocate frame and packet
    AVFrame *frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Failed to allocate frame.\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_CloseAudio();
        SDL_Quit();
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return -1;
    }

    AVPacket packet;

    // Read and decode audio frames
    while (av_read_frame(format_ctx, &packet) >= 0) {
        if (packet.stream_index == audio_stream_index) {
            if (avcodec_send_packet(codec_ctx, &packet) >= 0) {
                while (avcodec_receive_frame(codec_ctx, frame) >= 0) {
                    int data_size = av_samples_get_buffer_size(NULL, codec_ctx->channels, frame->nb_samples, codec_ctx->sample_fmt, 1);
                    audio_data.pos = frame->data[0];
                    audio_data.length = data_size;

                    // Wait for the audio to finish
                    while (audio_data.length > 0) {
                        SDL_Delay(1);
                    }
                }
            }
        }
        av_packet_unref(&packet);

        // Render song info
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        // Add code here to display song info (e.g., using SDL_ttf for text)
        SDL_RenderPresent(renderer);
    }

    // Free resources
    av_frame_free(&frame);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_CloseAudio();
    SDL_Quit();
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&format_ctx);

    // Clean up FFmpeg library
    avformat_network_deinit();

    return 0;
}

