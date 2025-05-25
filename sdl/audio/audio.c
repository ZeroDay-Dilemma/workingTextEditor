#include <SDL2/SDL.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/frame.h>



// Callback for audio
void audio_callback(void* userdata, Uint8* stream, int len) {
    AVCodecContext* codec_ctx = (AVCodecContext*)userdata;

    // Dummy example of how to process audio data
    memset(stream, 0, len); // Clear the audio buffer (silence)
}

int main() {
    // Initialize FFmpeg
    avformat_network_init();

    // Open the stream
    AVFormatContext* fmt_ctx = nullptr;
    if (avformat_open_input(&fmt_ctx, "https://streams.radio.co/s7a5fb9da6/listen", nullptr, nullptr) != 0) {
        fprintf(stderr, "Failed to open stream.\n");
        return -1;
    }

    // Find the best audio stream
    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
        fprintf(stderr, "Failed to find stream info.\n");
        return -1;
    }
    int audio_stream_idx = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (audio_stream_idx < 0) {
        fprintf(stderr, "Failed to find audio stream.\n");
        return -1;
    }
    AVStream* audio_stream = fmt_ctx->streams[audio_stream_idx];

    // Find decoder
    const AVCodec* codec = avcodec_find_decoder(audio_stream->codecpar->codec_id);
    if (!codec) {
        fprintf(stderr, "Failed to find codec.\n");
        return -1;
    }

    // Initialize codec context
    AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) {
        fprintf(stderr, "Failed to allocate codec context.\n");
        return -1;
    }
    if (avcodec_parameters_to_context(codec_ctx, audio_stream->codecpar) < 0) {
        fprintf(stderr, "Failed to copy codec parameters.\n");
        return -1;
    }

    // Open codec
    if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
        fprintf(stderr, "Failed to open codec.\n");
        return -1;
    }

    // Initialize SDL audio
    SDL_Init(SDL_INIT_AUDIO);

    SDL_AudioSpec spec;
    spec.freq = codec_ctx->sample_rate;
    spec.format = AUDIO_S16SYS;
    spec.channels = codec_ctx->channels;
    spec.samples = 1024;
    spec.callback = audio_callback;
    spec.userdata = codec_ctx;

    if (SDL_OpenAudio(&spec, nullptr) < 0) {
        fprintf(stderr, "Failed to open audio: %s\n", SDL_GetError());
        return -1;
    }

    // Start playback
    SDL_PauseAudio(0);

    // Decode audio packets
    AVPacket packet;
    AVFrame* frame = av_frame_alloc();
    while (av_read_frame(fmt_ctx, &packet) >= 0) {
        if (packet.stream_index == audio_stream_idx) {
            if (avcodec_send_packet(codec_ctx, &packet) == 0) {
                while (avcodec_receive_frame(codec_ctx, frame) == 0) {
                    // Here you would convert frame->data into an SDL audio buffer
                }
            }
        }
        av_packet_unref(&packet);
    }

    // Clean up
    av_frame_free(&frame);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&fmt_ctx);
    SDL_CloseAudio();
    SDL_Quit();

    return 0;
}

