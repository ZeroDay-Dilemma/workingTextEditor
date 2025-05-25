#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/channel_layout.h>
#include <libavutil/frame.h>
#include <libavutil/samplefmt.h>

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

    // Allocate frame and packet
    AVFrame *frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Failed to allocate frame.\n");
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
                    printf("Decoded audio frame: channels=%d, sample_rate=%d, nb_samples=%d\n",
                           av_get_channel_layout_nb_channels(frame->channel_layout),
                           frame->sample_rate,
                           frame->nb_samples);
                }
            }
        }
        av_packet_unref(&packet);
    }

    // Free resources
    av_frame_free(&frame);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&format_ctx);

    // Clean up FFmpeg library
    avformat_network_deinit();

    return 0;
}

