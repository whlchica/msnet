#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#define mch_msg(str) printf("%s", (str))
/*
 * print a message with three spaces prepended and '\n' appended.
 */
static void usage_msg(char* s)
{
    mch_msg("   ");
    mch_msg(s);
    mch_msg("\n");
}

/*
 * Print messages for "redis -p" or "redis --help" and exit.
 */
static void usage(void)
{
    static char*(use[]) = {
        "[file ..]       edit specified file(s)",
        "-               read text from stdin",
        "-t tag          edit file where tag is defined",
    };
    mch_msg("\n\nUsage:");
    for (int i = 0;; ++i) {
        mch_msg(" redis [arguments] ");
        mch_msg(use[i]);
        if (i == (sizeof(use) / sizeof(char*)) - 1)
            break;
        mch_msg("\n   or:");
    }
    mch_msg("\n\nArguments:\n");
    usage_msg("--width          default 1920, optional, width of image");
    usage_msg("--height         default 1080, optional, height of image");
    usage_msg("--format         default NV12, optional, fourcc of format");
    usage_msg("--count          default    1, optional, how many frames to capture");
    usage_msg("--device         required, path of video device");
    usage_msg("--output         required, output file path");
    usage_msg("--verbose        optional, print more log");
}

static struct option long_options[] = { { "width", required_argument, 0, 'w' },  { "height", required_argument, 0, 'h' }, { "format", required_argument, 0, 'f' },
                                        { "device", required_argument, 0, 'd' }, { "output", required_argument, 0, 'o' }, { "count", required_argument, 0, 'c' },
                                        { "help", no_argument, 0, 'p' },         { "verbose", no_argument, 0, 'v' },      { 0, 0, 0, 0 } };

void parse_args(int argc, char* const* argv)
{
    int c;
    int digit_optind = 0;
    while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index       = 0;

        /**
         * ./redis -w640 -h480 -ddev/video0
         * ./redis -w 640 -h 480 -d dev/video0
         * ./redis --width=640 --height=480 --device=dev/video0
         *
         * w:h:f:d:o::pv
         * 一个：表示-w后面必须带参数，可以连在一起也可以空格隔开
         * 两个: 表示可带可不带。这时带参数时必须用空格隔开
         */
        c = getopt_long(argc, argv, "w:h:f:d:o::pv", long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
        case 'c':
            printf("%d\n", atoi(optarg));
            break;
        case 'w':
            printf("%d\n", atoi(optarg));
            break;
        case 'h':
            printf("%d\n", atoi(optarg));
            break;
        case 'f':
            // format = v4l2_fourcc(optarg[0], optarg[1], optarg[2], optarg[3]);
            break;
        case 'd':
            printf("%s\n", optarg);
            break;
        case 'o':
            printf("%s\n", optarg);
            break;
        case '?':
        case 'p':
            usage();
            exit(-1);
        default:
            printf("?? getopt returned character code 0%o ??\n", c);
        }
    }
}