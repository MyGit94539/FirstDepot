/*
 * GPIO test - A small tool for GPIO verification. 

 */

#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>

#include <linux/gpio.h>

#define DEFAULT_GPIODEV "/dev/gpiochip0"
#define DEFAULT_GPIODEBUGFS "/sys/kernel/debug/gpio"
#define GPIO_LINES_MAX 128

#define ARRAYSIZE(a) (sizeof(a)/sizeof((a)[0]))

static const char *const short_options = "sil:d:";
static const struct option long_options[] = {
	{"help",		0,	NULL,	1  },
	{"set-dir",		0,	NULL,	2},
	{"set-level",		1,	NULL,	3},
	{"show-level",		0,	NULL,	4},
	{"blink",		0,	NULL,	5},
	{"iotest",		0,	NULL,	6},
	{"delay",		1,	NULL,	7},
	{"count",		1,	NULL,	8},
	{"timeout",		1,	NULL,	9},
	{"evttest",		0,	NULL,	10},
	{"dev",			1,	NULL,	'd'},
	{"show-debugfs",	0,	NULL,	's'},
	{"show-info",		0,	NULL,	'i'},
	{"line",		1,	NULL,	'l'},
	{NULL,			0,	NULL,	0  },
};

struct gpioline_flag_t {
	char *name;
	unsigned long mask;
};

static const struct gpioline_flag_t gpioline_flag[] = {
	{ "KERNEL", GPIOLINE_FLAG_KERNEL, },
	{ "IS_OUT", GPIOLINE_FLAG_IS_OUT, },
	{ "ACTIVE_LOW", GPIOLINE_FLAG_ACTIVE_LOW, },
	{ "OPEN_DRAIN", GPIOLINE_FLAG_OPEN_DRAIN, },
	{ "OPEN_SOURCE", GPIOLINE_FLAG_OPEN_SOURCE, },
};

static void print_usage(FILE *stream, char *exec_name, int exit_code)
{
	fprintf(stream, "Usage: %s [options]\n", exec_name);
	fprintf(stream,
		"     --help            Display this usage information\n"
		" -s  --show-debugfs    Show GPIO debugfs information\n"
		" -i  --show-info       Show information about specified lines or chip\n"
		" -d  --dev <DEVICE>    Use <device> as GPIO device file (default: %s)\n"
		" -l  --line <NR>       Handle line instead of whole chip\n"
		"     --show-level      Show levels of the selected lines\n"
		"     --set-level <LVL> Set levels of the selected lines\n"
		"     --blink           Blink selected lines with specified delay and count\n"
		"     --iotest          Do basic IO testing\n"
		"     --evttest         Do IO event testing\n"
		"     --delay <US>      Specifies a delay in us\n"
		"     --count <COUNT>   Specifies a count (0 means infinite)\n"
		"     --timeout <US>    Specifies a timeout\n",
		DEFAULT_GPIODEV);

	exit(exit_code);
}

void print_gpio_debugfs(void)
{
	int fd;
	char buf[100];
	int n;
	char* dbgfile = DEFAULT_GPIODEBUGFS;

	fprintf(stdout, "========= Start information from DebugFS ========\n");
	fd = open(dbgfile, O_RDONLY);
	if (-1 == fd) {
		fprintf(stdout, "Unable to open debug information (%s)\n",
			dbgfile);
		return;
	}

	while ((n = read(fd, buf, 100)) > 0)
		write(STDOUT_FILENO, buf, n);

	fprintf(stdout, "========== End information from DebugFS =========\n");

	close(fd);
}

unsigned long get_time_diff(struct timespec *ts_start, struct timespec *ts_end)
{
	if (ts_end->tv_nsec > ts_start->tv_nsec)
		return (ts_end->tv_nsec - ts_start->tv_nsec);

	return (1000000000L - ts_start->tv_nsec) + ts_end->tv_nsec;
}

int ns_sleep(unsigned long long sleep_time_ns, unsigned long offset)
{
	struct timespec tv;

	if (sleep_time_ns > offset)
		sleep_time_ns -= offset;
	else
		return 0;

	tv.tv_sec = (time_t)sleep_time_ns / 1000000000L;
	tv.tv_nsec = (long)(sleep_time_ns % 1000000000L);

	while (1)
	{
		int ret = clock_nanosleep (CLOCK_REALTIME, 0, &tv, &tv);
		if (ret == 0)
			return 0;
		else if (errno == EINTR)
			continue;
		else {
			fprintf(stderr, "nanosleep failed - %s\n",
				strerror(errno));
			return ret;
		}
	}

	return 0;
}

int ns_sleep_abs(struct timespec *tv, unsigned long long sleep_time_ns)
{
	tv->tv_sec = tv->tv_sec
		+ (time_t)((tv->tv_nsec + sleep_time_ns) / 1000000000L);
	tv->tv_nsec = (long)((tv->tv_nsec + sleep_time_ns) % 1000000000L);


	while (1)
	{
		int ret = clock_nanosleep (CLOCK_MONOTONIC, TIMER_ABSTIME, tv,
					   NULL);
		if (ret == 0)
			return 0;
		else if (errno == EINTR)
			continue;
		else {
			fprintf(stderr, "nanosleep failed - %s\n",
				strerror(errno));
			return ret;
		}
	}

	return 0;
}

void print_gpio_info(int fd, unsigned int *line_mask)
{
	struct gpiochip_info cinfo;
	unsigned int i;
	int ret;

	fprintf(stdout, "GPIO_GET_CHIPINFO_IOCTL:\n");
	ret = ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, &cinfo);
	if (ret == 0)
		fprintf(stdout,
			"  name:  %s\n"
			"  label: \"%s\"\n"
			"  lines: %u\n",
			cinfo.name, cinfo.label, cinfo.lines);
	else {
		fprintf(stderr, "Failed: %s\n", strerror(errno));
		return;
	}



	for (i = 0; i < cinfo.lines; i++) {
		struct gpioline_info linfo;
		int first;
		unsigned int j;

		if (!line_mask[i])
			continue;

		linfo.line_offset = i;

		fprintf(stdout, "GPIO_GET_LINEINFO_IOCTL - line_offset %u:\n",
			linfo.line_offset);

		if (linfo.line_offset > GPIO_LINES_MAX) {
			fprintf(stderr,
				"More than %u lines not supported - aborting!\n",
				GPIO_LINES_MAX);
			break;
		}

		ret = ioctl(fd, GPIO_GET_LINEINFO_IOCTL, &linfo);
		if (ret == 0) {
			fprintf(stdout, "  name:     %s\n",
				linfo.name[0]?linfo.name:"-");
			fprintf(stdout, "  consumer: %s\n",
				linfo.consumer[0]?linfo.consumer:"-");
			fprintf(stdout, "  flags:    0x%x (", linfo.flags);
			first = 1;
			for (j = 0; j < ARRAYSIZE(gpioline_flag); j++) {
				if (linfo.flags & gpioline_flag[j].mask) {
					if (!first)
						fprintf(stdout, ", ");
					fprintf(stdout, "%s",
						gpioline_flag[j].name);
					first = 0;
				}
			}
			if (first)
				fprintf(stdout, "-)\n");
			else
				fprintf(stdout, ")\n");

		} else
			fprintf(stderr, "Failed: %s\n", strerror(errno));
	}
}

int fill_request(struct gpiohandle_request *req, unsigned int *line_mask,
		 unsigned int flags, unsigned int line_min,
		 unsigned int line_max)
{
	int lines = 0;
	unsigned int i;

	strcpy(req->consumer_label, "gpio-test");
	req->flags = flags;

	for (i=0; i < GPIO_LINES_MAX; i++) {
		if (lines >= GPIOHANDLES_MAX) {
			fprintf(stderr,
				"Out of GPIO handles - limitting to %u\n",
				GPIOHANDLES_MAX);
			break;
		}
		if (line_mask[i] && (i >= line_min) && (i <= line_max))
			req->lineoffsets[lines++] = i;
	}

	req->lines = lines;

	return lines;
}

void show_gpio_level(int fd, unsigned int *line_mask)
{
	struct gpiohandle_request req;
	struct gpiohandle_data data;
	int lines;
	int i;
	int ret;

	fprintf(stdout, "Show GPIO values:\n");

	lines = fill_request(&req, line_mask, GPIOHANDLE_REQUEST_INPUT, 0, ~0);

	ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req);
	if (ret != 0) {
		fprintf(stderr, "GPIO_GET_LINEHANDLE_IOCTL: %s\n",
			strerror(errno));
		return;
	}

	ret = ioctl(req.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);
	if (ret != 0) {
		fprintf(stderr, "GPIOHANDLE_GET_LINE_VALUES_IOCTL: %s\n",
			strerror(errno));
		return;
	}

	for (i=0; i < lines; i++) {
		printf("line %u is %i %s\n", req.lineoffsets[i], data.values[i],
		       data.values[i] ? "high" : "low");
	}

	close(req.fd);
}

void do_gpio_set_level(int fd, unsigned int *line_mask, int value)
{
	struct gpiohandle_request req;
	struct gpiohandle_data data;
	int lines;
	int i;
	int ret;

	fprintf(stdout, "GPIO set level to %s\n", value?"high":"low");

	lines = fill_request(&req, line_mask, GPIOHANDLE_REQUEST_OUTPUT,
			     0, ~0);

	ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req);
	if (ret != 0) {
		fprintf(stderr, "GPIO_GET_LINEHANDLE_IOCTL: %s\n",
			strerror(errno));
		return;
	}

	for (i=0; i < lines; i++) {
		data.values[i] = value?1:0;
	}

	ret = ioctl(req.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
	if (ret != 0) {
		fprintf(stderr, "GPIOHANDLE_SET_LINE_VALUES_IOCTL: %s\n",
			strerror(errno));
		return;
	}

	fprintf(stdout, "Setting GPIO done\n");

	close(req.fd);
}

static int fill_gpio_data(struct gpiohandle_data *data, unsigned long ldata)
{
	int i;

	for (i=0; i< GPIOHANDLES_MAX; i++)
		data->values[i] = (ldata & (1<<i)) ? 1 : 0;

	return 0;
}

static int compare_gpio_data(struct gpiohandle_data *a,
			     struct gpiohandle_data *b, int lines, int print)
{
	int i;
	int ret = 0;

	if (lines > GPIOHANDLES_MAX || lines == 0)
		lines = GPIOHANDLES_MAX;

	for (i=0; i < lines; i++) {
		if (print)
			fprintf(stdout, "line %i: %d <=> %d\n", i,
				a->values[i],  b->values[i]);

		if (a->values[i] != b->values[i])
			ret = 1;
	}

	return ret;
}
typedef struct event_data_type {
	int                       fd;
	int                       running;
	struct gpioevent_request  req_evt;
	struct gpiohandle_request req_out;
	struct gpioevent_data     event;
	struct gpioevent_data     pin;
	unsigned long             timeout;
	unsigned long             delay;
	unsigned long             count;
	unsigned long             irq_counter;
	unsigned long             loop_counter;
} event_data_t;

void* gpio_pin_toggle_thread(void *ptr)
{
	event_data_t *event_data;
	event_data = (event_data_t*) ptr;
	struct gpiohandle_request *req = &event_data->req_out;
	struct gpiohandle_data data;
	unsigned long timeout = 50000;
	int ret;

	data.values[0] = 1;
	event_data->loop_counter = 0;

	printf("Starting toggling thread for pin %u\n", req->lineoffsets[0]);
	ret = ioctl(req->fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
	if (ret != 0) {
		fprintf(stderr, "GPIOHANDLE_SET_LINE_VALUES_IOCTL: %s\n",
			strerror(errno));
		pthread_exit(&ret);
	}
	while ((event_data->running == 0) && (timeout--))
		usleep(1);

	while (event_data->running
	       && (event_data->loop_counter < event_data->count)) {
		usleep(event_data->delay);

		if (data.values[0] == 0) {
			data.values[0] = 1;
			event_data->loop_counter++;
		} else
			data.values[0] = 0;

		ret = ioctl(req->fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
		if (ret != 0) {
			fprintf(stderr,
				"GPIOHANDLE_SET_LINE_VALUES_IOCTL: %s\n",
				strerror(errno));
			return NULL;
		}
	}
	printf("Toggling thread for pin %u exited\n", req->lineoffsets[0]);

	pthread_exit(&ret);
}

void* gpio_pin_event_thread(void *ptr)
{
	fd_set fds;
	struct timespec timeout;
	event_data_t *event_data;
	event_data = (event_data_t*) ptr;
	struct gpioevent_request *req = &event_data->req_evt;
	struct gpioevent_data *event = &event_data->event;
	int ret=0;
	int exit=0;

	printf("Starting event listener for pin %u\n", req->lineoffset);

	event_data->irq_counter = 0;
	timeout.tv_sec = event_data->timeout / 1000000;
	timeout.tv_nsec = (event_data->timeout % 1000000)*1000;

	event_data->running = 1;

	usleep(event_data->delay);

	while (!exit) {
		FD_ZERO(&fds);
		FD_SET(req->fd, &fds);

		if (pselect(req->fd+1, &fds, NULL, NULL, &timeout, NULL)
		    > 0) {
			ret = read(req->fd, event, sizeof(*event));
			if (ret < 0)
				fprintf(stderr,
					"Error reading event status: %s\n",
				strerror(errno));
			event_data->irq_counter++;
		} else
			exit=1;
	}

	event_data->running = 0;

	printf("Event listener for pin %u exited with %lu events\n",
	       req->lineoffset, event_data->irq_counter);

	pthread_exit(&ret);
}

void* gpio_read_event_thread(void *ptr)
{
	fd_set fds;
	struct timeval timeout;
	event_data_t *event_data;
	event_data = (event_data_t*) ptr;
	struct gpioevent_request *req = &event_data->req_evt;
	struct gpioevent_data *event = &event_data->event;
	int ret = 0;

	timeout.tv_sec = event_data->timeout / 1000000;
	timeout.tv_usec = event_data->timeout % 1000000;
	FD_ZERO(&fds);
	FD_SET(req->fd, &fds);

	if (select(req->fd+1, &fds, NULL, NULL, &timeout) > 0) {
		ret = read(req->fd, event, sizeof(*event));
		if (ret < 0)
			fprintf(stderr,
				"Error reading event status: %s\n",
				strerror(errno));
	} else
		event->id = 0;

	pthread_exit(&ret);
}

int do_gpio_evttest(int fd, unsigned int *line_mask, unsigned long count,
		   unsigned long delay, unsigned long timeout)
{
	pthread_t thread_evt[GPIOHANDLES_MAX];
	pthread_t thread_out[GPIOHANDLES_MAX];
	event_data_t event_data[GPIOHANDLES_MAX];
	struct gpiohandle_request req_all;
	struct gpiohandle_request req_out;
	struct gpiohandle_request req_in;
	int lines_all;
	int lines_in;
	int lines_out;
	int i;
	int ret;
	unsigned long fail = 0;

	fprintf(stdout, "GPIO event test\n");

	lines_all = fill_request(&req_all, line_mask, 0, 0, ~0);
	lines_out = fill_request(&req_out, line_mask, GPIOHANDLE_REQUEST_OUTPUT,
				  0, lines_all/2 - 1);
	lines_in = fill_request(&req_in, line_mask, GPIOHANDLE_REQUEST_INPUT,
				  lines_all/2, lines_all);

	if (lines_out != lines_in) {
		fprintf(stderr,
			"IO count needs to be a multiple of 2 - aborting\n");
		return -1;
	}

	for (i=0; i<lines_in; i++) {
		event_data[i].fd = fd;
		event_data[i].timeout = timeout;
		event_data[i].delay = delay;
		event_data[i].count = count;
		event_data[i].running = 0;

		event_data[i].req_evt.lineoffset = req_in.lineoffsets[i];
		if (i%2)
			event_data[i].req_evt.eventflags =
				GPIOEVENT_REQUEST_FALLING_EDGE;
		else
			event_data[i].req_evt.eventflags =
				GPIOEVENT_REQUEST_RISING_EDGE;
		strcpy(event_data[i].req_evt.consumer_label, "gpio-test");
		ret = ioctl(event_data[i].fd, GPIO_GET_LINEEVENT_IOCTL,
			    &event_data[i].req_evt);
		if (ret != 0) {
			fprintf(stderr, "GPIO_GET_LINEEVENT_IOCTL: %s\n",
				strerror(errno));
			return -1;
		}

		lines_out = fill_request(&event_data[i].req_out, line_mask,
					 GPIOHANDLE_REQUEST_OUTPUT, i, i);

		ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL,
			    &event_data[i].req_out);
		if (ret != 0) {
			fprintf(stderr,
				"GPIO_GET_LINEHANDLE_IOCTL (line %d): %s\n",
				event_data[i].req_out.lineoffsets[0],
				strerror(errno));
			return -1;
		}
		pthread_create(&thread_evt[i], NULL, &gpio_pin_event_thread,
			       (void*)&event_data[i]);
		pthread_create(&thread_out[i], NULL, &gpio_pin_toggle_thread,
			       (void*)&event_data[i]);
	}


	for (i=0;i<lines_in;i++) {
		pthread_join(thread_out[i], NULL);
		pthread_join(thread_evt[i], NULL);
		close(event_data[i].req_evt.fd);
		close(event_data[i].req_out.fd);

		printf("Pair %u -> loops requested %lu, loops done %lu -> IRQs detected %lu\n",
		       i, event_data[i].count, event_data[i].loop_counter,
		       event_data[i].irq_counter);
		if (event_data[i].count != event_data[i].loop_counter)
			fail++;
	}

	return fail?-1:0;
}

int evt_test_loop(int fd, int lines, struct gpiohandle_request *req_out,
		  struct gpiohandle_request *req_in, unsigned long irqdirptr,
		  unsigned long delay, unsigned long timeout)
{
	pthread_t thread[GPIOHANDLES_MAX];
	event_data_t event_data[GPIOHANDLES_MAX];
	struct gpiohandle_data data_in;
	struct gpiohandle_data data_out;
	int i,j;
	int ret, fail, failcount=0;

	fill_gpio_data(&data_out, 0);
	ret = ioctl(req_out->fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL,
		    &data_out);
	if (ret != 0) {
		fprintf(stderr,
			"GPIOHANDLE_SET_LINE_VALUES_IOCTL: %s\n",
			strerror(errno));
		return -1;
	}

	fill_gpio_data(&data_in, irqdirptr);
	for (j=0;j<lines;j++) {
		event_data[j].req_evt.lineoffset = req_in->lineoffsets[j];
		if (data_in.values[j])
			event_data[j].req_evt.eventflags =
				GPIOEVENT_REQUEST_FALLING_EDGE;
		else
			event_data[j].req_evt.eventflags =
				GPIOEVENT_REQUEST_RISING_EDGE;
		event_data[j].fd = fd;
		event_data[j].timeout = timeout;

		strcpy(event_data[j].req_evt.consumer_label, "gpio-test");
		ret = ioctl(event_data[j].fd, GPIO_GET_LINEEVENT_IOCTL,
			    &event_data[j].req_evt);
		if (ret != 0) {
			fprintf(stderr, "GPIO_GET_LINEEVENT_IOCTL: %s\n",
				strerror(errno));
			return -1;
		}
	}

	for (i=0; i<GPIOHANDLES_MAX; i++) {
		fprintf(stdout, "  Testing events with pattern 0x%03x: ", i);

		for (j=0;j<lines;j++) {
			pthread_create(&thread[j], NULL, &gpio_read_event_thread,
					(void*)&event_data[j]);
		}

		fill_gpio_data(&data_out, i);
		ret = ioctl(req_out->fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL,
			    &data_out);
		if (ret != 0) {
			fprintf(stderr,
				"GPIOHANDLE_SET_LINE_VALUES_IOCTL: %s\n",
				strerror(errno));
			return -1;
		}

		if (i>0)
			fill_gpio_data(&data_in, i-1);
		else
			fill_gpio_data(&data_in, 0);

		fail = 0;
		for (j=0;j<lines;j++) {
			char out = '_';

			pthread_join(thread[j], NULL);

			if ((data_in.values[j] == 0)
			    && (data_out.values[j] == 1)) {
				out='R';
			} else if ((data_in.values[j] == 1)
			    && (data_out.values[j] == 0)) {
				out='F';
			} else if (data_out.values[j] == 1) {
				out='H';
			} else if (data_out.values[j] == 0) {
				out='L';
			}
			printf("%02d%c", j, out);

			switch (event_data[j].event.id) {
			case GPIOEVENT_EVENT_RISING_EDGE:
				printf("R");
				if (event_data[j].req_evt.eventflags
				    & GPIOEVENT_REQUEST_RISING_EDGE) {
					printf("*");
				} else {
					fail++;
					printf("s");
				}
				break;
			case GPIOEVENT_EVENT_FALLING_EDGE:
				printf("F");
				if (event_data[j].req_evt.eventflags &
				    GPIOEVENT_REQUEST_FALLING_EDGE) {
					printf("*");
				} else {
					fail++;
					printf("s");
				}
				break;
			case 0:
				printf("-");
				if (out=='H' || out == 'L')
					printf(" ");
				else if ((out=='R') &&
					 !(event_data[j].req_evt.eventflags
				    & GPIOEVENT_REQUEST_RISING_EDGE))
					printf(" ");
				else if ((out=='F') &&
					 !(event_data[j].req_evt.eventflags
				    & GPIOEVENT_REQUEST_FALLING_EDGE))
					printf(" ");
				else {
					fail++;
					printf("o");
				}
				break;
			default:
				fail++;
				printf("?");
			}
			printf(" ");

		}
		if (fail) {
			failcount += fail;
			printf("FAIL\n");
		} else
			printf("OK\n");

		usleep(delay);
	}

	for (j=0;j<lines;j++)
		close(event_data[j].req_evt.fd);

	return failcount;
}

int do_gpio_iotest(int fd, unsigned int *line_mask, unsigned long count,
		   unsigned long delay, unsigned long timeout)
{
	struct gpiohandle_request req_all;
	struct gpiohandle_request req_out;
	struct gpiohandle_request req_in;
	struct gpiohandle_data data_in;
	struct gpiohandle_data data_out;
	unsigned long counter;
	int lines_all;
	int lines_out;
	int lines_in;
	int i;
	int ret;
	unsigned long fail = 0;

	fprintf(stdout, "GPIO test\n");

	lines_all = fill_request(&req_all, line_mask, 0, 0, ~0);
	lines_out = fill_request(&req_out, line_mask, GPIOHANDLE_REQUEST_OUTPUT,
				  0, lines_all/2 - 1);
	lines_in = fill_request(&req_in, line_mask, GPIOHANDLE_REQUEST_INPUT,
				  lines_all/2, lines_all);

	if (lines_out != lines_in) {
		fprintf(stderr,
			"IO count needs to be a multiple of 2 - aborting\n");
		return -1;
	}

	ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req_out);
	if (ret != 0) {
		fprintf(stderr, "GPIO_GET_LINEHANDLE_IOCTL (out): %s\n",
			strerror(errno));
		return -1;
	}

	ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req_in);
	if (ret != 0) {
		fprintf(stderr, "GPIO_GET_LINEHANDLE_IOCTL (in): %s\n",
			strerror(errno));
		close(req_out.fd);
		return -1;
	}

	for (i=0; i<GPIOHANDLES_MAX; i++) {
		fprintf(stdout, "  Testing pattern 0x%02x\n", i);

		fill_gpio_data(&data_out, i);
		ret = ioctl(req_out.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL,
			    &data_out);
		if (ret != 0) {
			fprintf(stderr,
				"GPIOHANDLE_SET_LINE_VALUES_IOCTL: %s\n",
				strerror(errno));
			goto error_cleanup;
		}
		ret = ioctl(req_in.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL,
			    &data_in);
		if (ret != 0) {
			fprintf(stderr,
				"GPIOHANDLE_SET_LINE_VALUES_IOCTL: %s\n",
				strerror(errno));
			goto error_cleanup;
		}

		if (compare_gpio_data(&data_out, &data_in, lines_out, 0)) {
			compare_gpio_data(&data_out, &data_in, lines_out, 1);
			fprintf(stderr,
				"Unexpected test result - aborting!\n");
			goto error_cleanup;
		}

		usleep(delay);
	}

	close(req_in.fd);

	for (counter=0; counter<count; counter++) {
		ret = evt_test_loop(fd, lines_in, &req_out, &req_in, counter,
				    delay, timeout);
		if (ret < 0)
			goto error_cleanup;

		if (ret) {
			fail += ret;
			printf("IO test run %lu failed (%lu fails so far)!\n",
			       counter+1, fail);
		} else
			printf("IO test run %lu successful (%lu fails so far)!\n",
				counter+1, fail);
	}


	close(req_out.fd);
	close(req_in.fd);
	fprintf(stderr, "IO test finished with %lu failures!\n", fail);

	return fail;
error_cleanup:
	fprintf(stderr, "IO test aborted!\n");

	close(req_out.fd);
	close(req_in.fd);

	return -1;
}

void do_gpio_blink(int fd, unsigned int *line_mask, unsigned long count,
	      unsigned long delay)
{
	struct timespec ts_start, ts_end, ts_delay;
	struct gpiohandle_request req;
	struct gpiohandle_data data_high;
	struct gpiohandle_data data_low;
	unsigned long long counter = 0;
	unsigned long dtime;
	float set_high_time_mean = 0;
	float set_high_time_max = 0;
	float set_low_time_mean = 0;
	float set_low_time_max = 0;
	int lines;
	int i;
	int ret;

	fprintf(stdout, "GPIO blink test - delay %lu us, count %lu:\n", delay,
		count);

	lines = fill_request(&req, line_mask, GPIOHANDLE_REQUEST_OUTPUT,
			     0, ~0);

	ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req);
	if (ret != 0) {
		fprintf(stderr, "GPIO_GET_LINEHANDLE_IOCTL: %s\n",
			strerror(errno));
		return;
	}

	for (i=0; i < lines; i++) {
		data_high.values[i] = 1;
		data_low.values[i] = 0;
	}

	clock_gettime(CLOCK_MONOTONIC, &ts_delay);
	while (counter++ < count || count == 0) {
		clock_gettime(CLOCK_MONOTONIC, &ts_start);
		ret = ioctl(req.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_high);
		if (ret != 0) {
			fprintf(stderr, "GPIOHANDLE_SET_LINE_VALUES_IOCTL: %s\n",
				strerror(errno));
			return;
		}
		clock_gettime(CLOCK_MONOTONIC, &ts_end);
		dtime = get_time_diff(&ts_start, &ts_end);
		if (dtime > set_high_time_max)
			set_high_time_max = dtime;
		set_high_time_mean -= (set_high_time_mean-dtime)/counter;

		ns_sleep(delay*1000, dtime);
		/*if (delay*1000>dtime)*/
			/*ns_sleep_abs(&ts_delay, delay*1000);*/

		clock_gettime(CLOCK_MONOTONIC, &ts_start);
		ret = ioctl(req.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_low);
		if (ret != 0) {
			fprintf(stderr, "GPIOHANDLE_SET_LINE_VALUES_IOCTL: %s\n",
				strerror(errno));
			return;
		}
		clock_gettime(CLOCK_MONOTONIC, &ts_end);
		dtime = get_time_diff(&ts_start, &ts_end);
		if (dtime > set_low_time_max)
			set_low_time_max = dtime;
		set_low_time_mean -= (set_low_time_mean-dtime)/counter;

		ns_sleep(delay*1000, dtime);
		/*if (delay*1000>dtime)*/
			/*ns_sleep_abs(&ts_delay, delay*1000);*/

	}

	fprintf(stdout, "IOCTL set high time: mean %.2f us (max %.2f us)\n",
		set_high_time_mean/1000, set_high_time_max/1000);
	fprintf(stdout, "IOCTL set low time:  mean %.2f us (max %.2f us)\n",
		set_low_time_mean/1000, set_low_time_max/1000);

	close(req.fd);
}

unsigned int device_get_lines(int fd)
{
	struct gpiochip_info cinfo;
	int ret;

	ret = ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, &cinfo);
	if (ret) {
		fprintf(stderr, "Failed GPIO_GET_CHIPINFO_IOCTL: %s\n",
			strerror(errno));
		return 0;
	}

	return cinfo.lines;
}

int device_open(char *dev)
{
	int fd;

	fprintf(stdout, "Opening %s\n", dev);
	fd = open(dev, 0);
	if (-1 == fd) {
		fprintf(stderr, "ERROR %s - opening %s\n", strerror(errno),
			dev);
		exit(EXIT_FAILURE);
	}
	return fd;
}

void device_close(int fd, char* dev)
{
	fprintf(stdout, "Closing %s\n", dev);
	if (close(fd))
		fprintf(stderr, "ERROR %s - closing %s\n", strerror(errno),
			dev);
}


int main(int argc, char **argv)
{
	int fd;
	char *dev;
	int next_option;
	int show_debugfs=0;
	int show_info=0;
	int set_level=0;
	int show_level=0;
	int do_blink=0;
	int do_iotest=0;
	int do_evttest=0;
	int value=0;
	unsigned long delay=500000;
	unsigned long timeout=50000;
	unsigned long count=16;
	unsigned int line_mask[GPIO_LINES_MAX] = {0};
	unsigned long int tmp;
	int nlines = 0;

	dev = DEFAULT_GPIODEV;

	do {
		next_option = getopt_long(argc, argv, short_options,
					  long_options, NULL);
		switch (next_option) {
			case 1: /* help */
				print_usage(stdout, argv[0], EXIT_SUCCESS);
				break;
			case 2: /* set-dir */
				break;
			case 3: /* set-level */
				set_level=1;
				tmp = strtoul(optarg, NULL, 10);
				value = tmp?1:0;
				break;
			case 4: /* show-level */
				show_level=1;
				break;
			case 5: /* blink */
				do_blink=1;
				break;
			case 6: /* iotest */
				do_iotest=1;
				break;
			case 7: /* delay */
				delay = strtoul(optarg, NULL, 10);
				if (delay == ULONG_MAX && errno == ERANGE)
					fprintf(stderr,
						"delay parameter out of range!\n");
				break;
			case 8: /* count */
				count = strtoul(optarg, NULL, 10);
				if ((count == ULONG_MAX && errno == ERANGE)
				    || (count > 1000000))
					fprintf(stderr,
						"count parameter out of range!\n");
				break;
			case 9: /* timeout */
				timeout = strtoul(optarg, NULL, 10);
				if (timeout == ULONG_MAX && errno == ERANGE)
					fprintf(stderr,
						"timeout parameter out of range!\n");
				break;
			case 10: /* evttest */
				do_evttest=1;
				break;
			case 'd':
				dev = optarg;
				break;
			case 's':
				show_debugfs = 1;
				break;
			case 'i':
				show_info = 1;
				break;
			case 'l':
				tmp = strtoul(optarg, NULL, 10);
				if (tmp < GPIOHANDLES_MAX) {
					line_mask[tmp] = 1;
					nlines++;
				} else
					fprintf(stderr,
						"Line parameter out of range!\n");

				break;
			case '?':
				print_usage(stderr, argv[0], EXIT_FAILURE);
				break;
			case -1:
				break;
			default:   /* Unexpected stuff */
				abort();
		}
	} while (next_option != -1);

	if (show_debugfs)
		print_gpio_debugfs();

	fd = device_open(dev);

	/* No specific line specified, so select all */
	if (!nlines) {
		int i;

		nlines = device_get_lines(fd);
		for (i=0; i < nlines; i++)
			line_mask[i] = 1;
	}

	if (show_info)
		print_gpio_info(fd, line_mask);

	if (show_level)
		show_gpio_level(fd, line_mask);

	if (do_iotest)
		do_gpio_iotest(fd, line_mask, count, delay, timeout);

	if (do_evttest)
		do_gpio_evttest(fd, line_mask, count, delay, timeout);

	if (do_blink) {
		print_gpio_info(fd, line_mask);
		do_gpio_blink(fd, line_mask, count, delay);
	}

	if (set_level) {
		do_gpio_set_level(fd, line_mask, value);
	}

	device_close(fd, dev);

	exit(EXIT_SUCCESS);
}
