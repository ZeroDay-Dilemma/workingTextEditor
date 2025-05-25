#include <SDL2/SDL.h>
#include <libavformat/avformat.h>

const char* stream_url = "https://streams.radio.co/s7a5fb9da6/listen";

// Audio callback function
void audio_callback(void* userdata, Uint8* stream, int len) {
    AVCodecContext* codec_ctx = (AVCodecContext*)userdata;
    static AVPacket packet;
    static AVFrame* frame = av_frame_alloc();
    static int packet_data_offset = 0;

    while (len > 0) {
        if (packet_data_offset >= packet.size) {
            // Get the next packet
            if (av_read_frame((AVFormatContext*)codec_ctx->opaque, &packet) < 0) break;
            packet_data_offset = 0;
        }

        // Decode audio packet
        int decoded_len = avcodec_decode_audio4(codec_ctx, frame, &packet_data_offset, &packet);
        if (decoded_len < 0) break;

        // Copy decoded audio to SDL stream
        int bytes_to_copy = SDL_min(len, frame->linesize[0]);
        memcpy(stream, frame->data[0], bytes_to_copy);
        len -= bytes_to_copy;
        stream += bytes_to_copy;
        packet_data_offset += bytes_to_copy;
    }
}

int main() {
    // Initialize SDL2
    if (SDL_Init(SDL_INIT_AUDIO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize FFmpeg
    av_register_all();
    avformat_network_init();

    AVFormatContext* format_ctx = nullptr;
    if (avformat_open_input(&format_ctx, stream_url, nullptr, nullptr) < 0) {
        printf("Failed to open stream\n");
        return 1;
    }

    // Find the best stream
    int audio_stream_index = av_find_best_stream(format_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (audio_stream_index < 0) {
        printf("Failed to find audio stream\n");
        return 1;
    }

    AVStream* audio_stream = format_ctx->streams[audio_stream_index];
    AVCodec* codec = avcodec_find_decoder(audio_stream->codecpar->codec_id);
    AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_ctx, audio_stream->codecpar);
    codec_ctx->opaque = format_ctx;

    if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
        printf("Failed to open codec\n");
        return 1;
    }

    // SDL audio setup
    SDL_AudioSpec spec;
    SDL_zero(spec);
    spec.freq = codec_ctx->sample_rate;
    spec.format = AUDIO_F32;
    spec.channels = codec_ctx->channels;
    spec.samples = 1024;
    spec.callback = audio_callback;
    spec.userdata = codec_ctx;

    if (SDL_OpenAudio(&spec, nullptr) < 0) {
        printf("Failed to open audio: %s\n", SDL_GetError());
        return 1;
    }

    SDL_PauseAudio(0); // Start audio playback

    // Keep running until the user quits
    SDL_Delay(10000); // Play for 10 seconds

    // Cleanup
    SDL_CloseAudio();
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&format_ctx);
    SDL_Quit();

    return 0;
}

