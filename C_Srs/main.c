#include <stdbool.h>
#include <libavutil/time.h>
#include <libavformat/avformat.h>

#define INPUT_FILE "/home/wyw/soft/GitHub/srs/trunk/doc/source.flv"
#define OUTPUT_URL "rtmp://192.168.0.221/live/livestream"

int main(int argc, char **argv)
{
    AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
    AVOutputFormat *ofmt = NULL;
    AVPacket pkt;
    int ret, i, stream_index = 0, *stream_mapping = NULL, stream_mapping_size = 0;

    /* 初始化网络模块 */
    avformat_network_init();

    /* 1.1、读取文件头,获取封装格式相关信息 */
    if ((ret = avformat_open_input(&ifmt_ctx, INPUT_FILE, NULL, NULL)) < 0)
    {
        fprintf(stderr, "Could not open input file.\n");
        goto end;
    }

    /* 1.2、获取封装格式上下文 */
    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0)
    {
        fprintf(stderr, "Failed to retrieve input stream information\n");
        goto end;
    }

    /* 打印输入文件信息: 打印有关输入或输出格式的详细信息, 例如持续时间、比特率、流，容器、程序、元数据、侧数据、编解码器和时基 */
    av_dump_format(ifmt_ctx, 0, INPUT_FILE, 0);

    /* 2.1、打开输出, 分配输出上下文 */
    bool push_stream = false;
    char *ofmt_name = NULL;
    if (strstr(OUTPUT_URL, "rtmp://") != NULL)
    {
        push_stream = true;
        ofmt_name = "flv";
        printf("Out put format name: %s.\n", ofmt_name);
    }
    else if (strstr(OUTPUT_URL, "udp://") != NULL)
    {
        push_stream = true;
        ofmt_name = "mpegts";
        printf("Out put format name: %s.\n", ofmt_name);
    }
    else
    {
        push_stream = false;
        ofmt_name = NULL;
        printf("Not out put format name.\n");
    }

    /* 2、创建输出上下文 */
    avformat_alloc_output_context2(&ofmt_ctx, NULL, ofmt_name, OUTPUT_URL);
    if (!ofmt_ctx)
    {
        printf("Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        goto end;
    }

    /* 获取流大小 */
    stream_mapping_size = ifmt_ctx->nb_streams;
    stream_mapping = av_mallocz_array(stream_mapping_size, sizeof(*stream_mapping));
    printf("Stream allocate a memory block size (bytes): %d\n", *stream_mapping);
    if (!stream_mapping)
    {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    /* 输出容器格式 */
    ofmt = ofmt_ctx->oformat;

    AVRational frame_rate;
    double duration;

    for (i = 0; i < ifmt_ctx->nb_streams; i++)
    {
        AVStream *out_stream;
        AVStream *in_stream = ifmt_ctx->streams[i];
        AVCodecParameters *in_codecpar = in_stream->codecpar;

        if (in_codecpar->codec_type != AVMEDIA_TYPE_AUDIO &&
            in_codecpar->codec_type != AVMEDIA_TYPE_VIDEO &&
            in_codecpar->codec_type != AVMEDIA_TYPE_SUBTITLE)
        {
            stream_mapping[i] = -1;
            continue;
        }

        if (push_stream && (in_codecpar->codec_type == AVMEDIA_TYPE_VIDEO))
        {
            /* 获取帧率: av_guess_frame_rate()根据容器和编解码器信息猜测帧速率 */
            frame_rate = av_guess_frame_rate(ifmt_ctx, in_stream, NULL);
            duration = (frame_rate.num && frame_rate.den ? av_q2d((AVRational){frame_rate.den, frame_rate.num}) : 0);
        }

        stream_mapping[i] = stream_index++;

        /* 2.2、将一个新流(out_stream)添加到输出文件(ofmt_ctx) */
        out_stream = avformat_new_stream(ofmt_ctx, NULL);
        if (!out_stream)
        {
            printf("Failed allocating output stream\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }

        /* 2.3 将当前输入流中的参数拷贝到输出流中 */
        ret = avcodec_parameters_copy(out_stream->codecpar, in_codecpar);
        if (ret < 0)
        {
            printf("Failed to copy codec parameters\n");
            goto end;
        }
        out_stream->codecpar->codec_tag = 0;
    }

    /* 打印输出文件信息: 打印有关输入或输出格式的详细信息, 例如持续时间、比特率、流，容器、程序、元数据、侧数据、编解码器和时基 */
    av_dump_format(ofmt_ctx, 0, OUTPUT_URL, 1);

    if (!(ofmt->flags & AVFMT_NOFILE))
    {
        /* 2.4 创建并初始化一个AVIOContext，用以访问URL(out_filename)指定的资源 */
        ret = avio_open(&ofmt_ctx->pb, OUTPUT_URL, AVIO_FLAG_WRITE);
        if (ret < 0)
        {
            printf("Could not open output file '%s'", OUTPUT_URL);
            goto end;
        }
    }

    /* 3.1、处理数据, 写输出文件头 */
    ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0)
    {
        printf("Error occurred when opening output file\n");
        goto end;
    }

    while (1)
    {
        AVStream *in_stream, *out_stream;

        // 3.2 从输出流读取一个packet
        ret = av_read_frame(ifmt_ctx, &pkt);
        if (ret < 0)
        {
            break;
        }

        in_stream = ifmt_ctx->streams[pkt.stream_index];
        if (pkt.stream_index >= stream_mapping_size ||
            stream_mapping[pkt.stream_index] < 0)
        {
            av_packet_unref(&pkt);
            continue;
        }

        int codec_type = in_stream->codecpar->codec_type;
        if (push_stream && (codec_type == AVMEDIA_TYPE_VIDEO))
        {
            av_usleep((int64_t)(duration * AV_TIME_BASE));
        }

        pkt.stream_index = stream_mapping[pkt.stream_index];
        out_stream = ofmt_ctx->streams[pkt.stream_index];

        /* copy packet
         * 3.3 更新packet中的pts和dts
         *
         * 关于AVStream.time_base(容器中的time_base)的说明：
         * 输入：输入流中含有time_base，在avformat_find_stream_info()中可取到每个流中的time_base
         * 输出：avformat_write_header()会根据输出的封装格式确定每个流的time_base并写入文件中
         * AVPacket.pts和AVPacket.dts的单位是AVStream.time_base，不同的封装格式AVStream.time_base不同
         * 所以输出文件中，每个packet需要根据输出封装格式重新计算pts和dts
         */
        av_packet_rescale_ts(&pkt, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;

        /* 3.4 将packet写入输出 */
        ret = av_interleaved_write_frame(ofmt_ctx, &pkt);
        if (ret < 0)
        {
            printf("Error muxing packet\n");
            break;
        }
        av_packet_unref(&pkt);
    }

    /* 3.5 写输出文件尾 */
    av_write_trailer(ofmt_ctx);

end:
    avformat_close_input(&ifmt_ctx);

    /* close output */
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
    {
        avio_closep(&ofmt_ctx->pb);
    }
    avformat_free_context(ofmt_ctx);

    av_freep(&stream_mapping);

    if (ret < 0 && ret != AVERROR_EOF)
    {
        printf("Error occurred: %s\n", av_err2str(ret));
        return 1;
    }

    return ret < 0 ? 1 : 0;
}